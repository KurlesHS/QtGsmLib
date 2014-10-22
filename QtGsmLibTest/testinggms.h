#ifndef TESTINGGMS_H
#define TESTINGGMS_H

#include <QObject>
#include <QByteArray>
#include <QDataStream>
#include <gtest/gtest.h>
#include "smsreader.h"
#include "atchat.h"


class TestingGms : public QObject, public ::testing::Test
{
    Q_OBJECT
public:
    explicit TestingGms(QObject *parent = 0);

    void SetUp();
    void TearDown();



    QByteArray inArray() const {return m_inBytearray;}
    QByteArray outArray() const {return m_outBytearray;}



signals:

public slots:
   void testSomething();

protected:
    QDataStream *m_outDataStream;
    QDataStream *m_inDataStream;
    QByteArray m_inBytearray;
    QByteArray m_outBytearray;
    SmsReader *m_smsReader;
    AtChat *m_atChat;
};

#endif // TESTINGGMS_H
