#include "FrequencyPlotter.h"
#include "DataStorage.h"

FrequencyPlotter::FrequencyPlotter(QChartView *chartView, QObject *parent)
    : QObject(parent)
{
    chart = new QChart();
    axisX = new QValueAxis();
    axisX->setTitleText("Время (сек)");
    axisX->setRange(0, 10);
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY = new QValueAxis();
    axisY->setTitleText("Частота (кГц)");
    axisY->setRange(0.05, 500);
    chart->addAxis(axisY, Qt::AlignLeft);

    chart->setTitle("Мониторинг PFM сигнала (0.05-500 кГц)");

    // Инициализация серий для всех 8 датчиков
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

void FrequencyPlotter::addDataPoint(int sensorId, double x, double y) {
    if (seriesMap.contains(sensorId)) {
        DataStorage::instance().addFrequencyData(sensorId, x, y);
        if (visibleSensors.contains(sensorId)) {
            updatePlot();
        }
    }
}

void FrequencyPlotter::clear() {
    for (auto series : seriesMap) {
        series->clear();
    }
}

void FrequencyPlotter::updatePlot() {
    for (auto sensorId : visibleSensors) {
        if (seriesMap.contains(sensorId)) {
            seriesMap[sensorId]->replace(DataStorage::instance().getFrequencyData(sensorId));
        }
    }

    // Обновление диапазона оси X
    double minX = 0, maxX = 10;
    for (auto sensorId : visibleSensors) {
        const auto& data = DataStorage::instance().getFrequencyData(sensorId);
        if (!data.isEmpty()) {
            minX = qMin(minX, data.first().x());
            maxX = qMax(maxX, data.last().x());
        }
    }
    axisX->setRange(minX, maxX);
}

void FrequencyPlotter::setVisibleSensors(const QSet<int>& sensors) {
    visibleSensors = sensors;
    for (auto it = seriesMap.begin(); it != seriesMap.end(); ++it) {
        it.value()->setVisible(visibleSensors.contains(it.key()));
    }
    updatePlot();
}
