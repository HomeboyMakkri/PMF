#ifndef DATASTORAGE_H
#define DATASTORAGE_H


#include <QVector>
#include <QPointF>
#include <QElapsedTimer>
#include <QMap>

class DataStorage {
public:
    static DataStorage& instance() {
        static DataStorage instance;
        return instance;
    }

    void addFrequencyData(int sensorId, double time, double frequency) {
        if (!frequencyData.contains(sensorId)) {
            frequencyData[sensorId] = QVector<QPointF>();
        }

        frequencyData[sensorId].append(QPointF(time, frequency));
        if (frequencyData[sensorId].size() > MAX_POINTS) {
            frequencyData[sensorId].removeFirst();
        }
    }

    const QVector<QPointF>& getFrequencyData(int sensorId) const {
        static QVector<QPointF> empty;
        auto it = frequencyData.find(sensorId);
        return (it != frequencyData.end()) ? *it : empty;
    }

    void addPulseData(int sensorId, double time, double frequency) {
        addFrequencyData(sensorId, time, frequency);
    }

    void clear() {
        frequencyData.clear();
        timer.restart();
        lastElapsed = 0;
    }

    void startTimer() {
        timer.start();
        lastElapsed = 0;
    }

    void stopTimer() {
        lastElapsed = timer.elapsed();
    }

    double getCurrentTimeSeconds() const {
        return (timer.isValid() ? timer.elapsed() : lastElapsed) / 1000.0; // в секундах
    }

    double getCurrentTimeMilliseconds() const {
        return (timer.isValid() ? timer.elapsed() : lastElapsed); // в миллисекундах
    }

private:
    DataStorage() = default;
    ~DataStorage() = default;

    QMap<int, QVector<QPointF>> frequencyData;
    static const int MAX_POINTS = 100;
    QElapsedTimer timer;
    qint64 lastElapsed = 0; // сохраняем время после остановки
};

#endif // DATASTORAGE_H
