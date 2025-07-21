#ifndef DATASTORAGE_H
#define DATASTORAGE_H


#include <QVector>
#include <QPointF>
#include <QElapsedTimer>

class DataStorage {
public:
    static DataStorage& instance() {
        static DataStorage instance;
        return instance;
    }

    void addFrequencyData(double time, double frequency) {
        frequencyData.append(QPointF(time, frequency));
        if (frequencyData.size() > MAX_POINTS) {
            frequencyData.removeFirst();
        }
    }

    void addPulseData(double time, double frequency) {
        while (frequencyData.size() >= MAX_POINTS) {
            frequencyData.removeFirst();
        }
        frequencyData.append(QPointF(time, frequency));
        if (frequencyData.size() > MAX_POINTS) {
            frequencyData.removeFirst();
        }
    }

    const QVector<QPointF>& getFrequencyData() const { return frequencyData; }

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

    QVector<QPointF> frequencyData;
    static const int MAX_POINTS = 100;
    QElapsedTimer timer;
    qint64 lastElapsed = 0; // сохраняем время после остановки
};

#endif // DATASTORAGE_H
