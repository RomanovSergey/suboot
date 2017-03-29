#ifndef SERIAL_H
#define SERIAL_H

#include <QString>
#include <QTextStream>

class Serial
{
public:
    Serial();

    static void showPorts( QTextStream &out );
};

#endif // SERIAL_H
