#include "OscilloscopePlotter.h"
#include "DataStorage.h"
#include <QDebug>

OscilloscopePlotter::OscilloscopePlotter(QChartView *chartView, QObject *parent)
    : QObject(parent)
{
    chart = new QChart();
    axisX = new QValueAxis();
    axisX->setTitleText("Время, мс");
    axisX->setRange(0, 10);
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
        if (frequency > 0 && time >= 0) { // Проверка на корректные значения
            double period = 1.0 / (frequency * 1000);
            double pulseWidth = period * 0.8;

            QLineSeries* series = seriesMap[sensorId];
            series->append(time, 0);
            series->append(time, 3.3);
            series->append(time + pulseWidth, 3.3);
            series->append(time + pulseWidth, 0);
        }
    }
}

void OscilloscopePlotter::clear() {
    for (auto series : seriesMap) {
        series->clear();
    }
}

void OscilloscopePlotter::updatePlot() {
    // Автомасштабирование по времени
    double minTime = 0, maxTime = 10;
    for (auto sensorId : visibleSensors) {
        const auto& data = DataStorage::instance().getFrequencyData(sensorId);
        if (!data.isEmpty()) {
            maxTime = qMax(maxTime, data.last().x());
            minTime = qMax(0.0, maxTime - 10.0);
        }
    }
    axisX->setRange(minTime, maxTime);
}

void OscilloscopePlotter::setVisibleSensors(const QSet<int>& sensors) {
    visibleSensors = sensors;
    for (auto it = seriesMap.begin(); it != seriesMap.end(); ++it) {
        it.value()->setVisible(visibleSensors.contains(it.key()));
    }
    clear();
    updatePlot();
}
