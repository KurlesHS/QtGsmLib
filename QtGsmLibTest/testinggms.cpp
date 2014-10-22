#include "testinggms.h"

TestingGms::TestingGms(QObject *parent) :
    QObject(parent)
{
}

void TestingGms::SetUp()
{
    m_inBytearray.clear();
    m_outBytearray.clear();
    m_inDataStream = new QDataStream(&m_inBytearray, QIODevice::ReadWrite);
    m_outDataStream = new QDataStream(&m_outBytearray, QIODevice::ReadWrite);
    m_atChat = new AtChat(m_inDataStream->device(), m_outDataStream->device());
    m_smsReader = new SmsReader(m_atChat);
}

void TestingGms::TearDown()
{
    delete m_smsReader;
    delete m_atChat;
    delete m_inDataStream;
    delete m_outDataStream;
}

void TestingGms::testSomething()
{
    QByteArray ok("OK\r\n");
    m_inDataStream->writeBytes(ok, ok.length());
}
