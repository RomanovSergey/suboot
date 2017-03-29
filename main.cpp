
#include <QCoreApplication>
#include <QTextStream>
#include <QCommandLineParser>
#include <iostream>
#include <stdio.h>


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

    QCommandLineParser parser;
    parser.setApplicationDescription("app for flash erase, write, read stm32 via uart boot loader");
    parser.addHelpOption();
    parser.addVersionOption();
    // An option with a value
    QCommandLineOption optConnect( {"c", "connect"},
                                 "wait for connect and ACK byte from the board." );
    parser.addOption( optConnect );
    QCommandLineOption optShow( {"s", "showports"},
                                 "show available ports for connections." );
    parser.addOption( optShow );

    parser.process( a ); // Process the actual command line arguments given by the user

    if ( parser.isSet(optShow) ) {
        bshowPorts = true;
        out << "You have chosen: show available ports." << endl;
    }

    if ( parser.isSet( optConnect ) ) {
        bconnect = true;
        out << "You have chosen: connect to board" << endl;
    }

    if ( !bconnect && !bshowPorts ) {
        out << "You must add command line option." << endl;
    }

    return 0; //a.exec();
}
