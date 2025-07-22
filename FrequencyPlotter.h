#ifndef FREQUENCYPLOTTER_H
#define FREQUENCYPLOTTER_H


#include <QtCharts>
#include <QSet>

class FrequencyPlotter : public QObject {
    Q_OBJECT
public:
    explicit FrequencyPlotter(QChartView *chartView, QObject *parent = nullptr);
    void addDataPoint(int sensorId, double x, double y);
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

#endif // FREQUENCYPLOTTER_H
