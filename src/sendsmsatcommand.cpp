#include "sendsmsatcommand.h"
#include "atchat.h"

SendSmsAtCommand::SendSmsAtCommand(const QString& pdu, const QString& id, QObject* parent)
    : AtCommand(QString("AT+CMGS=%0").arg(pdu.length() / 2 - 1), parent)
    , m_smsId(-1)
    , m_isLastPart(true)
    , m_id(id)
    , m_pdu(pdu)

{
}

void SendSmsAtCommand::afterSendCommand(AtChat* const chat)
{
    chat->startWaitDataTimeout(1000);
}

bool SendSmsAtCommand::processLine(QString line, AtChat* const chat)
{
    if (line == QString("> ")) {
        QByteArray toSend = m_pdu.toLatin1();
        toSend.append(0x1a);
        chat->writeRawData(toSend);
        // wait response 50 seconds
        chat->startWaitDataTimeout(50000);
        return true;
    }
    else if (line.toLower().contains("error")) {
        setValid(false);
        return false;
    }
    else if (line.toLower().trimmed() == QString("ok")) {
        setValid(true);
        return false;
    }
    else if (line.startsWith(QString("+CMGS: "))) {
        QString numStr = line.mid(6).trimmed();
        bool ok;
        int smsId = numStr.toInt(&ok);
        if (ok) {
            m_smsId = smsId;
        }
    }
    chat->startWaitDataTimeout(1000);
    return true;
}

AtResult SendSmsAtCommand::getCommandResult()
{
    AtResult res = atResult();
    res.setResultCode(isValid() ? AtResult::OK : AtResult::Error);
    return res;
}

void SendSmsAtCommand::onFailOnTimeoutReason(AtChat* const chat)
{
    QByteArray subtitutes;
    for (int i = 0; i < 10; ++i) {
        subtitutes.append(0x1a);
    }
    // try to restore modem status
    chat->writeRawData(subtitutes);
    AtCommand::onFailOnTimeoutReason(chat);
}
QString SendSmsAtCommand::fullSmsText() const
{
    return m_fullSmsText;
}

void SendSmsAtCommand::setFullSmsText(const QString &fullSmsText)
{
    m_fullSmsText = fullSmsText;
}
QString SendSmsAtCommand::recepient() const
{
    return m_recepient;
}

void SendSmsAtCommand::setRecepient(const QString &recepient)
{
    m_recepient = recepient;
}


