#ifndef SMSREADER_H
#define SMSREADER_H

#include "smsmessage/qsmsmessage.hpp"

#include <QObject>
#include <QList>

class AtChat;

// +CMTI: "SM",1 - incomming sms

class SmsReader : public QObject
{
    Q_OBJECT
public:
    explicit SmsReader(AtChat * const chat, QObject *parent = 0);

    void deleteSms(const int number);
    QSMSMessage getSmsMessage(int number);
    void init();

    int count();

signals:
    void incomingSms();

public slots:
    void check();


private:
    void getStorage();
    void logMsg(const QString msg);
    void joinMessages();

private slots:
    void onCpmsCommand();
    void onSetDefaultStorageCommand();
    void onRequestSmsCommand();
    void onDeleteCommand();

    void onNotification(const QString &notification, const QString &content);

private:
    AtChat *m_atChat;
    QString m_storage;
    QList<QSMSMessage> m_incomingMessages;

};

#endif // SMSREADER_H
