#ifndef MODEMCALLER_H
#define MODEMCALLER_H

#include <QObject>
#include <QTimer>

class AtChat;

class ModemCaller : public QObject
{
    Q_OBJECT
    enum mode {
        NormalMode,
        IncomingCall,
        OutgoingCall
    };

public:
    explicit ModemCaller(AtChat * const chat, QObject *parent = 0);
    void hangup();

    int secondBeforeHangupIncomingCall() const;
    void setSecondBeforeHangupIncomingCall(int secondBeforeHangupIncomingCall);

    int secondBeforeHangupOutgoingCall() const;
    void setSecondBeforeHangupOutgoingCall(int secondBeforeHangupOutgoingCall);

public slots:
    void call(const QString &number);

signals:
    void incomingRing(const QString &phoneNumber);
    void outgoingCall(const QString &phoneNumber, bool status);

private:
    void logMsg(const QString &msg);
    void handleRingNotification(const QString &content);
    void handleNoCarrierNotification();
    void stopTimer();


private slots:
    void onNotification(const QString &notification, const QString &content);
    void onCallCommand();
    void onStartCallCommand();
    void onTimer();
    void onHangupCommand();



private:
    AtChat *m_chat;
    int m_secondBeforeHangupIncomingCall;
    int m_secondBeforeHangupOutgoingCall;
    QString m_currentIncomingNumber;
    QString m_currentOutgingNumber;
    QTimer m_timer;
    quint64 m_lastHangupTime;
    QString m_lastHangupPhoneNum;
    mode m_currentMode;

};

#endif // MODEMCALLER_H
