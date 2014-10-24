#ifndef SERIALMODEMTRANSPORT_H
#define SERIALMODEMTRANSPORT_H
#include "imodemtransport.h"

#include <QObject>
#include <QSerialPort>

class SerialModemTransport : public IModemTransport
{
    Q_OBJECT
public:
    explicit SerialModemTransport(const QString &portName, QObject *parent = 0);

    // IModemTransport interface
public:
    virtual bool isOpen() const;
    virtual bool open();
    virtual void close();
    virtual void write(const QByteArray &rawData);
    virtual bool getChar(char *ch);
    virtual QByteArray read(const qint64 maxSize);
    virtual QByteArray readAll();
    virtual quint64 bytesAvailable() const;

private:
    QSerialPort *m_serialPort;
    const QString m_portName;
    int m_baudRate;
};

#endif // SERIALMODEMTRANSPORT_H
