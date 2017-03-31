#include "serial.h"
#include <QSerialPortInfo>
#include <QList>

Serial::Serial( QString &portName, qint32 baudR )
{
    setPortName( portName );
    setBaudRate( baudR );
    setDataBits( QSerialPort::Data8 );
    setFlowControl( QSerialPort::NoFlowControl );
    setParity( QSerialPort::EvenParity );
    setStopBits( QSerialPort::OneStop );
}

void Serial::showPorts( QTextStream &out )
{
    const QString blankString = QObject::tr("N/A");
    QString description;
    QString manufacturer;
    QString serialNumber;

    QList < QSerialPortInfo > lp = QSerialPortInfo::availablePorts();
    out << "Total number of ports available: " << lp.count() << endl;

    qSort(  lp.begin(), lp.end(),
            [](const QSerialPortInfo &a, const QSerialPortInfo &b) -> bool
            { return a.portName() < b.portName(); }  );

    foreach (const QSerialPortInfo &spi, lp) {
        description = spi.description();
        manufacturer = spi.manufacturer();
        serialNumber = spi.serialNumber();
        out << endl
            << "Port: " << spi.portName() << endl
            << "Location: " << spi.systemLocation() << endl
            << "Description: " << (!description.isEmpty() ? description : blankString) << endl
            << "Manufacturer: " << (!manufacturer.isEmpty() ? manufacturer : blankString) << endl
            << "Serial number: " << (!serialNumber.isEmpty() ? serialNumber : blankString) << endl
            << "Busy: " << (spi.isBusy() ? "Yes" : "No") << endl;
    }

}

/*
 * Wait for connect and ACK byte from the board.
 */
bool Serial::connect( QTextStream &out )
{
    bool ret = false;
    char buf[5] = {0};
    char ch;
    int count = 0;

    out << "Connection ..." << endl;
    if ( !open( QIODevice::ReadWrite ) ) {
        out << "Error: can't open port" << endl;
        close();
        return false;
    }

    forever {
        writeData( &CON, 1 );

        if ( !waitForBytesWritten( 200 ) ) {
            out << "Error: can't write to port on connect" << endl;
            break;
        }

        if ( !waitForReadyRead( 500 ) ) {
            out << "Error: no data read on connect" << endl;
            break;
        }

        ch = read( buf, 1 );
        if ( buf[0] == ACK ) {
            out << "Get ACK byte on connect" << endl;
            ret = true;
            break;
        } else {
            out << "Left byte: " << hex << ch << dec << " recived" << endl;
        }
        count++;
        if ( count == 5 ) {
            out << "Error: no ACK byte" << endl;
            break;
        }
        out << "Next iteration: " << count << endl;
    }


    close();
    return ret;
}

