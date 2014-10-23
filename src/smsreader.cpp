#include "smsreader.h"
#include "atchat.h"
#include "simpleatcommand.h"
#include "smsmessage/qsmsmessage.hpp"
#include <QRegExp>
#include <QStringList>
#include <QDebug>

// Get the multipart identifier information for a message.
static bool getMultipartInfo( const QSMSMessage& msg, QString& multiId,
                              uint& part, uint& numParts )
{
    QByteArray headers = msg.headers();
    int posn = 0;
    uint tag;
    int len;
    while ( ( posn + 2 ) <= headers.size() ) {
        tag = (unsigned char)(headers[posn]);
        len = (unsigned char)(headers[posn + 1]);
        if ( ( posn + len + 2 ) > headers.size() )
            break;
        if ( tag == 0 && len >= 3 ) {
            // Concatenated message with 8-bit identifiers.
            multiId = msg.sender() + "&" +
                      QString::number
                        ( (uint)(unsigned char)(headers[posn + 2]) );
            numParts = (unsigned char)(headers[posn + 3]);
            part     = (unsigned char)(headers[posn + 4]);
            if ( numParts && part && part <= numParts )
                return true;
        } else if ( tag == 8 && len >= 4 ) {
            // Concatenated message with 16-bit identifiers.
            multiId = msg.sender() + "&" +
                      QString::number
                        ( ( (uint)(unsigned char)(headers[posn + 2]) << 8 ) +
                            (uint)(unsigned char)(headers[posn + 3]) );
            numParts = (unsigned char)(headers[posn + 4]);
            part     = (unsigned char)(headers[posn + 5]);
            if ( numParts && part && part <= numParts )
                return true;
        }
        posn += 2 + len;
    }
    return false;
}

SmsReader::SmsReader(AtChat * const chat, QObject *parent) :
    QObject(parent),
    m_atChat(chat)
{
    m_atChat->registerNotification("+CMTI:");
    connect(m_atChat, SIGNAL(notification(QString,QString)),
            this, SLOT(onNotification(QString,QString)));
    check();
}

void SmsReader::deleteSms(const int number)
{
    if (number >= count()) {
        return;
    }
    QSMSMessage msg = getSmsMessage(number);
    for (const int numSmsInMemory : msg.messageIds()) {
        SimpleAtCommand *cmd = new SimpleAtCommand(QString("AT+CMGD=%0").arg(numSmsInMemory));
        connect(cmd, SIGNAL(isProcessed()), this, SLOT(onDeleteCommand()));
        m_atChat->addCommand(cmd);
    }
    m_incomingMessages.removeAt(number);
}

int SmsReader::count()
{
    return m_incomingMessages.count();
}

QSMSMessage SmsReader::getSmsMessage(int number)
{
    if (number < count()) {
        return m_incomingMessages.at(number);
    }
    return QSMSMessage();
}

void SmsReader::check()
{
    if (!m_atChat->isOpen()) {
        return;
    }
    if (m_storage.isEmpty()) {
        getStorage();
        return;
    }
    SimpleAtCommand *cmd = new SimpleAtCommand("AT+CMGL=4");
    cmd->setWaitDataTimeout(2000);
    connect(cmd, SIGNAL(isProcessed()), this, SLOT(onRequestSmsCommand()));
    m_atChat->addCommand(cmd);
}

void SmsReader::getStorage()
{
    if (m_atChat->isOpen()){
        SimpleAtCommand *cmd = new SimpleAtCommand("AT+CPMS?");
        connect(cmd, SIGNAL(isProcessed()), this, SLOT(onCpmsCommand()));
        m_atChat->addCommand(cmd);
    }
}

void SmsReader::logMsg(const QString msg)
{
    qDebug(QString("SmsReader: %0").arg(msg).toUtf8().data());
}

void SmsReader::onCpmsCommand()
{
    QString defaultStorage("SM");
    QString preferedStorage = "MT";
    AtCommand *cmd = qobject_cast<AtCommand*>(sender());
    if (cmd) {
        AtResult res = cmd->getCommandResult();
        if (res.resultCode() == AtResult::OK) {
            // expected responce like "+CPMS: "MT",6,100,"ME",6,100,"SM",0,20"
            QRegExp cpmsParser("^\\+CPMS:\\s*\\\"(\\w{2})\\\"\\s*,\\s*\\d+\\s*,\\s*\\d+,\\s*\\\"(\\w{2})\\\"\\s*,\\s*\\d+,\\s*\\d+\\s*,\\\"(\\w{2})\\\"\\s*,\\s*\\d+\\s*,\\s*\\d+");
            if (cpmsParser.indexIn(res.content()) >= 0) {
                for (int i = 1; i <= 3; ++i) {{
                        if (cpmsParser.cap(i).toUpper() == preferedStorage) {
                            defaultStorage = preferedStorage;
                            break;
                        }
                    }
                }
            }
        }
        cmd->deleteLater();
    }
    m_storage = defaultStorage;
    logMsg(QString("trying set default sms storage to \"%0\"...").arg(m_storage));

    cmd = new SimpleAtCommand(QString("AT+CPMS=\"%0\"").arg(m_storage));
    connect(cmd, SIGNAL(isProcessed()), this, SLOT(onSetDefaultStorageCommand()));
    m_atChat->addCommand(cmd);

}

