#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ip_addr->setInputMask("000.000.000.000");       //maska pola do wpisywania adresu IP
    startShortcut = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_R), this);  //ustawienie skrótu na rozpoczęcie skanowania
    stopShortcut = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_S), this);   //ustawienie skrótu na zakończenie skanowania
    connect(startShortcut, SIGNAL(activated()),this, SLOT(on_start_cap_button_clicked()));  //połączenie skrtótu klawiszowego z przyciskiem "Zacznij przechwytywanie"
    connect(stopShortcut, SIGNAL(activated()), this, SLOT(on_stop_cap_button_clicked()));   //Połączenie skrótu klawiszowego z przyciskiem "Zatrzyma przechwytywanie"
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_start_cap_button_clicked
 * Funkcja jaka się wykona po naciśnięciu klawiszan "Zatrzymaj przechwytywanie"
 */
void MainWindow::on_start_cap_button_clicked()
{
    //inicjalizacja wątków
    pingThread = new PingThread(ui->ip_addr->text());
    videoThread = new VideoThread(ui->ip_addr->text());
    opencvThread = new OpencvThread(ui->ip_addr->text());
    //uruchomienie wątków
    pingThread->start();
    videoThread->start();
    opencvThread->start();
}
/**
 * @brief MainWindow::on_stop_cap_button_clicked
 * Funkcja jaka się wykona po naciśnięciu klawisza "Zacznij przechwytywanie"
 */
void MainWindow::on_stop_cap_button_clicked()
{
    //zakończenie wątka pingującego urządzenie
    pingThread->quit();
    pingThread->wait();

    videoThread->quit();
    videoThread->wait();

    opencvThread->quit();
    opencvThread->wait();
    //zniszczenie obiektu PingThread
    delete pingThread;
    delete videoThread;
    delete opencvThread;
}

/**
 * @brief MainWindow::sendFrame
 * Funkcja wysyłająca pakiet z informacją o zaistniałym błędzie, jeśli nastąpi
 * pod adres wpisany w interfejsie graficznym
 */
void MainWindow::sendFrame()
{

}
