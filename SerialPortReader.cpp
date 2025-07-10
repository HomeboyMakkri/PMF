#include "SerialPortReader.h"
#include "QDebug"

SerialPortReader::SerialPortReader(QObject *parent) : QObject(parent)
{
    serialPort = new QSerialPort(this);
    connect(serialPort, &QSerialPort::readyRead, this, &SerialPortReader::handleReadyRead);
}

bool SerialPortReader::connectToPort(const QString &portName, int baudRate)
{
    serialPort->setPortName(portName);
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!serialPort->open(QIODevice::ReadOnly)) {
        emit errorOccurred(tr("Не удалось открыть порт %1: %2")
                               .arg(portName).arg(serialPort->errorString()));
        return false;
    }
    return true;
}

void SerialPortReader::disconnectPort()
{
    if (serialPort && serialPort->isOpen()) {
        serialPort->close();
        emit disconnected();  // Уведомляем о разрыве соединения
    }
}

void SerialPortReader::handleReadyRead()
{
    buffer += serialPort->readAll();
    qDebug() << "Raw input:" << buffer;

    int pos;
    while ((pos = buffer.indexOf('\n')) != -1) {
        QString line = buffer.left(pos).trimmed();
        buffer = buffer.mid(pos + 1);

        qDebug() << "Processing line:" << line;

        bool ok;
        double frequency = line.toDouble(&ok);

        // 5. Проверяем валидность данных
        if (!ok) {
            qDebug() << "Invalid number format:" << line;
            continue;
        }

        if (frequency >= 500000 && frequency <= 1000000) {
            frequency /= 1000; // Конвертируем в кГц
            qDebug() << "Valid frequency:" << frequency << "kHz";
            emit newDataReceived(frequency);
        } else {
            qDebug() << "Frequency out of range (500000-1000000 Hz):" << frequency;
        }
    }
}