void SmsReader::onSetDefaultStorageCommand()
{
    QString logMsg("Failed to set default storage...");
    AtCommand *cmd = qobject_cast<AtCommand*>(sender());
    if (cmd) {
        if (cmd->getCommandResult().resultCode() == AtResult::OK) {
            logMsg = QString("default storage is set to \"%0\"").arg(m_storage);
        }
        cmd->deleteLater();
    }
    this->logMsg(logMsg);
    check();
}

void SmsReader::onRequestSmsCommand()
{
    AtCommand *cmd = qobject_cast<AtCommand*>(sender());
    if (cmd) {
        QHash<QString, QList<QSMSMessage> > multiSms;
        AtResult res = cmd->getCommandResult();
        if (res.resultCode() == AtResult::OK) {
            m_incomingMessages.clear();
            QStringList lines = res.content().split("\n");
            int messageId = -1;
            QRegExp messageIdExtractor("^\\+CMGL:\\s*(\\d+)");
            Q_FOREACH(const QString &line, lines) {
                if (messageId < 0) {
                    // expected "CMGL: 0,1,,123", where first number is messageId
                    if (messageIdExtractor.indexIn(line) >= 0) {
                        bool ok;
                        messageId = messageIdExtractor.cap(1).toInt(&ok);
                        if (!ok) {
                            messageId = -1;
                        }
                    }
                } else {
                    // expected pdu
                    QSMSMessage smsMsg = QSMSMessage::fromPdu(QByteArray::fromHex(line.toLatin1()));
                    if (!smsMsg.sender().isEmpty()) {
                        QString multiId;
                        uint part;
                        uint partCount;
                        smsMsg.setMessageIds({messageId});
                        if (getMultipartInfo(smsMsg, multiId, part, partCount)) {
                            smsMsg.setMessagePartNumber(part);
                            multiSms[multiId].append(smsMsg);
                        } else {
                            m_incomingMessages.append(smsMsg);
                        }
                    }
                    messageId = -1;
                }
            }
            QHashIterator<QString, QList<QSMSMessage> > it(multiSms);
            while (it.hasNext()) {
                it.next();
                QList<QSMSMessage> mess = it.value();
                if (mess.count() > 1) {
                    QString multiId;
                    uint part;
                    uint partCount;
                    if (getMultipartInfo(mess.at(0), multiId, part, partCount)) {
                        if (partCount == (uint)mess.count()) {
                            // we have all part of sms message
                            QList<int> smsIds;
                            qSort(mess.begin(), mess.end(), [](const QSMSMessage &m1, const QSMSMessage &m2) {
                                return m1.messagePartNumber() < m2.messagePartNumber();
                            });
                            QString smsText;
                            QSMSMessage newMes;
                            for (const QSMSMessage &smsMess : mess) {
                                smsIds.append(smsMess.messageIds().at(0));
                                smsText.append(smsMess.text());
                                newMes.setTimestamp(smsMess.timestamp());
                                newMes.setSender(smsMess.sender());
                            }
                            newMes.setText(smsText);
                            newMes.setMessageIds(smsIds);
                            m_incomingMessages.append(newMes);
                        }
                    }
                }
            }
            logMsg(QString("Request sms command: we have %0 sms").arg(m_incomingMessages.count()));
            if (count() > 0) {
                emit incomingSms();
            }
        } else {
            logMsg(QString("Request sms command fail"));
        }
    }    
}

void SmsReader::onDeleteCommand()
{
    AtCommand *cmd = qobject_cast<AtCommand*>(sender());
    bool isOk = false;
    if (cmd) {
        if (cmd->getCommandResult().resultCode() == AtResult::OK) {
            isOk = true;
        }
    }
    if (isOk){
        logMsg(QString("remove sms from modem memory command executed sucessfuly"));
    } else {
        logMsg(QString("remove sms from modem memory command return error code"));
    }
}

void SmsReader::onNotification(const QString &notification, const QString &content)
{
    Q_UNUSED(content)
    if (notification == QString("+CMTI:")) {
        check();
    }
}
