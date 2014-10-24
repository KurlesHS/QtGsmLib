#ifndef ATCHAT_H
#define ATCHAT_H

#include "atcommand.h"
#include "imodemtransport.h"

#include <QObject>
#include <QIODevice>
#include <QStringList>
#include <QTimer>

class AtChat : public QObject
{
    Q_OBJECT
public:
    explicit AtChat(IModemTransport *modemTransport, QObject *parent = 0);
    void addCommand(AtCommand * const command);
    void writeRawData(const QByteArray &data);
    void startWaitDataTimeout(const int ms);
    void stopWaitDataTimer();
    bool isOpen() const;
    void registerNotification(const QString &notification);
    void registerPduNotification(const QString &notification);

private:
    void init();
    void read();
    bool processLine(QString line);

    void logMsg(const QString &msg);
    void nextCommand();

signals:
    void notification(QString notification, QString content);
    void pduNotification(QString notification, QString content);

public slots:

private slots:
    void onReadyRead();
    void onTimeout();
    void nextCommandSlot();
    void processCurrentCommand();

private:
    IModemTransport *m_modemTransport;
    QString m_currentLine;
    QString m_expectedPduNotification;
    QList<AtCommand*> m_listOfCommand;
    QStringList m_notifications;
    QStringList m_pduNotifications;
    AtCommand *m_currentCommand;
    QTimer m_timeoutTimer;
    bool m_isBusy;

};

#endif // ATCHAT_H
