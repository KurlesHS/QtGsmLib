#ifndef MODEMCALLER_H
#define MODEMCALLER_H

#include <QObject>

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

public slots:
    void call(const QString &number);

signals:
    void incomingRing(const QString &phoneNumber);
    void outgoingCall(const QString &phoneNumber, bool status);

private:
    void logMsg(const QString &msg);

private slots:
    void onNotification(const QString &notification, const QString &content);
    void onCallCommand();
    void onStartCallCommand();



private:
    AtChat *m_chat;
    int m_secondBeforeHangupIncomingCall;
    int m_secondBeforeHangupOutgoingCall;
    QString m_currentIncomingNumber;
    QString m_currentOutgingNumber;
    mode m_currentMode;

};

#endif // MODEMCALLER_H
