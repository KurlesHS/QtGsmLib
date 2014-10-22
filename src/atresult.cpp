#include "atresult.h"

AtResult::AtResult(QObject *parent) :
    QObject(parent),
    m_resultCode(OK),
    m_isValid(true)
{

}

AtResult::AtResult(const AtResult &atResult) :
    QObject()
{
    this->setContent(atResult.content());
    this->setValid(atResult.isValid());
    this->setResultCode(atResult.resultCode());
}

QString AtResult::content() const
{
    return m_content;
}

void AtResult::setContent(const QString &result)
{
    m_content = result;
}

void AtResult::appendToContent(const QString &line)
{
    if (m_content.isEmpty()) {
        m_content = line;
    } else {
        m_content += ("\n" + line);
    }
}

bool AtResult::isValid() const
{
    return m_isValid;
}

void AtResult::setValid(const bool isValid)
{
    m_isValid = isValid;
}
AtResult::ResultCode AtResult::resultCode() const
{
    return m_resultCode;
}

void AtResult::setResultCode(const ResultCode &resultCode)
{
    m_resultCode = resultCode;
}




