#ifndef SMSSENDER_H
#define SMSSENDER_H

#include "smsmessage/qsmsmessage.hpp"

#include <QObject>
#include <QHash>

class AtChat;

class SmsSender : public QObject
{
    Q_OBJECT
    struct SmsInfo {
        QString smsUuid;
        QString recepient;
        QString smsText;
    };

public:
    explicit SmsSender(AtChat * const chat, QObject *parent = 0);
    void sendSms(const QString &smsUuid, const QString &recipient, const QString &smsText);

signals:
    void smsSended(const QString &smsId, const bool sended);
    void smsDelivered(const QString &smsIs, const bool delivered);

private:
    void addCommandToQueue(const QSMSMessage &msg,
                           const QString &fullSmsText,
                           const QString &smsUuid,
                           bool isLastPath);
    void logMsg(const QString msg);
    void addDeliveryReportRequestToPdu(QByteArray &pdu);

private slots:
    void onSendSmsCommnand();
    void onPduNotification(const QString &notification, const QString &content);

private:
    AtChat *m_atChat;
    QHash<int, SmsInfo> m_hashSendedSms;


};

#endif // SMSSENDER_H
