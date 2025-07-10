#include "FrequencyPlotter.h"

FrequencyPlotter::FrequencyPlotter(QChartView *chartView, QObject *parent)
    : QObject(parent)
{
    chart = new QChart();
    series = new QLineSeries();
    series = new QLineSeries();
    chart->addSeries(series);

    axisX = new QValueAxis();
    axisX->setTitleText("Время (сек)");
    axisX->setRange(0, 10);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisY = new QValueAxis();
    axisY->setTitleText("Частота (кГц)");
    axisY->setRange(500, 1000);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setTitle("Мониторинг PFM сигнала (500-1000 кГц)");

    chartView->setChart(chart);
}

void FrequencyPlotter::addDataPoint(double x, double y)
{
    dataPoints.append(QPointF(x, y));
    if (dataPoints.size() > MAX_POINTS) {
        dataPoints.removeFirst();
    }
    series->replace(dataPoints);
    axisX->setRange(dataPoints.first().x(), dataPoints.last().x());
}

void FrequencyPlotter::clear()
{
    series->clear();
}
