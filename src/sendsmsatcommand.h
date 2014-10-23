#ifndef SENDSMSATCOMMAND_H
#define SENDSMSATCOMMAND_H

#include "atcommand.h"

class SendSmsAtCommand : public AtCommand
{
    Q_OBJECT
public:
    explicit SendSmsAtCommand(const QString &pdu, const QString &smsUuid, QObject *parent = 0);
    int smsId() const {return m_smsId;}
    bool isLastPart() const {return m_isLastPart;}
    void setLastPart(const bool lastPart) {m_isLastPart = lastPart;}
    QString smsUuid() const {return m_id;}

    // AtCommand interface
public:
    virtual void afterSendCommand(AtChat * const chat);
    virtual bool processLine(QString line, AtChat * const chat);
    virtual AtResult getCommandResult();
    virtual void onFailOnTimeoutReason(AtChat * const chat);

    QString fullSmsText() const;
    void setFullSmsText(const QString &fullSmsText);

    QString recepient() const;
    void setRecepient(const QString &recepient);

signals:

public slots:

private:
    int m_smsId;
    bool m_isLastPart;
    QString m_id;
    QString m_pdu;
    QString m_fullSmsText;
    QString m_recepient;


};

#endif // SENDSMSATCOMMAND_H
