#include "serialmodemtransport.h"

SerialModemTransport::SerialModemTransport(const QString &portName, QObject *parent) :
    IModemTransport(parent),
    m_serialPort(nullptr),
    m_portName(portName),
    m_baudRate(-1)
{
}


bool SerialModemTransport::isOpen() const
{
    return (m_serialPort && m_serialPort->isOpen());
}

bool SerialModemTransport::open()
{
    if (this->isOpen()) {
        return true;
    }
    if (m_serialPort) {
        m_serialPort->disconnect();
        m_serialPort->deleteLater();
    }
    m_serialPort = new QSerialPort(m_portName);
    bool isOpen = false;
    while (true) {
        if (!m_serialPort->open(QIODevice::ReadWrite)) {
            break;
        }
        if (m_baudRate > 0) {
            if (!m_serialPort->setBaudRate(m_baudRate)) {
                break;
            }
        } else {
            if (!m_serialPort->setBaudRate(QSerialPort::Baud38400)) {
                break;
            }
        }
        if (!m_serialPort->setDataBits(QSerialPort::Data8)) {
            break;
        }
        if (!m_serialPort->setParity(QSerialPort::NoParity)) {
            break;
        }
        if (!m_serialPort->setFlowControl(QSerialPort::NoFlowControl)) {
            break;
        }
        isOpen = true;
    }
    if (!isOpen) {
        if (m_serialPort->isOpen()) {
            m_serialPort->close();
        }
        m_serialPort->deleteLater();
        m_serialPort = 0;
    } else {
        connect(m_serialPort, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
    }
    return isOpen;
}

void SerialModemTransport::close()
{
    if (isOpen()) {
        m_serialPort->close();
        m_serialPort->disconnect();
        m_serialPort->deleteLater();
        m_serialPort = nullptr;
    }
}

void SerialModemTransport::write(const QByteArray &rawData)
{
    if (m_serialPort) {
        m_serialPort->write(rawData);
    }
}

bool SerialModemTransport::getChar(char *ch)
{
    if (m_serialPort) {
        return m_serialPort->getChar(ch);
    } else {
        return false;
    }
}

QByteArray SerialModemTransport::read(const qint64 maxSize)
{
    if (m_serialPort) {
        return m_serialPort->read(maxSize);
    } else {
        return QByteArray();
    }

}

QByteArray SerialModemTransport::readAll()
{
    if (m_serialPort) {
        return m_serialPort->readAll();
    } else {
        return QByteArray();
    }
}


quint64 SerialModemTransport::bytesAvailable() const
{
    if (m_serialPort) {
        return m_serialPort->bytesAvailable();
    } else {
        return -1;
    }
}
