
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

    bool bcmd = false;
    QString port; // serial port name
    qint32 baudRate; // serial baud rate

    bool bwrt = false;
    QString wrt;

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

    // option for different commands
    QCommandLineOption optCmd( QStringList( {"c", "cmd"} ) );
    optCmd.setDescription( "Pass command, next commands are allowed:\n"
                               "<connect> - Test for connection with stm's bootloader.\n"
                               "<get>     - Gets the version and the allowed commands supported by the current version of the bootloader.\n");
    optCmd.setValueName("command");
    parser.addOption( optCmd );

    QCommandLineOption optWrite( QStringList( {"w", "write"} ) );
    optWrite.setDescription("Write operation. Default address 0x08000000.");
    optWrite.setValueName("address");
    optWrite.setDefaultValue("0x08000000");
    parser.addOption( optWrite );

    parser.process( a ); // Process the actual command line arguments given by the user

    // ================================================================================

    if ( parser.isSet(optShow) ) {
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

    if ( parser.isSet( optCmd ) ) {
        bcmd = true;
    }

    if ( parser.isSet( optWrite ) ) {
        bwrt = true;
        wrt = parser.value( optWrite );
    }

    if ( !bcmd && !bwrt ) {
        out << "You must add command line option." << endl;
        return -1;
    }

    Serial se( out, port, baudRate );
    out << "PortName: " << se.getPortName() << endl;
    out << "BaudRate: " << se.getBaudRate() << endl;

    se.Open();

    if ( bcmd == true ) {
        QString cmd = parser.value( optCmd );

        if ( cmd == "connect" ) // connect test
        {
            se.cmdConnect();
        } else if ( cmd == "get" ) // get cmd 0x00
        {
            se.cmdGet();
        } else
        {
            out << "unknown command: " << cmd << endl;
        }
    }

    if ( bwrt == true ) {
        se.cmdWrite( wrt );
    }

    se.Close();

    return 0; //a.exec();
}
