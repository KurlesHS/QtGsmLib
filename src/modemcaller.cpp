#include "modemcaller.h"
#include "atchat.h"
#include "simpleatcommand.h"

ModemCaller::ModemCaller(AtChat * const chat, QObject *parent) :
    QObject(parent),
    m_chat(chat),
    m_secondBeforeHangupIncomingCall(10),
    m_secondBeforeHangupOutgoingCall(10),
    m_currentMode(NormalMode)
{
    m_chat->registerNotification("NO CARRIE");
}

void ModemCaller::call(const QString &number)
{
    SimpleAtCommand *cmd = new SimpleAtCommand(QString("ATD%0;").arg(number));
    cmd->setErrorResponce("NO CARRIER");
    connect(cmd, SIGNAL(isProcessed()), this, SLOT(onCallCommand()));
    connect(cmd, SIGNAL(startSendingOnModem()), this, SLOT(onStartCallCommand()));
    m_chat->addCommand(cmd);
}

void ModemCaller::onNotification(const QString &notification, const QString &content)
{

}

void ModemCaller::onCallCommand()
{
    SimpleAtCommand *cmd = qobject_cast<SimpleAtCommand*>(sender());
    if (cmd) {
        QString number = cmd->customData().toString();
        logMsg(QString("start dialing on '%0'").append(number));

    }
}

void ModemCaller::onStartCallCommand()
{
    SimpleAtCommand *cmd = qobject_cast<SimpleAtCommand*>(sender());
    if (cmd) {
        QString number = cmd->customData().toString();
        m_currentOutgingNumber = number;
        m_currentMode = OutgoingCall;
        logMsg(QString("start dialing on '%0'").append(number));
    }
}

void ModemCaller::logMsg(const QString &msg)
{
    qDebug(QString("ModemCaller: %0").arg(msg).toUtf8().data());
}
