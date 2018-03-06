#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QValidator *portValidator = new QIntValidator(0, 65535, this);      //zdefiniowanie walidatora dla pola port zakres od 0 do 65535
    ui->setupUi(this);
    this->setFixedHeight(this->height());
    this->setFixedWidth(this->width());
    ui->ip_addr->setInputMask("000.000.000.000");       //maska pola do wpisywania adresu IP
    ui->listenPort->setValidator(portValidator);
    QStringList connectionTypes;
    connectionTypes << "RTSP"<<"UDP";
    ui->portCheckBox->setChecked(false);
    ui->listenPort->setDisabled(true);
    ui->checkBox->setChecked(false);                    //ustawienie wartości początkowych dla pól hasło, login i checkboxa
    ui->loginField->setDisabled(true);
    ui->passwordField->setDisabled(true);
    ui->protocolType->addItems(connectionTypes);
    startShortcut = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_R), this);  //ustawienie skrótu na rozpoczęcie skanowania
    stopShortcut = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_S), this);   //ustawienie skrótu na zakończenie skanowania
    connect(startShortcut, SIGNAL(activated()),this, SLOT(on_start_cap_button_clicked()));  //połączenie skrtótu klawiszowego z przyciskiem "Zacznij przechwytywanie"
    connect(stopShortcut, SIGNAL(activated()), this, SLOT(on_stop_cap_button_clicked()));   //Połączenie skrótu klawiszowego z przyciskiem "Zatrzyma przechwytywanie"
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxClicked()));
    connect(ui->portCheckBox, SIGNAL(toggled(bool)), this, SLOT(portCheckBoxClicked()));
    ui->passwordField->setEchoMode(QLineEdit::Password);        //ustawianie pola hasła
    ui->passwordField->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);      //wyłaczenie wyświatlania wpisywanych znaków w polu hasła
    ui->passwordField->setPlaceholderText("Hasło");
    ui->loginField->setPlaceholderText("Login");
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkThreads()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_start_cap_button_clicked
 * Funkcja jaka się wykona po naciśnięciu klawisza "Zacznij przechwytywanie"
 */
void MainWindow::on_start_cap_button_clicked()
{
    QMessageBox msg;
    url = "";
    if(ui->ip_addr->text().isEmpty() || ui->ip_addr->text().length()<=4){       //sprawdzamy czy użtykownik cokolwiek wpisał

        if(ui->checkBox->isChecked()){
            msg.setText("Proszę uzupełnić pola ip, hasła i loginu.\nPole port jeśli nie uzupełnione oznacza posrt domyślny.");
        }else{
            msg.setText("Proszę uzupełnić pole ip.\nPole port jeśli nie uzupełnione oznacza port domyślny.");
        }

        msg.exec();

    }else{

        if(ui->checkBox->isChecked()){              //jeśli zaznaczony jesty checkbox "Zabezpieczone hasłem" to dodajemy do url'a informacje o haśle i loginie
            credentials = ui->loginField->text()+":"+ui->passwordField->text()+"@";
        }else{
            credentials = "";
        }

        if(ui->protocolType->currentText()=="RTSP"){
            url = "rtsp://";
        }else{
            url = "udp://";
        }

        if(!ui->portField->text().isEmpty()){
            url+=credentials+ui->ip_addr->text()+":"+ui->portField->text();
        }else{
            url+=credentials+ui->ip_addr->text();
        }

        ui->start_cap_button->setEnabled(false);
        ui->stop_cap_button->setEnabled(true);
        //inicjalizacja wątków
        pingThread = new PingThread(ui->ip_addr->text());       //do wątku ping przekazywany jest tylko adres ip urządzenia
        videoThread = new VideoThread(url, ui);                 //do tego wątku przekazywany jest sparsowany adres url urządzenia
        opencvThread = new OpencvThread(url, ui);               //do tego wątku przekazywany jest sparsowany adres url urządzenia
        //uruchomienie wątków
        pingThread->start();
        videoThread->start();
        opencvThread->start();
        ui->status_label->setText("Program działa");

        //uruchomienie funkcji sprawdzającej stan wątków
        timer->start(200);
    }

}
/**
 * @brief MainWindow::on_stop_cap_button_clicked
 * Funkcja jaka się wykona po naciśnięciu klawisza "Zatrzymaj przechwytywanie"
 */
