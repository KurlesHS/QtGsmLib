#ifndef ATCHAT_H
#define ATCHAT_H

#include "atcommand.h"

#include <QObject>
#include <QIODevice>
#include <QStringList>
#include <QTimer>

class AtChat : public QObject
{
    Q_OBJECT
public:
    explicit AtChat(QIODevice *readDevice, QIODevice *writeDevice = 0, QObject *parent = 0);
    void addCommand(AtCommand * const command);
    void writeRawData(const QByteArray &data);
    void setWaitDataTimeout(const int ms);
    bool isOpen() const;
    void registerNotification(const QString &notification);

private:
    void init();
    void read();
    void processLine(QString line);
    void processCurrentCommand();

    void logMsg(const QString &msg);

signals:
    void notification(QString notification, QString content);

public slots:

private slots:
    void onReadyRead();
    void onTimeout();
    void nextCommand();

private:
    QIODevice *m_readDevice;
    QIODevice *m_writeDevice;
    QString m_currentLine;
    QList<AtCommand*> m_listOfCommand;
    QStringList m_notifications;
    AtCommand *m_currentCommand;
    QTimer m_timeoutTimer;
    bool m_isBusy;
};

#endif // ATCHAT_H
