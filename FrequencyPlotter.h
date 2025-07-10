#ifndef FREQUENCYPLOTTER_H
#define FREQUENCYPLOTTER_H


#include <QtCharts>

class FrequencyPlotter : public QObject {
    Q_OBJECT
public:
    explicit FrequencyPlotter(QChartView *chartView, QObject *parent = nullptr);
    void addDataPoint(double x, double y);
    void clear();

private:
    QChart *chart;
    QLineSeries *series;
    QValueAxis *axisX;
    QValueAxis *axisY;
    const int MAX_POINTS = 100;
    QList<QPointF> dataPoints;
};

#endif // FREQUENCYPLOTTER_H