void MainWindow::on_stop_cap_button_clicked()
{
    if(timer->isActive()){
        timer->stop();
    }


    ui->status_label->setText("Zatrzymano");

    ui->stop_cap_button->setEnabled(false);
    ui->start_cap_button->setEnabled(true);
    //zakończenie wątka pingującego urządzenie
    if(pingThread->isRunning()){

        pingThread->quit();
        pingThread->wait();
    }

    //zakończenie wątka przetwarzającego strumień wideo z wykorzystaniem biblioteki libvlc
    if(videoThread->isRunning()){

        videoThread->quit();
        videoThread->wait();
    }

    //zakończenie wątka przetwarzającego strumień wideo z wykorzystaniem biblioteki openCV
    if(opencvThread->isRunning()){

        opencvThread->quit();
        opencvThread->wait();
    }

}

/**
 * @brief MainWindow::sendFrame
 * Funkcja wysyłająca pakiet z informacją o zaistniałym błędzie, jeśli nastąpi
 * pod adres wpisany w interfejsie graficznym
 */
void MainWindow::sendFrame(int code)
{
    //do dokończenia
    qDebug()<<"code sent: "+QString::number(code);
}

/**
 * @brief MainWindow::checkThreads
 * Funkcja monitorująca stan poszczególnych wątków, i jeśli któryś z nich zostanie zakończony to wysłany zostanie komunikat o błędzie
 */
void MainWindow::checkThreads(){
    QMessageBox msg;

    //jeśli któryś z wątków zostanie zatrzymany to, zatrzyma się timer, sprawdzony zostanie stan poszczególnych wątków i
    //wywołana zostanie metoda on stop button clicked
    //na podstawie tego jaki wątek został zatrzymany zostanie wyemitowany odpowiedni pakiet z informacją o błędzie
    if(pingThread->isFinished() || videoThread->isFinished() || opencvThread->isFinished()){

        timer->stop();
        if(pingThread->isFinished()){
            //błąd polecenia ping, błąd połączenia
            qDebug()<<"błąd połączenia";
            msg.setText("Brak połączenia z urządzeniem");
            sendFrame(1);
        }else if (videoThread->isFinished()) {
            //błąd połączenia video (libavc) - pobieranie parametrów strumienia wideo
            qDebug()<<"błąd video thread";
            msg.setText("Problem z połączeniem");
            sendFrame(2);
        }else{
            //błąd połączenia video (opencv) - sprawdzanie czy wystąpiło zamrożenie obrazu
            qDebug()<<"błąd open cv";
            if(pingThread->isFinished()){
                msg.setText("Problem z połączeniem");
                sendFrame(1);
                opencvThread->quit();
            }else{
                opencvThread->quit();
                msg.setText("Wykryto zamrożenie obrazu");
                sendFrame(3);
            }
        }

        on_stop_cap_button_clicked();       //zatrzymanie wszytkich wątków
        msg.exec();
    }

}

/**
 * @brief MainWindow::checkBoxClicked
 * Metoda aktuwująca i dezaktywująca pola hasła i loginu
 */
void MainWindow::checkBoxClicked(){
    if(!ui->checkBox->isChecked()){
        ui->passwordField->setDisabled(true);
        ui->loginField->setDisabled(true);
    }else{
        ui->passwordField->setEnabled(true);
        ui->loginField->setEnabled(true);
    }
}

/**
 * @brief MainWindow::portCheckBoxClicked
 * Metoda aktywująca i dezaktywująca pole portu nasłuchującego
 */
void MainWindow::portCheckBoxClicked(){
    if(!ui->portCheckBox->isChecked()){
        ui->listenPort->setDisabled(true);
    }else{
        ui->listenPort->setDisabled(false);
    }
}
