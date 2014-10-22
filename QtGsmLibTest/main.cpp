#include <QCoreApplication>

#include <QTextCodec>
#include <QTimer>
#include <QDebug>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "testinggms.h"
#include "smsmessage/qsmsmessage.hpp"

TEST(QtGsmLibTest, TestSmsMessage) {

    QSMSMessage sms;
    sms.setText(QObject::tr("Привет"));
    sms.setReplyRequest(true);
    ASSERT_EQ(sms.toPdu().toHex().size() > 0, true);

    // this is delivery report pdu
    sms = QSMSMessage::fromPdu(QByteArray::fromHex("07919730071111F1068E0B919746760143F0410112512394614101125123156100"));
    int xx = -127;
    xx <<= 4;
    for (int i = 0; i < 0xfff; ++i) {
        uint tmp;
        uint tmp2 = (uint)i;
        if(tmp2 & 0x800) tmp = tmp2 - 0x800;
        else             tmp = 0x800 - tmp2;
        tmp >>= 2;
        if(tmp2 & 0x800) tmp = 0x80 + tmp;
        else             tmp = 0x80 - tmp;


        int t = i;
        t -= 0x800;
        if (t < 0 && t & 0x03) {
            t += 4;
        }
        t >>= 2;
        t += 0x80;

        uint t2 = (uint)t & 0x3ff;
        ASSERT_EQ (t2 , tmp &0x3ff);



    }
    /*
    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), qApp, SLOT(quit()));
    timer.start(2000);
    qDebug() << qApp;
    qApp->exec();
    */
}

TEST_F(TestingGms, test1) {
    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), qApp, SLOT(quit()));

    timer.start(2000);
    QTimer::singleShot(1, this, SLOT(testSomething()));
    qApp->exec();
    qDebug() << inArray().length();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << &a;

    QTextCodec *cyrillicCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(cyrillicCodec);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Win-1251"));
    QTextCodec::setCodecForCStrings(cyrillicCodec);
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
