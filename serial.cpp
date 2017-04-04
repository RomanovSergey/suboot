#include "serial.h"
#include <QSerialPortInfo>
#include <QList>

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

Serial::Serial( QTextStream &outstr, QString &portName, qint32 baudR ) : out(outstr)
{
    setPortName( portName );
    setBaudRate( baudR );
    setDataBits( QSerialPort::Data8 );
    setFlowControl( QSerialPort::NoFlowControl );
    setParity( QSerialPort::EvenParity );
    setStopBits( QSerialPort::TwoStop );
}

bool Serial::Open()
{
    if ( !open( QIODevice::ReadWrite ) ) {
        out << "Error: can't open port" << endl;
        close();
        return false;
    }
    return true;
}

void Serial::Close()
{
    close();
}

bool Serial::sendcmd( char cmd, QByteArray &answer)
{
    qint64 num;
    QByteArray wba;

    wba[0] = cmd;
    wba[1] = 0xFF; // ? not documented, but without it do not work
    wba[2] = ~wba[0];

    num = write( wba );
    if ( num != wba.size() ) {
        out << "Error: can't write to port, write ret=" << (int)num << endl;
        return false;
    } else {
        out << "send: " << wba.size() << " bytes" << endl;
    }
    if ( !waitForReadyRead( 500 ) ) {
        out << "Error: no data read on cmd Get" << endl;
        return false;
    }
    answer = read( 50 );
    return true;
}

/*
 * Wait for connect and ACK byte from the board.
 */
bool Serial::cmdConnect()
{
    bool ret = false;
    QByteArray answer;

    if ( sendcmd( CON, answer ) ) {
        out << "We read " << answer.size() << " bytes" << endl;
        if ( answer[0] == ACK ) {
            out << "Get ACK byte on cmd Connect" << endl;
            ret = true;
            out << showbase;
        } else {
            out << "Left byte: " << hex << (quint8)answer[0] << dec << endl;
        }
    } else {
        out << "Failed" << endl;
    }
    return ret;
}

bool Serial::cmdGet()
{
    bool ret = false;
    QByteArray answer;

    if ( !cmdConnect() ) {
        out << "Error: can't to connect to board" << endl;
        return false;
    }

    if ( sendcmd( GET, answer ) ) {
        out << "We read " << answer.size() << " bytes" << endl;
        if ( answer[0] == ACK ) {
            out << "Get ACK byte on cmd Get" << endl;
            ret = true;
            out << showbase;

            if ( answer.size() < 3 ) {
                out << "Get cmd, too short answer: " << answer.size() << endl;
                return false;
            }
            quint8 len = answer[1];
            m_cmds.clear();
            m_cmds.resize( len );
            m_bootLoaderVersion = answer[2];
            out << "Boot Loader ver: " << hex << uppercasedigits << m_bootLoaderVersion << endl;
            for ( int i = 0; i < len; i++ ) {
                out << "cmd(" << dec << i << ")=" << hex << (quint8)(answer[i+3]) << endl;
                m_cmds.append( answer[i+3] );
            }

        } else {
            out << "Left byte: " << hex << (quint8)answer[0] << dec << " recived" << endl;
        }
    } else {
        out << "Failed" << endl;
    }
    return ret;
}

void fillArrayFromQuint32( quint32 val, QByteArray &ba )
{
    ba.clear();
    ba.append( (char)(val >> 24) );
    ba.append( 0xFF );
    ba.append( (char)(val >> 16) );
    ba.append( 0xFF );
    ba.append( (char)(val >> 8) );
    ba.append( 0xFF );
    ba.append( (char)(val >> 0) );
    ba.append( 0xFF );
}

char calcXOR( QByteArray &ba )
{
    char vxor = 0;
    for ( int i = 0; i < ba.size(); i++ ) {
        vxor ^= ba[i];
    }
    return 0x08;
}

bool Serial::cmdWrite( quint32 adr )
{
    bool ret = false;
    QByteArray answer;
    QByteArray wba;
    qint64 num;

    out << "cmd write addr=" << showbase << hex << uppercasedigits << adr << endl;
    out << dec;

    if ( !cmdConnect() ) {
        out << "Error: can't to connect to board" << endl;
        return false;
    }

    if ( sendcmd( WRT, answer ) ) {
        out << "We read " << answer.size() << " bytes" << endl;
        if ( answer[0] == ACK ) {
            out << "Get ACK byte on cmd Write memory start" << endl;
            ret = true;
        } else {
            out << "Left byte: " << hex << (quint8)answer[0] << dec << " recived" << endl;
        }
    } else {
        out << "Failed" << endl;
    }

    // отправим адрес и xor сумму
    fillArrayFromQuint32( adr, wba );
    wba.append( calcXOR( wba ) );
    for ( int i = 0; i < wba.size(); i++ ) {
        out << "wba[" << dec << i << "]=" << showbase << hex << (quint8)wba[i] << endl;
    }
    num = write( wba );
    if ( num != wba.size() ) {
        out << "Error: can't write to port, write ret=" << (int)num << endl;
        return false;
    } else {
        out << "send: " << wba.size() << " bytes" << endl;
    }
    if ( !waitForReadyRead( 500 ) ) {
        out << "Error: no data read on cmd Get" << endl;
        return false;
    }
    answer = read( 50 );
    if ( answer[0] == ACK ) {
        out << "get second ACK" << endl;
    } else {
        out << "Left second: " << hex << (quint8)answer[0] << dec << endl;
    }

    return ret;
}
