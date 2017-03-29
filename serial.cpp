#include "serial.h"
#include <QSerialPortInfo>
#include <QList>

Serial::Serial()
{

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
            << "Vendor Identifier: " << (spi.hasVendorIdentifier() ? QByteArray::number(spi.vendorIdentifier(), 16) : blankString) << endl
            << "Product Identifier: " << (spi.hasProductIdentifier() ? QByteArray::number(spi.productIdentifier(), 16) : blankString) << endl
            << "Busy: " << (spi.isBusy() ? "Yes" : "No") << endl;
    }

}
