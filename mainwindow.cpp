#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileInfo>

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
    ui->nameField->setDisabled(true);
    ui->nameCheckBox->setChecked(false);
    startShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this); //ustawienie skrótu na rozpoczęcie skanowania
    stopShortcut = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_S), this);   //ustawienie skrótu na zakończenie skanowania
    connect(startShortcut, SIGNAL(activated()),this, SLOT(on_start_cap_button_clicked()));  //połączenie skrtótu klawiszowego z przyciskiem "Zacznij przechwytywanie"
    connect(stopShortcut, SIGNAL(activated()), this, SLOT(on_stop_cap_button_clicked()));   //Połączenie skrótu klawiszowego z przyciskiem "Zatrzyma przechwytywanie"
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxClicked()));
    connect(ui->portCheckBox, SIGNAL(toggled(bool)), this, SLOT(portCheckBoxClicked()));
    connect(ui->nameCheckBox, SIGNAL(toggled(bool)), this, SLOT(nameCheckBoxClicked()));
    ui->passwordField->setEchoMode(QLineEdit::Password);                                                            //ustawianie pola hasła
    ui->passwordField->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);      //wyłaczenie wyświatlania wpisywanych znaków w polu hasła
    ui->passwordField->setPlaceholderText("Hasło");
    ui->loginField->setPlaceholderText("Login");
    QFileInfo file(IMAGE_PATH);
    absFilePath = file.absoluteFilePath();
    ui->videoLabel->setPixmap(QPixmap(absFilePath));                            //załadowanie obrazka wyświetlanego w sytuacji kiedy, nie jest wyświetlany obraz ze strumienia wideo
    ui->videoLabel->setScaledContents(true);

    //INICJALIZACJA KLAS WORKERÓW DLA WĄTKÓW
    pingWorker = new PingWorker;
    pingWorker->moveToThread(&pingThread);
    opencvWorker = new OpencvWorker;
    opencvWorker->moveToThread(&openCvThread);

    //ŁĄCZENIE WĄTKÓW Z KLASAMI WORKERÓW - ZAMYKANIE WĄTKÓW
    connect(&pingThread, SIGNAL(finished()), pingWorker, SLOT(stopPing()));
    connect(&openCvThread, SIGNAL(finished()), opencvWorker, SLOT(stopCapture()));

    //POŁĄCZNIE METOD PODCZAS INICJALIZACJI WĄTKÓW
    connect(this, SIGNAL(capturePing(QString)), pingWorker, SLOT(sniff(QString))) ;
    connect(this, SIGNAL(playStream(QString)), opencvWorker, SLOT(capture(QString)));

    //POŁĄCZENIE METOD OCZEKUJĄCYCH NA INFORMACJĘ ZWROTNĄ Z WĄTKÓW
    connect(pingWorker, SIGNAL(pingReturnMessage(QString)), this, SLOT(checkPing(QString)));
    connect(opencvWorker, SIGNAL(openCvReturnMsg(QString)), this, SLOT(checkVideoStream(QString)));
    qRegisterMetaType<Mat>("Mat");
    connect(opencvWorker, SIGNAL(returnFrame(Mat)), this, SLOT(getVideoFrame(Mat)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkThreads()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete pingWorker;
    delete opencvWorker;
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
            msg.setText("Proszę uzupełnić pola ip, hasła i loginu.\nPole port jeśli nie uzupełnione oznacza port domyślny.");
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

        if(ui->nameCheckBox->isChecked()){
            url += "/" + ui->nameField->text();
        }


        //aktywacja przycisku zatrzymywania protramu i wyłączenie przycisku uruchamiania programu - nie można dwa razy uruchomić przechwytywania
        ui->start_cap_button->setEnabled(false);
        ui->stop_cap_button->setEnabled(true);

        //Dezaktywacja wszystkich pól - nie można zmienić parametrów przechwytywania podczas działania programu

        if(ui->nameCheckBox->isChecked()){
            ui->nameField->setDisabled(true);
        }

        if(ui->checkBox->isChecked()){
            ui->passwordField->setDisabled(true);
            ui->loginField->setDisabled(true);
        }

        ui->protocolType->setDisabled(true);
        ui->ip_addr->setDisabled(true);
        ui->portField->setDisabled(true);
        //URUCHAMIANIE WSZYSTKICH WĄTKÓW
        capturePing(ui->ip_addr->text());
        pingThread.start();

        playStream(url);
        openCvThread.start();
//        videoThread = new VideoThread(url, ui);                 //do tego wątku przekazywany jest sparsowany adres url urządzenia
//        socketThread = new SocketThread(ui);

        ui->status_label->setText("Program działa");
//        capturePing(ui->ip_addr->text());

        //uruchomienie funkcji sprawdzającej stan wątków
//        timer->start(200);
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

    //zakończenie wątka nasłuchującego żądań z sieci
//    if(socketThread->isRunning()){
//        socketThread->quit();
//        socketThread->wait();
//        delete socketThread;
//    }

    //zakończenie wątka pingującego urządzenie
    if(pingThread.isRunning()){
        pingThread.quit();
        pingThread.wait();
        qDebug()<<"ping thr. zamknięty i usunięty";

    }
    //zakończenie wątka przetwarzającego strumień wideo z wykorzystaniem biblioteki openCV
    if(openCvThread.isRunning()){
        openCvThread.quit();
        openCvThread.wait();
        qDebug()<<"opencv thr. zamknięty i usunięty";
    }
    //zakończenie wątka przetwarzającego strumień wideo z wykorzystaniem biblioteki libvlc
//    if(videoThread->isRunning()){
//        qDebug()<<"video thr. zamykanie";
//        videoThread->stopVideo();
//        videoThread->quit();
//        videoThread->wait();
//        qDebug()<<"video thr. zamknięty";
//        qDebug()<<"usunięte video";
//    }

//    if(openCvThread.isFinished()){
//      qDebug()<<absFilePath;
//        ui->videoLabel->setPixmap(QPixmap(absFilePath));
//    }
//    ui->videoLabel->setScaledContents(true);
//    ui->videoLabel->setPixmap(QPixmap(absFilePath));

    //        pingThread->stopPing();
    if(ui->nameCheckBox->isChecked()){
        ui->nameField->setDisabled(false);
    }

    if(ui->checkBox->isChecked()){
        ui->passwordField->setDisabled(false);
        ui->loginField->setDisabled(false);
    }

    ui->protocolType->setDisabled(false);
    ui->ip_addr->setDisabled(false);
    ui->portField->setDisabled(false);
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
//    qDebug()<<"opencv "+QString(opencvThread->isFinished() ? "false" : "true")+"\n";
//    qDebug()<<"ping "+QString(pingThread->isFinished() ? "false" : "true")+"\n";
//    qDebug()<<"sock "+QString(socketThread->isFinished() ? "false" : "true")+"\n";
}

/**
 * @brief MainWindow::checkThreads
 * Funkcja monitorująca stan poszczególnych wątków, i jeśli któryś z nich zostanie zakończony to wysłany zostanie komunikat o błędzie
 */
void MainWindow::checkThreads()
{

    //jeśli któryś z wątków zostanie zatrzymany to, zatrzyma się timer, sprawdzony zostanie stan poszczególnych wątków i
    //wywołana zostanie metoda on stop button clicked
    //na podstawie tego jaki wątek został zatrzymany zostanie wyemitowany odpowiedni pakiet z informacją o błędzie
//    if(pingThread->isFinished() || videoThread->isFinished() || opencvThread->isFinished()){

        QMessageBox msg;
//        qDebug()<<"opencv thread work: "+QString(opencvThread->isFinished() ? "false" : "true")+"\n";
//        timer->stop();
//        if(pingThread->isFinished()){
//            //błąd polecenia ping, błąd połączenia
////            qDebug()<<"błąd połączenia";
//            msg.setText("Brak połączenia z urządzeniem");
//            sendFrame(1);
//            on_stop_cap_button_clicked();       //zatrzymanie wszytkich wątków
//            msg.exec();
//        }else if (videoThread->isFinished()) {
//            //błąd połączenia video (libavc) - pobieranie parametrów strumienia wideo
////            qDebug()<<"błąd video thread";
//            msg.setText("Problem z połączeniem");
//            sendFrame(2);
//            on_stop_cap_button_clicked();       //zatrzymanie wszytkich wątków
//            msg.exec();
//        }else{
//            //błąd połączenia video (opencv) - sprawdzanie czy wystąpiło zamrożenie obrazu
////            qDebug()<<"błąd open cv";
//            if(pingThread->isFinished()){
//                msg.setText("Problem z połączeniem");
//                sendFrame(1);
//                opencvThread->quit();
//                on_stop_cap_button_clicked();       //zatrzymanie wszytkich wątków
//                msg.exec();
//            }else{
//                opencvThread->quit();
//                msg.setText("Wykryto zamrożenie obrazu");
//                sendFrame(3);
//                on_stop_cap_button_clicked();       //zatrzymanie wszytkich wątków
//                msg.exec();
//            }
//        }
//    }
}

/**
 * @brief MainWindow::checkBoxClicked
 * Metoda aktuwująca i dezaktywująca pola hasła i loginu
 */
void MainWindow::checkBoxClicked()
{
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
void MainWindow::portCheckBoxClicked()
{
    if(!ui->portCheckBox->isChecked()){
        ui->listenPort->setDisabled(true);
    }else{
        ui->listenPort->setDisabled(false);
    }
}

void MainWindow::nameCheckBoxClicked()
{
	if(!ui->nameCheckBox->isChecked()){
		ui->nameField->setDisabled(true);
	}else{
		ui->nameField->setDisabled(false);
	}
}

void MainWindow::checkVideoStream(QString string){
    qDebug()<<string;
}

void MainWindow::checkPing(QString string)
{
    qDebug()<<string;
}

void MainWindow::checkFreezeThread(QString string){
    qDebug()<<string;
}

void MainWindow::getVideoFrame(Mat frame)
{
    img = QImage((const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
    ui->videoLabel->setScaledContents(true);
    ui->videoLabel->setPixmap(QPixmap::fromImage(img));
    ui->videoLabel->resize(ui->videoLabel->pixmap()->size());
}
