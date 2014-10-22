#ifndef SIMPLEATCOMMAND_H
#define SIMPLEATCOMMAND_H

#include "atcommand.h"

class SimpleAtCommand : public AtCommand
{
    Q_OBJECT
public:
    explicit SimpleAtCommand(const QString &atCommand, QObject *parent = 0);

    // AtCommand interface
public:
    virtual bool processLine(QString line, AtChat * const chat);
    virtual AtResult getCommandResult();
    virtual void afterSendCommand(AtChat * const chat);

signals:

public slots:

private:
    QString m_content;
};

#endif // SIMPLEATCOMMAND_H
