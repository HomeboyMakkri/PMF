#include "FrequencyPlotter.h"
#include "DataStorage.h"

FrequencyPlotter::FrequencyPlotter(QChartView *chartView, QObject *parent)
    : QObject(parent)
{
    chart = new QChart();
    series = new QLineSeries();
    chart->addSeries(series);

    axisX = new QValueAxis();
    axisX->setTitleText("Время (сек)");
    axisX->setRange(0, 10);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisY = new QValueAxis();
    axisY->setTitleText("Частота (кГц)");
    axisY->setRange(0.05, 500);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setTitle("Мониторинг PFM сигнала (0.05-500 кГц)");

    chartView->setChart(chart);
}

void FrequencyPlotter::addDataPoint(double x, double y) {
    DataStorage::instance().addFrequencyData(x, y);
    updatePlot();
}

void FrequencyPlotter::clear()
{
    series->clear();
}

void FrequencyPlotter::updatePlot() {
    series->replace(DataStorage::instance().getFrequencyData());
    if (!DataStorage::instance().getFrequencyData().isEmpty()) {
        auto points = DataStorage::instance().getFrequencyData();
        axisX->setRange(points.first().x(), points.last().x());
    }
}
