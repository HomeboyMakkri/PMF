#ifndef OSCILLOSCOPEPLOTTER_H
#define OSCILLOSCOPEPLOTTER_H

#include <QObject>
#include <QtCharts>
#include <QSet>

class OscilloscopePlotter : public QObject
{
    Q_OBJECT
public:
    explicit OscilloscopePlotter(QChartView *chartView, QObject *parent = nullptr);
    void addPulse(int sensorId, double time, double frequency);
    void clear();
    void updatePlot();
    void setVisibleSensors(const QSet<int>& sensors);

private:
    QChart *chart;
    QMap<int, QLineSeries*> seriesMap;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QSet<int> visibleSensors;
};

#endif // OSCILLOSCOPEPLOTTER_H

