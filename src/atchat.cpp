#include "atchat.h"

#include <QTimer>

AtChat::AtChat(QIODevice *readDevice, QIODevice *writeDevice, QObject *parent) :
    QObject(parent),
    m_readDevice(readDevice),
    m_writeDevice(writeDevice ? writeDevice : readDevice),
    m_currentCommand(0),
    m_isBusy(false)
{
    init();
}

void AtChat::addCommand(AtCommand * const command)
{
    if (!m_currentCommand) {
        m_currentCommand = command;
        processCurrentCommand();

    } else {
        m_listOfCommand.append(command);
    }
}

void AtChat::init()
{
    connect(m_readDevice, SIGNAL(readyRead()),
            this, SLOT(onReadyRead()));
    connect(&m_timeoutTimer, SIGNAL(timeout()),
            this, SLOT(onTimeout()));
    m_timeoutTimer.setSingleShot(true);
}

void AtChat::read()
{
    qDebug() << Q_FUNC_INFO;
    char ch;
    bool lineIsProcessed = true;
    while (m_readDevice->getChar(&ch)) {
        if (ch == 0x0d) {
            continue;
        }
        m_timeoutTimer.stop();
        lineIsProcessed = false;
        if (ch != 0x0a) {
            m_currentLine.append(QChar(ch));

            if (m_readDevice->bytesAvailable() == 0 && m_currentLine == QString("> ")) {
                // this is the place to send pdu or something else
                if (m_currentCommand){
                    if (!m_currentCommand->processLine(m_currentLine, this)) {
                        m_currentLine.clear();
                        lineIsProcessed = true;
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
            processLine(m_currentLine);
            if (m_currentCommand) {
                if (!m_currentCommand->processLine(m_currentLine, this)) {
                    lineIsProcessed = true;
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
        m_timeoutTimer.start(200);
    }
}

void AtChat::processLine(QString line)
{
    qDebug() << Q_FUNC_INFO << line;
    for (const QString &strNotification : m_notifications) {
        if (line.startsWith(strNotification)) {
            QString content = line.mid(strNotification.length()).trimmed();
            qDebug() << Q_FUNC_INFO;
            emit notification(strNotification, content);
        }
    }
}

void AtChat::writeRawData(const QByteArray &data)
{
    m_writeDevice->write(data);
}

void AtChat::setWaitDataTimeout(const int ms)
{
    m_timeoutTimer.start(ms);
}

bool AtChat::isOpen() const
{
    return m_readDevice && m_readDevice->isOpen();
}

void AtChat::registerNotification(const QString &notification)
{
    if (!m_notifications.contains(notification)) {
        m_notifications.append(notification);
    }
}

void AtChat::processCurrentCommand()
{
    if (m_currentCommand) {
        writeRawData(m_currentCommand->atCommand().toAscii());
        m_currentCommand->afterSendCommand(this);
    }
}

void AtChat::logMsg(const QString &msg)
{
    qDebug(QString("AtChat: %0").arg(msg).toUtf8().data());
}

void AtChat::onReadyRead()
{
    read();
}

void AtChat::onTimeout()
{
    if (m_currentCommand) {
        m_currentCommand->setItFailOnTimeoutReason();
        m_currentCommand = 0;
    }
    logMsg("Timeout while waiting data");
    nextCommand();
}

void AtChat::nextCommand()
{
    if (!m_currentCommand && m_listOfCommand.size()) {
        m_currentCommand = m_listOfCommand.takeFirst();
        processCurrentCommand();
    }
}
