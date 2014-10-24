#include "smssender.h"
#include "atchat.h"
#include "sendsmsatcommand.h"

SmsSender::SmsSender(AtChat *const chat, QObject *parent)
    : QObject(parent)
    , m_atChat(chat)
{
    m_atChat->registerPduNotification("+CDS:");
    connect(m_atChat, SIGNAL(pduNotification(QString,QString)),
            this, SLOT(onPduNotification(QString,QString)));
}

void SmsSender::sendSms(const QString &smsUuid, const QString &recipient,
                        const QString &smsText)
{
    QSMSMessage sms;
    sms.setRecipient(recipient);
    sms.setText(smsText);
    sms.setReplyRequest(true);
    if (sms.shouldSplit()) {
        QList<QSMSMessage> messagesList = sms.split();
        int lastPart = messagesList.count() - 1;
        for (int i = 0; i < messagesList.count(); ++i) {
            QSMSMessage msg = messagesList.at(i);
            addCommandToQueue(msg, smsText, smsUuid, i == lastPart);
        }
    }
    else {
        addCommandToQueue(sms, smsText, smsUuid, true);
    }
}

void SmsSender::addCommandToQueue(const QSMSMessage &msg,
                                  const QString &fullSmsText,
                                  const QString &smsUuid,
                                  const bool isLastPath)
{
    QByteArray pdu = msg.toPdu();
    if (isLastPath) {
        addDeliveryReportRequestToPdu(pdu);
    }
    SendSmsAtCommand *cmd = new SendSmsAtCommand(pdu.toHex(), smsUuid);
    cmd->setLastPart(isLastPath);
    if (isLastPath) {
        cmd->setFullSmsText(fullSmsText);
        cmd->setRecepient(msg.recipient());
    }
    connect(cmd, SIGNAL(isProcessed()), this, SLOT(onSendSmsCommnand()));
    m_atChat->addCommand(cmd);
}

void SmsSender::onSendSmsCommnand()
{
    SendSmsAtCommand *cmd = qobject_cast<SendSmsAtCommand *>(sender());
    if (cmd)  {
        bool isSended = cmd->getCommandResult().resultCode() == AtResult::OK;
        logMsg(QString("send sms command: %0")
               .arg(isSended ? "Ok" : "Error"));
        if (cmd->isLastPart()) {
            emit smsSended(cmd->smsUuid(), isSended);
            QString log = QString("sms '%0' to '%1' %2 sended")
                    .arg(cmd->fullSmsText(), cmd->recepient(), isSended ? "is" : "isn't");
            logMsg(log);
            if (isSended) {
                SmsInfo si;
                si.smsText = cmd->fullSmsText();
                si.recepient = cmd->recepient();
                si.smsUuid = cmd->smsUuid();
                m_hashSendedSms[cmd->smsId()] = si;
            }
        }
    }
}

void SmsSender::onPduNotification(const QString &notification,
                                  const QString &content)
{
    if (notification == QString("+CDS:")) {
        while (true) {
            QByteArray pdu = QByteArray::fromHex(content.toLatin1());
            int size = pdu.size();
            int index = 1;
            if (size <= index) {
                break;
            }
            // skip sms center number
            index += pdu[0] & 0xff;
            // skip flags
            ++index;
            if (size <= index) {
                return;
            }

            int internalModemSmsId = pdu[index] & 0xff;
            if (m_hashSendedSms.contains(internalModemSmsId)) {
                SmsInfo si = m_hashSendedSms.value(internalModemSmsId);
                emit smsDelivered(si.smsUuid, true);
                m_hashSendedSms.remove(internalModemSmsId);
                logMsg(QString("message '%0' is delivered to '%1'")
                       .arg(si.smsText, si.recepient));
            }
            break;
        }
    }
}

void SmsSender::logMsg(const QString msg)
{
    qDebug(QString("[SmsSender] %0").arg(msg).toUtf8().data());
}

void SmsSender::addDeliveryReportRequestToPdu(QByteArray &pdu)
{
    int size = pdu.size();
    if (size < 2) {
        return;
    }
    int pduHeaderIndex = pdu.at(0) % 0xff;
    ++pduHeaderIndex;
    if (pduHeaderIndex >= size) {
        return;
    }
    pdu[pduHeaderIndex] = pdu[pduHeaderIndex] | 0x20;
}
