#ifndef ATCOMMAND_H
#define ATCOMMAND_H

#include <QObject>
#include <QDebug>

#include "atresult.h"

class AtChat;

class AtCommand : public QObject
{
    Q_OBJECT
public:
    explicit AtCommand(const QString atCommand,
                       QObject *parent = 0);
    virtual ~AtCommand() {}

    QString atCommand() const {return m_atCommand + "\r\n";}

    virtual void afterSendCommand(AtChat * const chat) = 0;
    virtual bool processLine(QString line, AtChat * const chat) = 0;
    virtual AtResult getCommandResult() = 0;

    virtual void onFailOnTimeoutReason(AtChat * const chat);

    QString errorResponce() const;
    void setErrorResponce(const QString &errorResponce);

    QString okResponce() const;
    void setOkResponce(const QString &okResponce);

    bool isValid() const {return m_isValid;}

    int waitDataTimeout() const;
    void setWaitDataTimeout(int waitDataTimeout);

    void informAboutStartSendingCommand() {emit startSendingOnModem();}

public slots:
    void setIsProcessed() {emit isProcessed();}
    void setItFailOnTimeoutReason(AtChat * const chat);

protected:
    AtResult &atResult() {return m_result;}
    void setAtCommand(const QString &atCommand) {m_atCommand = atCommand;}
    void setValid(const bool isValid) {m_isValid = isValid;}

signals:
    void startSendingOnModem();
    void isProcessed();

public slots:

private:
    QString m_atCommand;
    QString m_errorResponce;
    QString m_okResponce;
    int m_waitDataTimeout;
    bool m_isValid;
    AtResult m_result;
};

#endif // ATCOMMAND_H
