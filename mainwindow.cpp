#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

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
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkThreads()));
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

    timer->start(200);

    //uruchomienie funkcji sprawdzającej stan wątków
}
/**
 * @brief MainWindow::on_stop_cap_button_clicked
 * Funkcja jaka się wykona po naciśnięciu klawisza "Zacznij przechwytywanie"
 */
void MainWindow::on_stop_cap_button_clicked()
{

    qDebug()<<"stop";
    //zakończenie wątka pingującego urządzenie
    if(pingThread->isRunning()){
        pingThread->quit();
        pingThread->wait();
        delete pingThread;
    }

    if(videoThread->isRunning()){
        videoThread->quit();
        videoThread->wait();
        delete videoThread;
    }

    if(opencvThread->isRunning()){
        opencvThread->quit();
        opencvThread->wait();
        delete opencvThread;
    }

    ui->status_label->setText("Zatrzymano");
}

/**
 * @brief MainWindow::sendFrame
 * Funkcja wysyłająca pakiet z informacją o zaistniałym błędzie, jeśli nastąpi
 * pod adres wpisany w interfejsie graficznym
 */
void MainWindow::sendFrame(int code)
{

}

/**
 * @brief MainWindow::checkThreads
 * Funkcja monitorująca stan poszczególnych wątków, i jeśli któryś z nich zostanie zakończony to wysłany zostanie komunikat o błędzie
 */
void MainWindow::checkThreads(){

    //jeśli któryś z wątków zostanie zatrzymany to, zatrzyma się timer, sprawdzony zostanie stan poszczególnych wątków i
    //wywołana zostanie metoda on stop button clicked
    //na podstawie tego jaki wątek został zatrzymany zostanie wyemitowany odpowiedni pakiet z informacją o błędzie
    if(pingThread->isFinished() || videoThread->isFinished() || opencvThread->isFinished()){

        timer->stop();
        if(pingThread->isFinished()){
            //błąd polecenia ping, błąd połączenia
        }else if (videoThread->isFinished()) {
            //błąd połączenia video (libavc)
        }else{
            //błąd połączenia video (opencv)
        }
        on_stop_cap_button_clicked();       //zatrzymanie wszytkich wątków
    }

}
