
#include <QCoreApplication>
#include <QTextStream>
#include <QCommandLineParser>
#include <iostream>
#include <stdio.h>
#include "serial.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("stm32_uart_boot");
    QCoreApplication::setApplicationVersion("1.0");

    QTextStream out(stdout);

    bool bconnect = false;
    bool bshowPorts = false;
    QString port; // serial port name
    qint32 baudRate; // serial baud rate

    QCommandLineParser parser;
    parser.setApplicationDescription("App for stm32 flash erase, write, read via boot loader");
    parser.addHelpOption();
    parser.addVersionOption();

    // option for show available Serial Ports in system
    QCommandLineOption optShow( QStringList( {"s", "showports"} ) );
    optShow.setDescription( "Show available Serial Ports in system." );
    parser.addOption( optShow );

    // option for Serial Port Name
    QCommandLineOption optPort( QStringList( {"p", "port"} ) );
    optPort.setDescription("Set the serial <port_name> (short or long with location). "
                           "Default value: /dev/ttyUSB0.");
    optPort.setValueName("port_name");
    optPort.setDefaultValue("/dev/ttyUSB0");
    parser.addOption( optPort );

    // option for BaudRate control
    QCommandLineOption optBaudrate( QStringList( {"b", "baudrate"} ) );
    optBaudrate.setDescription("Set Baudrate value. Default 115200.");
    optBaudrate.setValueName("baudrate");
    optBaudrate.setDefaultValue("115200");
    parser.addOption( optBaudrate );

    // option for Connection check to the stm32 bootloader
    QCommandLineOption optConnect( QStringList( {"c", "connect"} ) );
    optConnect.setDescription( "Wait for connect and ACK byte from the board." );
    parser.addOption( optConnect );

    parser.process( a ); // Process the actual command line arguments given by the user
    // ================================================================================


    if ( parser.isSet(optShow) ) {
        bshowPorts = true;
        out << "You have chosen: show available ports." << endl;
        Serial::showPorts( out );
        return 0;
    }

    port = parser.value( optPort );
    if ( port.isEmpty() ) {
        out << "Please set the serial port name (-p port)." << endl;
        return -1;
    }

    bool ok;
    baudRate = parser.value( optBaudrate ).toInt( &ok );
    if ( ok == false ) {
        out << "Baud rate is false: " << baudRate << endl;
        return -1;
    }

    Serial se( port, baudRate );
    out << "PortName: " << se.getPortName() << endl;
    out << "BaudRate: " << se.getBaudRate() << endl;

    if ( parser.isSet( optConnect ) ) {
        bconnect = true;
        out << "You have chosen: connect to board" << endl;
        se.connect( out );
        return 0;
    }

    if ( !bconnect && !bshowPorts ) {
        out << "You must add command line option." << endl;
    }

    return 0; //a.exec();
}
