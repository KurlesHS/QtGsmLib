#include "atcommand.h"
#include "atchat.h"

AtCommand::AtCommand(const QString atCommand, QObject *parent) :
    QObject(parent),
    m_atCommand(atCommand),
    m_waitDataTimeout(200),
    m_isValid(true)
{

}

void AtCommand::onFailOnTimeoutReason(AtChat * const chat)
{
    Q_UNUSED(chat)
    m_isValid = false;
    emit isProcessed();
}

void AtCommand::setItFailOnTimeoutReason(AtChat * const chat)
{
    onFailOnTimeoutReason(chat);
}
int AtCommand::waitDataTimeout() const
{
    return m_waitDataTimeout;
}

void AtCommand::setWaitDataTimeout(int waitDataTimeout)
{
    m_waitDataTimeout = waitDataTimeout;
}





