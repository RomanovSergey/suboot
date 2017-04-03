#ifndef SERIAL_H
#define SERIAL_H

#include <QString>
#include <QTextStream>
#include <QSerialPort>

class Serial : QSerialPort
{
    const char CON   = 0x7F; // connection - take a link with board
    const char ACK   = 0x79; // good answer from board
    const char NACK  = 0x1F; // bad  answer from board
    const char GET   = 0x00; // Get command

    quint8 m_bootLoaderVersion = 0;
    QByteArray m_cmds;

public:
    Serial( QTextStream &outstr, QString &portName, qint32 baudR );
    virtual ~Serial() {
    }

    static void showPorts( QTextStream &out );
    bool Open();
    void Close();

    QString getPortName() {
        return portName();
    }
    QString getBaudRate() {
        return QString::number( baudRate() );
    }

    bool sendcmd( char cmd, QByteArray &answer);

    bool cmdConnect();
    bool cmdGet();

private:
    QTextStream &out;
};

#endif // SERIAL_H
