#include "atcommand.h"
#include "atchat.h"

AtCommand::AtCommand(const QString atCommand, QObject *parent) :
    QObject(parent),
    m_atCommand(atCommand),
    m_errorResponce("ERROR"),
    m_okResponce("OK"),
    m_isValid(true)
{

}

void AtCommand::onFailOnTimeoutReason()
{
    m_isValid = false;
    emit isProcessed();
}

void AtCommand::setItFailOnTimeoutReason()
{
    onFailOnTimeoutReason();
}
QString AtCommand::okResponce() const
{
    return m_okResponce;
}

void AtCommand::setOkResponce(const QString &okResponce)
{
    m_okResponce = okResponce;
}

QString AtCommand::errorResponce() const
{
    return m_errorResponce;
}

void AtCommand::setErrorResponce(const QString &errorResponce)
{
    m_errorResponce = errorResponce;
}





