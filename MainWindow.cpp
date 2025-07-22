#include "MainWindow.h"
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFormLayout>
#include "DataStorage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      frequencyPlotter(nullptr),
      oscilloscopePlotter(nullptr),
      chartView(nullptr),
      serialReader(new SerialPortReader(this)),
      isConnected(false)
{

    // Настраиваем соединения
    connect(serialReader, &SerialPortReader::newDataReceived, this, &MainWindow::onNewData);
    connect(serialReader, &SerialPortReader::errorOccurred, this, &MainWindow::onError);
    // Сначала настраиваем UI
    setupUI(); // chartView будет создан здесь
    // Затем настраиваем plotter, когда chartView уже существует и добавлен в layout
    setupFrequencyPlotter();
}

void MainWindow::setupUI()
{
    // Главный виджет и layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    // 1. Верхняя панель (управление + датчики)
    QHBoxLayout *topPanelLayout = new QHBoxLayout();
    topPanelLayout->setSpacing(15);

    // Группа настроек подключения
    QGroupBox *controlGroup = new QGroupBox("Настройки подключения", this);
    controlGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QGridLayout *controlLayout = new QGridLayout(controlGroup);
    controlLayout->setVerticalSpacing(10);
    controlLayout->setHorizontalSpacing(8);
    controlLayout->setContentsMargins(12, 25, 12, 12);

    // Элементы управления подключением
    portCombo = new QComboBox(controlGroup);
    portCombo->setFixedWidth(120);
    refreshPortList();

    baudSpin = new QSpinBox(controlGroup);
    baudSpin->setRange(9600, 115200);
    baudSpin->setValue(115200);
    baudSpin->setFixedWidth(120);

    connectButton = new QPushButton("Подключить", controlGroup);
    connectButton->setFixedWidth(120);
    connectButton->setStyleSheet("QPushButton { background-color: #0078d4; color: white; border-radius: 4px; padding: 5px; }"
                                "QPushButton:hover { background-color: #106ebe; }"
                                "QPushButton:pressed { background-color: #005a9e; }");
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked); // Добавленная строка


    QPushButton *refreshBtn = new QPushButton("⟳", controlGroup);
    refreshBtn->setFixedSize(28, 28);
    refreshBtn->setStyleSheet("QPushButton { font-size: 14px; border: 1px solid #ccc; border-radius: 4px; background: white; }"
                             "QPushButton:hover { background: #f0f0f0; }");
    refreshBtn->setToolTip("Обновить список портов");
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshPortList);

    graphTypeCombo = new QComboBox(controlGroup);
    graphTypeCombo->addItem("Частота/Время");
    graphTypeCombo->addItem("Осциллограф");
    graphTypeCombo->setFixedWidth(150);
    graphTypeCombo->setCurrentIndex(FrequencyGraph);
    connect(graphTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
           this, &MainWindow::onGraphTypeChanged);

    // Layout для кнопки и статуса
    QVBoxLayout *buttonStatusLayout = new QVBoxLayout();
    buttonStatusLayout->setSpacing(8);
    buttonStatusLayout->addWidget(connectButton, 0, Qt::AlignLeft);
    statusLabel = new QLabel("Статус: не подключено", controlGroup);
    statusLabel->setStyleSheet("QLabel { color: #666666; font-style: italic; font-size: 11px; }");
    statusLabel->setAlignment(Qt::AlignLeft);
    buttonStatusLayout->addWidget(statusLabel);
    buttonStatusLayout->addStretch();

    // Добавление элементов в controlLayout
    controlLayout->addWidget(refreshBtn, 0, 0, Qt::AlignLeft);
    controlLayout->addWidget(new QLabel("Порт:"), 0, 1, Qt::AlignRight);
    controlLayout->addWidget(portCombo, 0, 2, Qt::AlignLeft);
    controlLayout->addWidget(new QLabel("Скорость:"), 1, 1, Qt::AlignRight);
    controlLayout->addWidget(baudSpin, 1, 2, Qt::AlignLeft);
    controlLayout->addWidget(new QLabel("Тип графика:"), 2, 1, Qt::AlignRight);
    controlLayout->addWidget(graphTypeCombo, 2, 2, Qt::AlignLeft);
    controlLayout->addLayout(buttonStatusLayout, 0, 3, 3, 1, Qt::AlignLeft);

    // Группа датчиков
    QGroupBox* sensorsGroup = new QGroupBox("Датчики", this);
    sensorsGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QGridLayout* sensorsLayout = new QGridLayout(sensorsGroup);
    sensorsLayout->setVerticalSpacing(6);
    sensorsLayout->setHorizontalSpacing(12);
    sensorsLayout->setContentsMargins(12, 25, 12, 12);

    for (int i = 1; i <= 8; ++i) {
        QCheckBox* checkbox = new QCheckBox(QString("Датчик %1").arg(i), this);
        checkbox->setChecked(true);
        checkbox->setStyleSheet("QCheckBox { spacing: 5px; }"
                               "QCheckBox::indicator { width: 16px; height: 16px; }"
                               "QCheckBox::indicator:unchecked { border: 1px solid #999; border-radius: 3px; background: white; }"
                               "QCheckBox::indicator:checked { border: 1px solid #0078d4; border-radius: 3px; background: #0078d4; image: url(:/images/checkmark.png); }");
        connect(checkbox, &QCheckBox::stateChanged, this, &MainWindow::updateVisibleSensors);
        sensorCheckboxes.append(checkbox);
        sensorsLayout->addWidget(checkbox, (i-1)/2, (i-1)%2);
    }

    // Логотип
    QLabel *logoLabel = new QLabel(this);
    logoLabel->setStyleSheet("QLabel { background: #f8f8f8; border: 1px solid #ddd; border-radius: 6px; padding: 8px; }");
    QPixmap logoPixmap(":/images/itelma_logo.png");
    if(!logoPixmap.isNull()) {
        logoLabel->setPixmap(logoPixmap.scaledToHeight(50, Qt::SmoothTransformation));
        logoLabel->setAlignment(Qt::AlignCenter);
    } else {
        logoLabel->setText("Итэлма");
        logoLabel->setStyleSheet("QLabel { font: bold 18px; color: #0055A0; background: #f8f8f8; border: 1px solid #ddd; border-radius: 6px; padding: 15px; }");
        logoLabel->setAlignment(Qt::AlignCenter);
    }
    logoLabel->setFixedSize(120, 80);

    // Компоновка верхней панели
    topPanelLayout->addWidget(controlGroup, 4);
    topPanelLayout->addWidget(sensorsGroup, 3);
    topPanelLayout->addWidget(logoLabel, 1, Qt::AlignTop);
    topPanelLayout->setAlignment(Qt::AlignTop);

    // 2. Добавляем верхнюю панель и график в главный layout
    mainLayout->addLayout(topPanelLayout);

    chartView = new QChartView(this); // Теперь это инициализирует член класса
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("QChartView { border: 1px solid #ccc; border-radius: 4px; background: white; }");
    mainLayout->addWidget(chartView, 1); // Добавляем ТОТ ЖЕ chartView в layout

    setCentralWidget(centralWidget);
    resize(1200, 700);
}

