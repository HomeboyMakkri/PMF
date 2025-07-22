#include "OscilloscopePlotter.h"
#include "DataStorage.h"
#include <QDebug>

OscilloscopePlotter::OscilloscopePlotter(QChartView *chartView, QObject *parent)
    : QObject(parent)
{
    chart = new QChart();
    axisX = new QValueAxis();
    axisX->setTitleText("Время, мс");
    axisX->setRange(0, 5);
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY = new QValueAxis();
    axisY->setTitleText("Напряжение, В");
    axisY->setRange(0, 3.5);
    chart->addAxis(axisY, Qt::AlignLeft);

    chart->setTitle("Осциллографный вид (PFM)");

    // Инициализация серий для всех датчиков
    for (int i = 1; i <= 8; ++i) {
        QLineSeries* series = new QLineSeries();
        series->setName(QString("Датчик %1").arg(i));
        seriesMap[i] = series;
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    chartView->setChart(chart);
}

void OscilloscopePlotter::addPulse(int sensorId, double time, double frequency) {
    if (seriesMap.contains(sensorId) && visibleSensors.contains(sensorId)) {
        if (frequency > 0 && time >= 0) {
            // frequency в кГц, преобразуем в Гц для расчета периода
            double period_ms = 1.0 / (frequency * 1000.0) * 1000.0; // период в мс
            double pulseWidth = period_ms * 0.8; // ширина импульса 80% периода

            QLineSeries* series = seriesMap[sensorId];

            // Добавляем точки импульса
            series->append(time, 0);
            series->append(time, 3.3);
            series->append(time + pulseWidth, 3.3);
            series->append(time + pulseWidth, 0);

            // Ограничение количества точек (например, последние 1000 точек)
            const int MAX_POINTS = 1000;
            if (series->count() > MAX_POINTS) {
                series->removePoints(0, series->count() - MAX_POINTS);
            }
        }
    }
}

void OscilloscopePlotter::clear() {
    for (auto series : seriesMap) {
        series->clear();
    }
}

void OscilloscopePlotter::updatePlot() {
    double minTime = 0, maxTime = 5; // Было 10, стало 5
    bool hasData = false;

    for (auto sensorId : visibleSensors) {
        const auto& data = DataStorage::instance().getFrequencyData(sensorId);
        if (!data.isEmpty()) {
            hasData = true;
            maxTime = qMax(maxTime, data.last().x());
            minTime = qMax(0.0, maxTime - 5.0); // Было 10.0, стало 5.0
        }
    }

    if (hasData) {
        axisX->setRange(minTime, maxTime);
    } else {
        axisX->setRange(0, 5); // Было 10, стало 5
    }
}

void OscilloscopePlotter::setVisibleSensors(const QSet<int>& sensors) {
    visibleSensors = sensors;
    for (auto it = seriesMap.begin(); it != seriesMap.end(); ++it) {
        it.value()->setVisible(visibleSensors.contains(it.key()));
    }
    clear();
    updatePlot();
}
