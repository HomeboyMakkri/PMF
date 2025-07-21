#include "OscillocopePlotter.h"
#include "DataStorage.h"
OscilloscopePlotter::OscilloscopePlotter(QChartView *chartView, QObject *parent)
    : QObject(parent)
{
    chart = new QChart();
    voltageSeries = new QLineSeries();

    // Настройка графика
    chart->addSeries(voltageSeries);
    chart->setTitle("Осциллографный вид (PFM)");

    // Оси
    axisX = new QValueAxis();
    axisX->setTitleText("Время, мс");
    axisX->setRange(0, 1);
    chart->addAxis(axisX, Qt::AlignBottom);
    voltageSeries->attachAxis(axisX);

    axisY = new QValueAxis();
    axisY->setTitleText("Напряжение, В");
    axisY->setRange(0, 3.5);
    chart->addAxis(axisY, Qt::AlignLeft);
    voltageSeries->attachAxis(axisY);

    chartView->setChart(chart);
}

void OscilloscopePlotter::addPulse(double time, double frequency) {
    DataStorage::instance().addPulseData(time, frequency);
    updatePlot();
}

void OscilloscopePlotter::clear()
{
    voltageSeries->clear();
}

void OscilloscopePlotter::updatePlot() {
    voltageSeries->clear();

    const auto& pulseData = DataStorage::instance().getFrequencyData();
    if (pulseData.isEmpty()) return;

    // Определяем видимый временной диапазон (последние 10 мс)
    double maxTime = pulseData.last().x();
    double minTime = qMax(0.0, maxTime - 10.0); // Показываем последние 10 мс

    // Строим только видимые импульсы
    for (const QPointF& point : pulseData) {
        double time = point.x();
        if (time < minTime) continue; // Пропускаем точки вне видимого диапазона

        double frequency = point.y();
        if (frequency > 0) {
            double period = 1.0 / (frequency * 1000);
            double pulseWidth = period * 0.8;

            voltageSeries->append(time, 0);
            voltageSeries->append(time, 3.3);
            voltageSeries->append(time + pulseWidth, 3.3);
            voltageSeries->append(time + pulseWidth, 0);
        }
    }

    // Автомасштабирование с небольшим запасом справа
    if (!pulseData.isEmpty()) {
        double lastTime = pulseData.last().x();
        axisX->setRange(minTime, lastTime); // +0.5 мс запас
    }
}
