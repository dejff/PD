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
    pingTimer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkThreads()));
    connect(pingTimer, SIGNAL(timeout()), this, SLOT(ping()));
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
    ui->start_cap_button->setEnabled(false);
    ui->stop_cap_button->setEnabled(true);
    //inicjalizacja wątków
    pingThread = new PingThread(ui->ip_addr->text());
    videoThread = new VideoThread(ui->ip_addr->text(), ui->fps_label->winId());
    opencvThread = new OpencvThread(ui->ip_addr->text());
    //uruchomienie wątków
    pingThread->start();
    videoThread->start();
    opencvThread->start();

    ui->status_label->setText("Program działa");

    timer->start(200);
    pingTimer->start(200);

    //uruchomienie funkcji sprawdzającej stan wątków
}
/**
 * @brief MainWindow::on_stop_cap_button_clicked
 * Funkcja jaka się wykona po naciśnięciu klawisza "Zacznij przechwytywanie"
 */
void MainWindow::on_stop_cap_button_clicked()
{
    ui->stop_cap_button->setEnabled(false);
    ui->start_cap_button->setEnabled(true);
    qDebug()<<"stop";
    //zakończenie wątka pingującego urządzenie
    if(pingThread->isRunning()){
        pingThread->quit();
        pingThread->wait();
        delete pingThread;
    }

    //zakończenie wątka przetwarzającego strumień wideo z wykorzystaniem biblioteki libvlc
    if(videoThread->isRunning()){
        videoThread->quit();
        videoThread->wait();
        delete videoThread;
    }

    //zakończenie wątka przetwarzającego strumień wideo z wykorzystaniem biblioteki openCV
    if(opencvThread->isRunning()){
        opencvThread->quit();
        opencvThread->wait();
        delete opencvThread;
    }

    if(timer->isActive()){
        timer->stop();
    }
    if(pingTimer->isActive()){
        pingTimer->stop();
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
    qDebug()<<"code sent: "+QString::number(code);
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
            qDebug()<<"ping nie działa";
            sendFrame(1);
            //błąd polecenia ping, błąd połączenia
        }else if (videoThread->isFinished()) {
            qDebug()<<"video nie działa";
            sendFrame(2);
            //błąd połączenia video (libavc)
        }else{
            qDebug()<<"opencv nie działa";
            sendFrame(3);
            //błąd połączenia video (opencv)
        }
        on_stop_cap_button_clicked();       //zatrzymanie wszytkich wątków
    }

}

/**
 * @brief MainWindow::ping
 * Funkcja pingująca urządzenie znajdujące się pod adresem
 */
//void MainWindow::ping()
//{
//    qDebug()<<"poszedł ping";
//    QString ping_exp = "ping "+ui->ip_addr->text()+" -c 5 >/dev/null 2>&1";   //wykonanie pinga bez wyświetlania informacji w terminalu
//    QByteArray ping_char = ping_exp.toUtf8();               //przekonwertowania zmiennej typu string na bytearray, żeby dalej można było ją przekonwertować na typ char

//    system(ping_char.data());
//}
