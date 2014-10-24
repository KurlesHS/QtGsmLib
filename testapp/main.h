#ifndef MAIN_H
#define MAIN_H

#include <QIODevice>
#include <QDebug>

#include "simpleatcommand.h"
#include "atchat.h"
#include "smsreader.h"
#include "smssender.h"
#include "gsmmodem.h"
#include "modemcaller.h"

#include <QTimer>

class TestClass : public QObject
{
    Q_OBJECT
public:
    TestClass(IModemTransport *device) :
        QObject(),
        chat(device),
        modem(&chat),
        smsReader(&chat),
        smsSender(&chat),
        modemCaller(&chat)
    {
        connect(&smsReader, SIGNAL(incomingSms()),
                this, SLOT(onIncomingSms()));
    }

public slots:

    void isProcessed() {
        AtCommand *cmd = qobject_cast<AtCommand*>(sender());
        if (cmd) {
            static int count= 0;
            AtResult res = cmd->getCommandResult();
            //qDebug() << ++count << Q_FUNC_INFO << res.resultCode() <<  res.content();
            cmd->deleteLater();
            QTimer::singleShot(2000, this, SLOT(isProcessed()));
            return;
        }
        cmd = getCommand();
        chat.addCommand(cmd);
    }

    void onIncomingSms() {
        qDebug() << Q_FUNC_INFO;
        for (int i = 0; i < smsReader.count(); ++i) {
            QSMSMessage sms = smsReader.getSmsMessage(i);
            qDebug() << sms.sender() << sms.text();
        }
        //while (smsReader.count() > 0) {
            //smsReader.deleteSms(0);
        //}
    }

    SimpleAtCommand *getCommand() {
        SimpleAtCommand *command = new SimpleAtCommand("at+cgmi");
        connect(command, SIGNAL(isProcessed()),
                this, SLOT(isProcessed()));
        return command;
    }
    void run() {
        isProcessed();
        QTimer::singleShot(5000, this, SLOT(onStartSendSms()));
    }

    void onStartSendSms() {
        qDebug() << Q_FUNC_INFO;
        modemCaller.call("+79646710340");
        //smsSender.sendSms("213123", "+79646710340", "По идее это должена быть длинная смс, которая не помещается в одной как минимум");
    }

private:
    AtChat chat;
    GsmModem modem;
    SmsReader smsReader;
    SmsSender smsSender;
    ModemCaller modemCaller;
};

#endif // MAIN_H
