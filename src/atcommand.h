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

    virtual QString secondAtCommand() const {return QString();}
    virtual void onFailOnTimeoutReason();

    QString errorResponce() const;
    void setErrorResponce(const QString &errorResponce);

    QString okResponce() const;
    void setOkResponce(const QString &okResponce);

    bool isValid() const {return m_isValid;}

public slots:
    void setIsProcessed() {emit isProcessed();}
    void setItFailOnTimeoutReason();

protected:
    AtResult &atResult() {return m_result;}
    void setAtCommand(const QString &atCommand) {m_atCommand = atCommand;}
    void setValid(const bool isValid) {m_isValid = isValid;}

signals:
    void isProcessed();

public slots:

private:
    QString m_atCommand;
    QString m_errorResponce;
    QString m_okResponce;
    bool m_isValid;
    AtResult m_result;
};

#endif // ATCOMMAND_H
