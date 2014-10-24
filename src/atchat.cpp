#include "atchat.h"

#include <QTimer>
#include <QUuid>

AtChat::AtChat(IModemTransport *modemTransport, QObject *parent) :
    QObject(parent),
    m_modemTransport(modemTransport),
    m_currentCommand(0),
    m_isBusy(false)
{
    init();
}

void AtChat::addCommand(AtCommand * const command)
{
    //qDebug() << Q_FUNC_INFO << command->atCommand().trimmed();
    if (!m_currentCommand && m_listOfCommand.size() == 0) {
        m_currentCommand = command;
        QTimer::singleShot(10, this, SLOT(processCurrentCommand()));

    } else {
        m_listOfCommand.append(command);
    }
}

void AtChat::init()
{
    connect(m_modemTransport, SIGNAL(readyRead()),
            this, SLOT(onReadyRead()));
    connect(&m_timeoutTimer, SIGNAL(timeout()),
            this, SLOT(onTimeout()));
    m_timeoutTimer.setSingleShot(true);
}



void AtChat::read()
{
    char ch;
    bool lineIsProcessed = true;
    while (m_modemTransport->getChar(&ch)) {
        //qDebug() << Q_FUNC_INFO << QUuid::createUuid().toString();
        if (ch == 0x0d) {
            continue;
        }
        stopWaitDataTimer();
        lineIsProcessed = false;
        if (ch != 0x0a) {
            m_currentLine.append(QChar(ch));

            if (m_modemTransport->bytesAvailable() == 0 && m_currentLine == QString("> ")) {
                // this is the place to send pdu or something else
                if (m_currentCommand) {
                    lineIsProcessed = true;
                    if (!m_currentCommand->processLine(m_currentLine, this)) {
                        QTimer::singleShot(0, m_currentCommand, SLOT(setIsProcessed()));
                        m_currentCommand = 0;
                    }
                }
                m_currentLine.clear();
            }
        } else {
            if (m_currentCommand && m_currentLine.trimmed() == m_currentCommand->atCommand().trimmed()) {
                // handle echo
                m_currentLine.clear();
                continue;
            }
            if (m_currentLine.trimmed().isEmpty()) {
                continue;
            }
            lineIsProcessed = true;
            processLine(m_currentLine);
            if (m_currentCommand) {
                if (!m_currentCommand->processLine(m_currentLine, this)) {
                    m_currentLine.clear();
                    QTimer::singleShot(0, m_currentCommand, SLOT(setIsProcessed()));
                    m_currentCommand = 0;
                    nextCommand();
                }
            }
            m_currentLine.clear();
        }
    }
    if (!lineIsProcessed || !m_currentLine.isEmpty()) {
        int timeout = 200;
        if (m_currentCommand) {
            timeout = m_currentCommand->waitDataTimeout();
        }
        startWaitDataTimeout(timeout);
    }
}

bool AtChat::processLine(QString line)
{
    //qDebug() << Q_FUNC_INFO << line;
    if (!m_expectedPduNotification.isEmpty()) {
        emit pduNotification(m_expectedPduNotification, line);
        m_expectedPduNotification.clear();
        return true;
    }

    for (const QString &strNotification : m_notifications) {
        if (line.startsWith(strNotification)) {
            QString content = line.mid(strNotification.length()).trimmed();
            emit notification(strNotification, content);
            return true;
        }
    }

    for (const QString &strNotification : m_pduNotifications) {
        if (line.startsWith(strNotification)) {
            m_expectedPduNotification = strNotification;
            return true;
        }
    }
    return false;
}

void AtChat::writeRawData(const QByteArray &data)
{
    //qDebug() << Q_FUNC_INFO << QString::fromLocal8Bit(data).trimmed();
    m_modemTransport->write(data);
}

void AtChat::startWaitDataTimeout(const int ms)
{
    m_timeoutTimer.start(ms);
}

void AtChat::stopWaitDataTimer()
{
    if (m_timeoutTimer.isActive()) {
        m_timeoutTimer.stop();
    }
}

bool AtChat::isOpen() const
{
    return m_modemTransport->isOpen();
}

void AtChat::registerNotification(const QString &notification)
{
    if (!m_notifications.contains(notification)) {
        m_notifications.append(notification);
    }
}

void AtChat::registerPduNotification(const QString &notification)
{
    if (!m_pduNotifications.contains(notification)) {
        m_pduNotifications.append(notification);
    }
}

void AtChat::processCurrentCommand()
{
    if (m_currentCommand) {
        //logMsg(QString("Process command: %0").arg(m_currentCommand->atCommand()));
        m_currentCommand->informAboutStartSendingCommand();
        writeRawData(m_currentCommand->atCommand().toLatin1());
        m_currentCommand->afterSendCommand(this);
    }
}

void AtChat::logMsg(const QString &msg)
{
    qDebug(QString("AtChat: %0").arg(msg).toUtf8().data());
}

void AtChat::nextCommand()
{
    QTimer::singleShot(10, this, SLOT(nextCommandSlot()));
}

void AtChat::onReadyRead()
{
    read();
}

void AtChat::onTimeout()
{
    qDebug() << Q_FUNC_INFO;
    if (m_currentCommand) {
        m_currentCommand->setItFailOnTimeoutReason(this);
        m_currentCommand = 0;
    }
    logMsg("Timeout while waiting data");
    nextCommand();
}

void AtChat::nextCommandSlot()
{
    if (!m_currentCommand && m_listOfCommand.size()) {
        m_currentCommand = m_listOfCommand.takeFirst();
        processCurrentCommand();
    }
}
