#include "modemcaller.h"
#include "atchat.h"
#include "simpleatcommand.h"

#include <QDateTime>

static const char *noCarrierStr = "NO CARRIER";
static const char *ringNotification = "+CLIP:";

ModemCaller::ModemCaller(AtChat *const chat, QObject *parent)
    : QObject(parent)
    , m_chat(chat)
    , m_secondBeforeHangupIncomingCall(5)
    , m_secondBeforeHangupOutgoingCall(5)
    , m_lastHangupTime(0)
    , m_currentMode(NormalMode)
{
    m_chat->registerNotification(noCarrierStr);
    m_chat->registerNotification(ringNotification);
    connect(m_chat, SIGNAL(notification(QString, QString)),
            this, SLOT(onNotification(QString, QString)));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    m_timer.setSingleShot(true);
}

void ModemCaller::hangup()
{
    if (m_chat->isOpen()){
        SimpleAtCommand *cmd = new SimpleAtCommand("ATH");
        connect(cmd, SIGNAL(isProcessed()), this, SLOT(onHangupCommand()));
        m_chat->addCommand(cmd);
    }
}

void ModemCaller::call(const QString &number)
{
    if (m_chat->isOpen()){
        SimpleAtCommand *cmd = new SimpleAtCommand(QString("ATD%0;").arg(number));
        cmd->addErrorResponce(noCarrierStr);
        cmd->setCustomData(number);
        cmd->setAfterSendCommandWaitResponseTimeout(15000);
        connect(cmd, SIGNAL(isProcessed()), this, SLOT(onCallCommand()));
        connect(cmd, SIGNAL(startSendingOnModem()), this, SLOT(onStartCallCommand()));
        m_chat->addCommand(cmd);
    }
}

void ModemCaller::onNotification(const QString &notification, const QString &content)
{
    if (notification == noCarrierStr) {
        handleNoCarrierNotification();
    } else if (notification == ringNotification) {
        handleRingNotification(content);
    }
}

void ModemCaller::onCallCommand()
{
    SimpleAtCommand *cmd = qobject_cast<SimpleAtCommand *>(sender());
    if (cmd) {
        QString number = cmd->customData().toString();
        AtResult res = cmd->getCommandResult();
        if (res.resultCode() == AtResult::OK) {
            // дозвонились
            logMsg(QString("dial on '%0' is ok").arg(number));
            m_timer.start(m_secondBeforeHangupOutgoingCall * 1000);
            emit outgoingCall(number, true);
            m_currentMode = OutgoingCall;
        }
        cmd->deleteLater();
    }
}

void ModemCaller::onStartCallCommand()
{
    SimpleAtCommand *cmd = qobject_cast<SimpleAtCommand *>(sender());
    if (cmd) {
        QString number = cmd->customData().toString();
        m_currentOutgingNumber = number;
        m_currentMode = OutgoingCall;
        logMsg(QString("start dialing on '%0'").arg(number));
    }
}

void ModemCaller::onTimer()
{
    if (!m_currentIncomingNumber.isEmpty()) {
        logMsg(QString("hang up incoming ring from '%0'").arg(m_currentIncomingNumber));
        hangup();
        m_lastHangupPhoneNum = m_currentIncomingNumber;
        m_lastHangupTime = QDateTime::currentMSecsSinceEpoch();
        m_currentIncomingNumber.clear();

    } else if (!m_currentOutgingNumber.isEmpty()) {
        logMsg(QString("hang up outgoing call to '%0'").arg(m_currentOutgingNumber));
        m_currentOutgingNumber.clear();
        hangup();
    }
    m_currentMode = NormalMode;
}

void ModemCaller::onHangupCommand()
{
    AtCommand *cmd = qobject_cast<AtCommand*>(sender());
    if (cmd) {
        cmd->deleteLater();
    }
}
int ModemCaller::secondBeforeHangupOutgoingCall() const
{
    return m_secondBeforeHangupOutgoingCall;
}

void ModemCaller::setSecondBeforeHangupOutgoingCall(int secondBeforeHangupOutgoingCall)
{
    m_secondBeforeHangupOutgoingCall = secondBeforeHangupOutgoingCall;
}

int ModemCaller::secondBeforeHangupIncomingCall() const
{
    return m_secondBeforeHangupIncomingCall;
}

void ModemCaller::setSecondBeforeHangupIncomingCall(int secondBeforeHangupIncomingCall)
{
    m_secondBeforeHangupIncomingCall = secondBeforeHangupIncomingCall;
}

void ModemCaller::logMsg(const QString &msg)
{
    qDebug(QString("[ModemCaller] %0").arg(msg).toUtf8().data());
}

void ModemCaller::handleRingNotification(const QString &content)
{
    QRegExp grabNumberRegExp("^\\\"(\\+?\\d+)\\\"");
    if (grabNumberRegExp.indexIn(content) >= 0) {
        QString phoneNum = grabNumberRegExp.cap(1);
        if (!phoneNum.isEmpty()) {
            if (m_currentIncomingNumber.isEmpty()) {
                if ((QDateTime::currentMSecsSinceEpoch() - m_lastHangupTime) < 1000
                        && m_lastHangupPhoneNum == phoneNum) {
                    return;
                }
                m_currentIncomingNumber = phoneNum;
                emit incomingRing(phoneNum);
                m_timer.start(secondBeforeHangupIncomingCall() * 1000);
                m_currentMode = IncomingCall;
                logMsg(QString("incoming call from %0").arg(phoneNum));
            }
        }
    }
}

void ModemCaller::handleNoCarrierNotification()
{
    stopTimer();
    switch (m_currentMode) {
    case IncomingCall: {
        if (!m_currentIncomingNumber.isEmpty()) {
            m_currentIncomingNumber.clear();
            m_currentMode = NormalMode;
        }
    } break;
    case OutgoingCall: {
        if (!m_currentOutgingNumber.isEmpty()) {
            emit outgoingCall(m_currentIncomingNumber, false);
            m_currentOutgingNumber.clear();
            m_currentMode = NormalMode;
        }
    } break;
    default:
        break;
    }
}

void ModemCaller::stopTimer()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    }
}
