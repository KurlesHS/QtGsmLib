#ifndef SIMPLEATCOMMAND_H
#define SIMPLEATCOMMAND_H

#include "atcommand.h"

#include <QStringList>

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
    QVariant customData() const;
    void setCustomData(const QVariant &customData);
    void addErrorResponce(const QString &error);
    void addOkErrorResponce(const QString &ok);

signals:

public slots:

private:
    QString m_content;
    QVariant m_customData;
    QStringList m_okResponces;
    QStringList m_errorResponces;
};

#endif // SIMPLEATCOMMAND_H
