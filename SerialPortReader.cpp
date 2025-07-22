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

void SerialPortReader::handleReadyRead() {
    buffer += serialPort->readAll();

    int pos;
    while ((pos = buffer.indexOf('\n')) != -1) {
        QString line = buffer.left(pos).trimmed();
        buffer = buffer.mid(pos + 1);

        // Обработка строки вида "sensorX:значение"
        QStringList parts = line.split(':');
        if (parts.size() == 2) {
            QString sensorStr = parts[0].trimmed();
            QString valueStr = parts[1].trimmed();

            if (sensorStr.startsWith("sensor") && sensorStr.length() > 6) {
                bool ok;
                int sensorId = sensorStr.mid(6).toInt(&ok);

                if (ok && sensorId >= 1 && sensorId <= 8) {
                    double frequency = valueStr.toDouble(&ok);

                    if (ok && frequency >= 50 && frequency <= 500000) {
                        frequency /= 1000; // Конвертируем в кГц
                        qDebug() << "Converted to kHz:" << frequency << "kHz";
                        if (frequency > 0) { // Дополнительная проверка
                            emit newDataReceived(sensorId, frequency);
                        }
                    }
                }
            }
        }
    }
}
