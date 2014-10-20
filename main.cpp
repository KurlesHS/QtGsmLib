#include <QCoreApplication>
#include <QTextCodec>
#include <QDebug>

#include "smsmessage/qsmsmessage.hpp"

int main(int argc, char *argv[])
{
    QTextCodec *cyrillicCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(cyrillicCodec);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Win-1251"));
    QTextCodec::setCodecForCStrings(cyrillicCodec);
    QCoreApplication a(argc, argv);

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


    return 0;
}
