#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include "SerialPortReader.h"
#include "FrequencyPlotter.h"
#include "OscillocopePlotter.h"

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
    void refreshPortList();
    void onGraphTypeChanged(int index);

private:

    enum GraphType { FrequencyGraph, OscilloscopeGraph };
    QComboBox *graphTypeCombo;  // Выпадающий список для выбора типа графика
    FrequencyPlotter *frequencyPlotter;
    OscilloscopePlotter *oscilloscopePlotter;
    QChartView *chartView;

    void setupFrequencyPlotter();
    void setupOscilloscopePlotter();

    void setupUI();
    SerialPortReader *serialReader;
    int timeCounter = 0;

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
