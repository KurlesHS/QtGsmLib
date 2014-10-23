#ifndef GSMMODEM_H
#define GSMMODEM_H

#include "atcommand.h"

#include <QObject>

class AtChat;

class GsmModem : public QObject
{
    Q_OBJECT
public:
    explicit GsmModem(AtChat * const chat,
                      const QString &balanceUssdCommand = "*102#",
                      QObject *parent = 0);
    double balanceCash() {return m_balanceCash;}

    void onCommandHelper(QString log, AtCommand *cmd);
    void onCommandHelper(AtCommand * const cmd, const QString &log);
private:
    void init();
    void logMsg(const QString msg);

signals:
    void balanceCash(const double balance);

public slots:
    void requestBalance();

private slots:
    void onInitCommand();
    void onRequestBalanceCommand();
    void onNotification(const QString &notification, const QString &content);

private:
    AtChat *m_atChat;
    double m_balanceCash;
    QString m_balanceUssdCommand;

};

#endif // GSMMODEM_H
