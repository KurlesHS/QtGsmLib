#include <QCoreApplication>
#include <QTextCodec>
#include <QDebug>
#include <atchat.h>
#include <QSerialPort>
#include <simpleatcommand.h>
#include "main.h"

#include "smsmessage/qsmsmessage.hpp"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec *cyrillicCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(cyrillicCodec);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Win-1251"));
    QTextCodec::setCodecForCStrings(cyrillicCodec);


    QSMSMessage msg1;
    msg1.setRecipient("+71234567890");
    msg1.setText(QObject::trUtf8("hello!"));
    msg1.setReplyRequest(true);
    qDebug() << msg1.toPdu().toHex();

    QSMSMessage msg2 = QSMSMessage::fromPdu(QByteArray::fromHex("07911326040000F0040B911346610089F60000208062917314080CC8F71D14969741F977FD07"));
    qDebug() << msg2.text();
    qDebug() << msg2.sender();
    qDebug() << msg2.messageType();
    qDebug() << msg2.timestamp();

    QSerialPort serialPort;
    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);
    serialPort.setPortName("COM3");
    if (!serialPort.open(QIODevice::ReadWrite)) {
        return -1;
    }
    qDebug() << "port is open";
    TestClass t(&serialPort);
    t.run();

    return a.exec();
}
