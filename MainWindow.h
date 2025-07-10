#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include "SerialPortReader.h"

QT_CHARTS_USE_NAMESPACE

    class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow(){};

private slots:
    void onConnectClicked();
    void onNewData(double frequency);
    void onError(const QString &error);
    void updateConnectionStatus(bool connected);
    //
    void refreshPortList();
    //
private:
    void setupUI();
    void setupChart();

    SerialPortReader *serialReader;
    QChart *chart;
    QChartView *chartView;
    QLineSeries *series;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QVector<QPointF> dataPoints;
    int timeCounter = 0;
    const int MAX_POINTS = 100;

    // Состояние подключения
    bool isConnected;
    QString currentPortName;

    // Указатели на UI-элементы
    QLabel *statusLabel;
    QPushButton *connectButton;
    QComboBox *portCombo;
    QSpinBox *baudSpin;

};
#endif // MAINWINDOW_H
