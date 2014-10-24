#ifndef IMODEMTRANSPORT_H
#define IMODEMTRANSPORT_H

#include <QObject>

class IModemTransport : public QObject
{
    Q_OBJECT
public:
    IModemTransport(QObject *parent = 0) : QObject(parent) {}

    virtual bool isOpen() const = 0;
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual void write(const QByteArray &rawData) = 0;
    virtual bool getChar(char *ch) = 0;
    virtual QByteArray read(const qint64 maxSize) = 0;
    virtual QByteArray readAll() = 0;
    virtual quint64 bytesAvailable() const = 0;

signals:
    void readyRead();
};

#endif // IMODEMTRANSPORT_H
