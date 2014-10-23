#include "gsmmodem.h"
#include "simpleatcommand.h"
#include "atchat.h"

GsmModem::GsmModem(AtChat * const chat, const QString &balanceUssdCommand, QObject *parent) :
    QObject(parent),
    m_atChat(chat),
    m_balanceCash(0.),
    m_balanceUssdCommand(balanceUssdCommand)
{
    init();
}

void GsmModem::onCommandHelper(AtCommand * const cmd, const QString &log)
{
    QString _log;
    if (cmd->getCommandResult().resultCode() == AtResult::OK) {
        _log = log.arg("Ok");
    } else {
        _log = log.arg("Error");
    }
    logMsg(_log);
    cmd->deleteLater();
}

void GsmModem::onInitCommand()
{
    AtCommand *cmd = qobject_cast<AtCommand*>(sender());
    if (cmd) {
        QString logStr = QString("init modem command '%0': %1").arg(cmd->atCommand().trimmed());
        onCommandHelper(cmd, logStr);
    }
}

void GsmModem::onRequestBalanceCommand()
{
    AtCommand *cmd = qobject_cast<AtCommand*>(sender());
    if (cmd) {
        QString logStr = QString("request balance ussd command: %1");
        onCommandHelper(cmd, logStr);
    }
}

void GsmModem::onNotification(const QString &notification, const QString &content)
{
    if (notification == QString("+CUSD:")) {
        // ballance?
        QRegExp re ("^\\s*(?:\\d+,\")([\\dA-F]+)(?:\",\\d+)");
        if (re.indexIn(content,0) >= 0)
        {
            QString text = re.cap(1);
            QByteArray a;
            bool ok;
            for (int i = 0; i < text.length() / 4; ++i)
            {
                a.append(text.mid(i*4 + 2, 2).toInt(&ok, 16));
                a.append(text.mid(i*4, 2).toInt(&ok, 16));
            }
            QString smsText = QString::fromUtf16((ushort*)a.data(), a.length() / 2);
            bool negative = false;
            // only for russian operators
            if (smsText.contains(tr("Минус")) || smsText.contains(tr("Отрицательный")))
                negative = true;
            QRegExp re("(?:w*)(-*\\d+[.,]\\d+)(?:.*)");
            if (re.indexIn(smsText,0) >= 0)
            {
                double temp = re.cap(1).toDouble();
                if (negative)
                    temp = temp * -1;
                m_balanceCash = temp;
                emit balanceCash(temp);
                logMsg(QString("balance: %0").arg(m_balanceCash));
            }
        }
    }
}

void GsmModem::init()
{
    QStringList initList({"ATE 0" // echo off
                         ,"AT+CMEE=1" //??????
                         ,"AT+CMGF=0"// Sms PDU
                         ,"AT+CGMI"
                         ,"AT+CGMM"
                         ,"AT+CGMR"
                         ,"AT+CGSN"
                         ,"AT+CNMI=2,1,0,1,0"
                         ,"AT+CLIP=1"
                         ,"AT+COPS?"
                         ,"AT+CREG?"});
    for (const QString &at : initList) {
        SimpleAtCommand *cmd = new SimpleAtCommand(at);
        connect(cmd, SIGNAL(isProcessed()), this, SLOT(onInitCommand()));
        m_atChat->addCommand(cmd);
    }
    m_atChat->registerNotification("+CUSD:");
    connect(m_atChat, SIGNAL(notification(QString,QString)), this, SLOT(onNotification(QString,QString)));
    requestBalance();
}

void GsmModem::logMsg(const QString msg)
{
    qDebug(QString("GsmModem: %0").arg(msg).toUtf8().data());
}

void GsmModem::requestBalance()
{
    if (!m_balanceUssdCommand.isEmpty()) {
        SimpleAtCommand *cmd = new SimpleAtCommand(QString("AT+CUSD=1,%0,15").arg(m_balanceUssdCommand));
        connect(cmd, SIGNAL(isProcessed()), this, SLOT(onRequestBalanceCommand()));
        m_atChat->addCommand(cmd);
    }
}
