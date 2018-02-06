#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ip_addr->setInputMask("000.000.000.000");
    ui->ip_addr->setPlaceholderText("000.000.000.000");
    startShortcut = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_R), this);
    stopShortcut = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_S), this);
    connect(startShortcut, SIGNAL(activated()),this, SLOT(on_start_cap_button_clicked()));
    connect(stopShortcut, SIGNAL(activated()), this, SLOT(on_stop_cap_button_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_start_cap_button_clicked()
{
    //uruchomienie wątka pingThread i przekazanie jako parametru adresu IP wpisanego w interfejsie
    pingThread = new PingThread(ui->ip_addr->text());
    pingThread->start();

}

void MainWindow::on_stop_cap_button_clicked()
{
    //zakończenie wątka pingującego urządzenie
    pingThread->quit();
    pingThread->wait();

    //zniszczenie obiektu PingThread
    delete pingThread;
}
