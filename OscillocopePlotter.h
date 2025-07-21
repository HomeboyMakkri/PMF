#ifndef OSCILLOSCOPEPLOTTER_H
#define OSCILLOSCOPEPLOTTER_H

#include <QObject>
#include <QtCharts>

class OscilloscopePlotter : public QObject
{
    Q_OBJECT
public:
    explicit OscilloscopePlotter(QChartView *chartView, QObject *parent = nullptr);
    void addPulse(double time, double frequency);
    void clear();
    void updatePlot();

private:
    QChart *chart;
    QLineSeries *voltageSeries;
    QValueAxis *axisX;
    QValueAxis *axisY;
};

#endif // OSCILLOSCOPEPLOTTER_H
