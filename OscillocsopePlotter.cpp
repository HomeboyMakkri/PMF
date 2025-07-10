#include "OscillocopePlotter.h"

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
    axisX->setRange(0, 10);
    chart->addAxis(axisX, Qt::AlignBottom);
    voltageSeries->attachAxis(axisX);

    axisY = new QValueAxis();
    axisY->setTitleText("Напряжение, В");
    axisY->setRange(0, 3.5);
    chart->addAxis(axisY, Qt::AlignLeft);
    voltageSeries->attachAxis(axisY);

    chartView->setChart(chart);
}

void OscilloscopePlotter::addPulse(double time, double frequency)
{
    double period = 1.0 / (frequency * 1000);  // Период в мс
    double pulseWidth = period * 0.8;          // 80% скважность

    voltageSeries->append(time, 0);
    voltageSeries->append(time, 3.3);
    voltageSeries->append(time + pulseWidth, 3.3);
    voltageSeries->append(time + pulseWidth, 0);

    // Автомасштабирование
    axisX->setRange(0, time + 2*period);
}

void OscilloscopePlotter::clear()
{
    voltageSeries->clear();
}
