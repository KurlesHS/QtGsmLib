#include "simpleatcommand.h"
#include "atchat.h"

SimpleAtCommand::SimpleAtCommand(const QString &atCommand,
                                 QObject *parent) :
    AtCommand(atCommand, parent),
    m_afterSendCommandWaitResponseTimeout(1000)
{
    addErrorResponce("ERROR");
    addOkErrorResponce("OK");
}


bool SimpleAtCommand::processLine(QString line, AtChat * const chat)
{
    for (const QString &ok : m_okResponces) {
        if (line == ok) {
            setValid(true);
            return false;
        }
    }

    for (const QString &error : m_errorResponces) {
        if (line == error) {
            setValid(false);
            return false;
        }
    }

    if (!line.trimmed().isEmpty()) {
        atResult().appendToContent(line);
    }
    afterSendCommand(chat);
    return true;
}

AtResult SimpleAtCommand::getCommandResult()
{

    AtResult res = atResult();
    res.setResultCode(isValid() ? AtResult::OK : AtResult::Error);
    return res;
}


void SimpleAtCommand::afterSendCommand(AtChat * const chat)
{
    chat->startWaitDataTimeout(m_afterSendCommandWaitResponseTimeout);
}
QVariant SimpleAtCommand::customData() const
{
    return m_customData;
}

void SimpleAtCommand::setCustomData(const QVariant &customData)
{
    m_customData = customData;
}

void SimpleAtCommand::addErrorResponce(const QString &error)
{
    m_errorResponces.append(error);
}

void SimpleAtCommand::addOkErrorResponce(const QString &ok)
{
    m_okResponces.append(ok);
}
int SimpleAtCommand::afterSendCommandWaitResponseTimeout() const
{
    return m_afterSendCommandWaitResponseTimeout;
}

void SimpleAtCommand::setAfterSendCommandWaitResponseTimeout(int ms)
{
    m_afterSendCommandWaitResponseTimeout = ms;
}


