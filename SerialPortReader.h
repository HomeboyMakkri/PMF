#ifndef SERIALPORTREADER_H
#define SERIALPORTREADER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QVector>
#include <QPointF>

class SerialPortReader : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortReader(QObject *parent = nullptr);
    ~SerialPortReader()
    {
        disconnectPort();
    };

    bool connectToPort(const QString &portName, int baudRate = 115200);
    void disconnectPort();

signals:
    void newDataReceived(double frequency);
    void errorOccurred(const QString &error);
    void disconnected();

private slots:
    void handleReadyRead();

private:
    QSerialPort *serialPort;
    QString buffer;
};

#endif // SERIALPORTREADER_H