void MainWindow::setupFrequencyPlotter()
{
    if (oscilloscopePlotter) {
        oscilloscopePlotter->clear();
        delete oscilloscopePlotter;
        oscilloscopePlotter = nullptr;
    }

    if (!frequencyPlotter) {
        frequencyPlotter = new FrequencyPlotter(chartView, this);
        frequencyPlotter->updatePlot();
    }
}

void MainWindow::setupOscilloscopePlotter()
{
    if (frequencyPlotter) {
        frequencyPlotter->clear();
        delete frequencyPlotter;
        frequencyPlotter = nullptr;
    }

    if (!oscilloscopePlotter) {
        oscilloscopePlotter = new OscilloscopePlotter(chartView, this);
        oscilloscopePlotter->updatePlot();
    }
}

void MainWindow::onGraphTypeChanged(int index) {
    switch(index) {
    case FrequencyGraph:
        setupFrequencyPlotter();
        break;
    case OscilloscopeGraph:
        setupOscilloscopePlotter();
        break;
    }
    updateVisibleSensors(); // Обновляем видимость датчиков после смены типа графика
}

void MainWindow::updateVisibleSensors() {
    QSet<int> visibleSensors;
    for (int i = 0; i < sensorCheckboxes.size(); ++i) {
        if (sensorCheckboxes[i]->isChecked()) {
            visibleSensors.insert(i + 1);
        }
    }

    if (frequencyPlotter) {
        frequencyPlotter->setVisibleSensors(visibleSensors);
    }
    if (oscilloscopePlotter) {
        oscilloscopePlotter->setVisibleSensors(visibleSensors);
    }
}

void MainWindow::onNewData(int sensorId, double frequency) {
    double currentTime = DataStorage::instance().getCurrentTimeSeconds();
    DataStorage::instance().addFrequencyData(sensorId, currentTime, frequency);

    if(graphTypeCombo->currentIndex() == FrequencyGraph && frequencyPlotter) {
        frequencyPlotter->addDataPoint(sensorId, currentTime, frequency);
    } else if (oscilloscopePlotter) {
        oscilloscopePlotter->addPulse(sensorId, currentTime, frequency);
        oscilloscopePlotter->updatePlot(); // Добавляем принудительное обновление графика
    }
}

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
            DataStorage::instance().clear(); // Очищаем хранилище при новом подключении
            DataStorage::instance().startTimer();
            qDebug() << "Successfully connected to" << currentPortName << "at" << baudRate << "baud";
            isConnected = true;
            updateConnectionStatus(true);

            // Очищаем графики при новом подключении
            if (frequencyPlotter) frequencyPlotter->clear();
            if (oscilloscopePlotter) oscilloscopePlotter->clear();

        } else {
            qDebug() << "Failed to connect to" << currentPortName;
        }
    } else {
        // Процесс отключения
        serialReader->disconnectPort();
        DataStorage::instance().clear();
        DataStorage::instance().stopTimer();
        qDebug() << "Disconnected from" << currentPortName;
        isConnected = false;
        updateConnectionStatus(false);

        // Очищаем графики при отключении
        if (frequencyPlotter) frequencyPlotter->clear();
        if (oscilloscopePlotter) oscilloscopePlotter->clear();
    }
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




