#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QVBoxLayout"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_path = "/home/egor/build-adcmmodifier-Desktop-Debug/adcm.dat.mod";
    m_controller = new Controller(m_path, m_pre);

    m_pushButton = new QPushButton("Process", this);

    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_pushButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(topFiller);
    layout->addLayout(mainLayout, 1);
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);

    connect(m_pushButton, &QPushButton::clicked, m_controller, [this](){
        m_controller->operate("1");
    });
    connect(m_controller, &Controller::handleResults, this, [](const QString &result){});

}

MainWindow::~MainWindow()
{
    delete ui;
}

