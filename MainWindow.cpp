#include "MainWindow.h"
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFormLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      serialReader(new SerialPortReader(this)),
      isConnected(false)
{
    // Сначала создаём все виджеты
    chartView = new QChartView();  // Без parent пока что
    plotter = new FrequencyPlotter(chartView, this);

    // Настраиваем соединения
    connect(serialReader, &SerialPortReader::newDataReceived, this, &MainWindow::onNewData);
    connect(serialReader, &SerialPortReader::errorOccurred, this, &MainWindow::onError);

    // Затем настраиваем UI
    setupUI();
}

void MainWindow::setupUI()
{
    // Главный виджет и layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // 1. Панель управления с логотипом
    QHBoxLayout *headerLayout = new QHBoxLayout();

    // Группа настроек подключения
    QGroupBox *controlGroup = new QGroupBox("Настройки подключения", this);
    QGridLayout *controlLayout = new QGridLayout(controlGroup);
    controlLayout->setVerticalSpacing(8);

    // Элементы управления
    portCombo = new QComboBox(controlGroup);
    refreshPortList(); // Первоначальное заполнение списка портов

    baudSpin = new QSpinBox(controlGroup);
    baudSpin->setRange(9600, 115200);
    baudSpin->setValue(115200);
    baudSpin->setFixedWidth(120);

    connectButton = new QPushButton("Подключить", controlGroup);
    connectButton->setFixedWidth(120);
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);

    // Кнопка обновления портов (теперь в одной строке с "Порт:")
    QPushButton *refreshBtn = new QPushButton("⟳", controlGroup);
    refreshBtn->setFixedSize(25, 25); // Квадратная кнопка
    refreshBtn->setToolTip("Обновить список портов");
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshPortList);

    // Индикатор состояния
    statusLabel = new QLabel("Статус: не подключено", controlGroup);
    statusLabel->setStyleSheet("QLabel { color: #666; font-style: italic; }");
    statusLabel->setAlignment(Qt::AlignCenter);

    // Добавление элементов в сетку (обновлённая версия)
    controlLayout->addWidget(refreshBtn, 0, 0); // Кнопка обновления
    controlLayout->addWidget(new QLabel("Порт:"), 0, 1, Qt::AlignRight);
    controlLayout->addWidget(portCombo, 0, 2);
    controlLayout->addWidget(new QLabel("Скорость:"), 1, 1, Qt::AlignRight);
    controlLayout->addWidget(baudSpin, 1, 2);
    controlLayout->addWidget(connectButton, 0, 3, 2, 1, Qt::AlignCenter);
    controlLayout->addWidget(statusLabel, 2, 0, 1, 4); // Растягиваем на все колонки

    // Логотип ИТЭЛМа
    QLabel *logoLabel = new QLabel(this);
    QPixmap logoPixmap("${CMAKE_CURRENT_SOURCE_DIR}/Resources/images/itelma_logo.png");
    if(!logoPixmap.isNull()) {
        logoLabel->setPixmap(logoPixmap.scaledToHeight(60, Qt::SmoothTransformation));
    } else {
        logoLabel->setText("Итэлма");
        logoLabel->setStyleSheet("QLabel { font: bold 20px; color: #0055A0; }");
    }
    logoLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Компоновка верхней панели
    headerLayout->addWidget(controlGroup, 1);
    headerLayout->addWidget(logoLabel, 0, Qt::AlignRight);
    headerLayout->setStretch(0, 1);

    // 2. График (теперь parent устанавливается здесь)
    chartView->setParent(centralWidget);  // Устанавливаем parent
    chartView->setRenderHint(QPainter::Antialiasing);

    // 3. Общая компоновка
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(chartView, 1);  // Добавляем в layout

    setCentralWidget(centralWidget);
    resize(1100, 650);
}

//void MainWindow::setupChart()
//{
//    chart = new QChart();
//    series = new QLineSeries();
//    chart->addSeries(series);

//    axisX = new QValueAxis();
//    axisX->setTitleText("Время (сек)");
//    axisX->setRange(0, 10);
//    chart->addAxis(axisX, Qt::AlignBottom);
//    series->attachAxis(axisX);

//    axisY = new QValueAxis();
//    axisY->setTitleText("Частота (кГц)");
//    axisY->setRange(500, 1000);
//    chart->addAxis(axisY, Qt::AlignLeft);
//    series->attachAxis(axisY);

//    chart->setTitle("Мониторинг PFM сигнала (500-1000 кГц)");
//    chartView->setChart(chart);
//}

void MainWindow::refreshPortList()

{
    portCombo->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        portCombo->addItem(info.portName());
    }
}

void MainWindow::onConnectClicked()
{
    if (!serialReader) {
        qCritical() << "SerialReader not initialized";
        return;
    }

    currentPortName = portCombo->currentText();
    const int baudRate = baudSpin->value();

    if (!isConnected) {
        // Попытка подключения
        if (serialReader->connectToPort(currentPortName, baudRate)) {
            qDebug() << "Successfully connected to" << currentPortName << "at" << baudRate << "baud";
            isConnected = true;
            updateConnectionStatus(true);
        } else {
            qDebug() << "Failed to connect to" << currentPortName;
        }
    } else {
        // Процесс отключения
        serialReader->disconnectPort();
        qDebug() << "Disconnected from" << currentPortName;
        isConnected = false;
        updateConnectionStatus(false);
    }
}


void MainWindow::onNewData(double frequency)
{
   plotter->addDataPoint(timeCounter++ / 10.0, frequency);
}

void MainWindow::onError(const QString &error)
{
    QMessageBox::critical(this, "Ошибка", error);
}

void MainWindow::updateConnectionStatus(bool connected)
{
      //Проверка на nullptr всех виджетов
     if (!statusLabel || !connectButton || !portCombo || !baudSpin) {
        qCritical() << "UI elements not initialized!";
        return;
    }

    isConnected = connected;

    if (connected) {
        statusLabel->setText(QString("Статус: Подключено к ") + QString(currentPortName));
        statusLabel->setStyleSheet("QLabel { color: #2A9D34; font-weight: bold; }");
        connectButton->setText("Отключить");
        portCombo->setEnabled(false);
        baudSpin->setEnabled(false);
    } else {
        statusLabel->setText("Статус: не подключено");
        statusLabel->setStyleSheet("QLabel { color: #666; font-style: italic; }");
        connectButton->setText("Подключить");
        portCombo->setEnabled(true);
        baudSpin->setEnabled(true);
    }
}


