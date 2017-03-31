#ifndef SERIAL_H
#define SERIAL_H

#include <QString>
#include <QTextStream>
#include <QSerialPort>

class Serial : QSerialPort
{
    const char CON   = 0x7F;
    const char ACK   = 0x79;

public:
    Serial( QString &portName, qint32 baudR );
    virtual ~Serial() {
    }

    static void showPorts( QTextStream &out );

    bool connect( QTextStream &out );

    QString getPortName() {
        return portName();
    }
    QString getBaudRate() {
        return QString::number( baudRate() );
    }

private:
    QString port;
};

#endif // SERIAL_H
