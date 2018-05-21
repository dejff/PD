#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    connectionError = ErrorEnums::WAIT_FOR_PING;
    credentialError = ErrorEnums::NO_ERROR;
    freezeError = ErrorEnums::NO_ERROR;

    server = new QTcpServer(this);
    QValidator *portValidator = new QIntValidator(0, 65535, this);      //zdefiniowanie walidatora dla pola port zakres od 0 do 65535
    ui->setupUi(this);

    //ustawienie rozmiarów okienka - wyłączenie możliwości rozciągania okienka
    this->setFixedHeight(this->height());
    this->setFixedWidth(this->width());
    ui->ip_addr->setInputMask("000.000.000.000");       //maska pola do wpisywania adresu IP
    ui->listenPort->setValidator(portValidator);
    QStringList connectionTypes;
    connectionTypes << "RTSP"<< "UDP" << "HTTP" << "FREEZE TEST" ;

    //wstępna konfiguracja pól - które mają być powyłączane i jakie checkboxy zaznaczone
    ui->portCheckBox->setChecked(false);
    ui->listenPort->setDisabled(true);
    ui->checkBox->setChecked(false);                    //ustawienie wartości początkowych dla pól hasło, login i checkboxa
    ui->loginField->setDisabled(true);
    ui->passwordField->setDisabled(true);
    ui->protocolType->addItems(connectionTypes);
    ui->nameField->setDisabled(true);
    ui->nameCheckBox->setChecked(false);
    ui->streamPortChckbx->setChecked(false);
    ui->portField->setDisabled(true);

    //konfiguracja skrótów klawiaturowych Ctrl+r dla uruchomienia przechwytywanie oraz Ctrl+s dla zatrzymania
    startShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this); //ustawienie skrótu na rozpoczęcie skanowania
    stopShortcut = new QShortcut(QKeySequence(Qt::CTRL+Qt::Key_S), this);   //ustawienie skrótu na zakończenie skanowania

    connect(startShortcut, SIGNAL(activated()),this, SLOT(on_start_cap_button_clicked()));  //połączenie skrtótu klawiszowego z przyciskiem "Zacznij przechwytywanie"
    connect(stopShortcut, SIGNAL(activated()), this, SLOT(on_stop_cap_button_clicked()));   //Połączenie skrótu klawiszowego z przyciskiem "Zatrzyma przechwytywanie"
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxClicked()));
    connect(ui->portCheckBox, SIGNAL(toggled(bool)), this, SLOT(portCheckBoxClicked()));
    connect(ui->nameCheckBox, SIGNAL(toggled(bool)), this, SLOT(nameCheckBoxClicked()));
    connect(ui->streamPortChckbx, SIGNAL(toggled(bool)), this, SLOT(streamPortChckxbClicked()));
    ui->passwordField->setEchoMode(QLineEdit::Password);                                                            //ustawianie pola hasła
    ui->passwordField->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);      //wyłaczenie wyświatlania wpisywanych znaków w polu hasła
    ui->passwordField->setPlaceholderText("Hasło");
    ui->loginField->setPlaceholderText("Login");
    QFileInfo file(IMAGE_PATH);
    absFilePath = file.absoluteFilePath();
    ui->videoLabel->setPixmap(QPixmap(absFilePath));                            //załadowanie obrazka wyświetlanego w sytuacji kiedy, nie jest wyświetlany obraz ze strumienia wideo
    ui->videoLabel->setScaledContents(true);



    //REJESTROWANIE TYPÓW WŁASNYCH
    qRegisterMetaType<ErrorEnums>("ErrorEnums");
    qRegisterMetaType<Mat>("Mat");
    
    //INICJALIZACJA KLAS WORKERÓW DLA WĄTKÓW
    pingWorker = new PingWorker;
    pingWorker->moveToThread(&pingThread);
    opencvWorker = new OpencvWorker;
    opencvWorker->moveToThread(&openCvThread);
    videoWorker = new VideoWorker;
    videoWorker->moveToThread(&videoThread);

    //ŁĄCZENIE WĄTKÓW Z KLASAMI WORKERÓW - ZAMYKANIE WĄTKÓW
    connect(&pingThread, SIGNAL(finished()), pingWorker, SLOT(stopPing()));
    connect(&openCvThread, SIGNAL(finished()), opencvWorker, SLOT(stopCapture()));
    connect(opencvWorker, SIGNAL(capStopped()), this, SLOT(checkCapStopped()));
    connect(&videoThread, SIGNAL(finished()), videoWorker, SLOT(stopVideo()));

    //POŁĄCZNIE METOD PODCZAS INICJALIZACJI WĄTKÓW
    connect(this, SIGNAL(capturePing(QString)), pingWorker, SLOT(sniff(QString))) ;
    connect(this, SIGNAL(playStream(QString)), opencvWorker, SLOT(capture(QString)));
    connect(this, SIGNAL(runVideoCodec(QString)), videoWorker, SLOT(processVideo(QString)));

    //POŁĄCZENIE METOD OCZEKUJĄCYCH NA INFORMACJĘ ZWROTNĄ Z WĄTKÓW
    connect(pingWorker, SIGNAL(pingReturnMessage(ErrorEnums)), this, SLOT(checkPing(ErrorEnums)));
    connect(pingWorker, SIGNAL(sendParams(double, double)), this, SLOT(getPingParams(double, double)));
    connect(opencvWorker, SIGNAL(openCvReturnMsg(ErrorEnums)), this, SLOT(checkVideoStream(ErrorEnums)));
    connect(opencvWorker, SIGNAL(returnFrame(Mat)), this, SLOT(getVideoFrame(Mat)));
    connect(videoWorker, SIGNAL(sendVideoParams(int, int, QString)), this, SLOT(getVideoInfo(int, int, QString)));
    connect(opencvWorker, SIGNAL(diffLevel(QString)), this, SLOT(getDiffLevel(QString)));
    
    //POŁĄCZENIE SLOTU SERVERA Z FUNKCJĄ ZWRACAJĄCĄ INFORMACJE O STANIE URZĄDZENIA
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));


//    timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(checkThreads()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete pingWorker;
    delete opencvWorker;
    delete videoWorker;
}

/**
 * @brief MainWindow::on_start_cap_button_clicked
 * Funkcja jaka się wykona po naciśnięciu klawisza "Zacznij przechwytywanie"
 */
void MainWindow::on_start_cap_button_clicked()
{
    connectionError = ErrorEnums::WAIT_FOR_PING;
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
        }else if(ui->protocolType->currentText()=="RTSP"){
            url = "udp://";
        }else{
            url = "http://";
        }

        if(!ui->portField->text().isEmpty() && ui->streamPortChckbx->isChecked()){
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

        if(ui->portCheckBox->isChecked()){
            ui->listenPort->setDisabled(true);
        }

        ui->protocolType->setDisabled(true);
        ui->ip_addr->setDisabled(true);
        ui->portField->setDisabled(true);
        ui->nameCheckBox->setDisabled(true);
        ui->portCheckBox->setDisabled(true);
        ui->checkBox->setDisabled(true);
        ui->streamPortChckbx->setDisabled(true);


        //URUCHAMIANIE WSZYSTKICH WĄTKÓW
        
        //NAJPIERW SPRAWDZAMY CZY ADRES KTÓRY WPISALIŚMY JEST OSIĄGALNY - ODPOWIADA NA PING

        if(ui->protocolType->currentText()=="FREEZE TEST")
        {
            url = "./lena.avi";
        }

        connectionError = PingWorker::checkConnection(ui->ip_addr->text());

        //Sprawdzanie czy jest połączenie z kamerą - odpowiada na ping
        if(connectionError==ErrorEnums::CONNECTION_ERROR)
        {
            msg.setText("Urządzenie o podanym IP nie odpowiada");
            msg.exec();
            on_stop_cap_button_clicked();
        }
        else
        {
            credentialError = VideoWorker::checkCredentials(url);
            
            //Sprawdzenie czy wpisane hasło i login są poprawne
            if(credentialError==ErrorEnums::CREDENTIALS_ERROR)
            {
                msg.setText("Problem z uwierzytelnieniem");
                msg.exec();
                on_stop_cap_button_clicked();
            }
            else
            {
                runVideoCodec(url);
                videoThread.start();
                capturePing(ui->ip_addr->text());
                pingThread.start();
                qDebug()<<url;
                playStream(url);
                openCvThread.start();
                
                if(!ui->listenPort->text().trimmed().isEmpty() && ui->portCheckBox->isChecked())   //sprawdzenie czy checkbox portu został zaznaczony i czy pole zostało wypełnione
                {
                    waitForRequest(ui->listenPort->text().toInt());
                }
                else
                {
                    waitForRequest(DEFAULT_PORT);
                }
                
                ui->status_label->setText("Program działa");

                //uruchomienie funkcji sprawdzającej stan wątków
//                timer->start(200);
            }
            
        }

    }

}
/**
 * @brief MainWindow::on_stop_cap_button_clicked
 * Funkcja jaka się wykona po naciśnięciu klawisza "Zatrzymaj przechwytywanie"
 */
void MainWindow::on_stop_cap_button_clicked()
{
    qDebug()<<"Test w on stop cap button clicked";

//    if(timer->isActive()){
//        timer->stop();
//    }

    if(server->isListening()){
        server->close();
    }

    qDebug()<<"serwer zamknięty";

    ui->status_label->setText("Zatrzymano");

    ui->stop_cap_button->setEnabled(false);
    ui->start_cap_button->setEnabled(true);

    //zakończenie wątka pingującego urządzenie
    if(pingThread.isRunning()){
        pingThread.quit();
        pingThread.wait();
        qDebug()<<"ping thr. zamknięty i usunięty";

    }

    qDebug()<<"po pingThread";
    //zakończenie wątka przetwarzającego strumień wideo z wykorzystaniem biblioteki openCV
    if(openCvThread.isRunning()){
        if(connectionError==ErrorEnums::CONNECTION_ERROR)
        {
//            qDebug()<<"Zamykam siłą";
//            openCvThread.terminate();
//        }
//        else
//        {
            openCvThread.quit();
            openCvThread.wait();
        }
        qDebug()<<"opencv thr. zamknięty i usunięty";
    }

    qDebug()<<"po opencv";

    //zakończenie wątka przetwarzającego strumień wideo z wykorzystaniem biblioteki libvlc
    if(videoThread.isRunning()){
        videoThread.quit();
        videoThread.wait();
        qDebug()<<"video thr. zamknięty";
    }

    if(ui->nameCheckBox->isChecked()){
        ui->nameField->setDisabled(false);
    }

    if(ui->checkBox->isChecked()){
        ui->passwordField->setDisabled(false);
        ui->loginField->setDisabled(false);
    }

    if(ui->portCheckBox->isChecked()){
        ui->portField->setDisabled(false);
    }

    ui->protocolType->setDisabled(false);
    ui->ip_addr->setDisabled(false);
    ui->portField->setDisabled(false);
    ui->nameCheckBox->setDisabled(false);
    ui->portCheckBox->setDisabled(false);
    ui->checkBox->setDisabled(false);
    ui->streamPortChckbx->setDisabled(false);
    
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


//DO WYWALENIA
/**
 * @brief MainWindow::checkThreads
 * Funkcja monitorująca stan poszczególnych wątków, i jeśli któryś z nich zostanie zakończony to wysłany zostanie komunikat o błędzie
 */
void MainWindow::checkThreads()
{

    //jeśli któryś z wątków zostanie zatrzymany to, zatrzyma się timer, sprawdzony zostanie stan poszczególnych wątków i
    //wywołana zostanie metoda on stop button clicked
    //na podstawie tego jaki wątek został zatrzymany zostanie wyemitowany odpowiedni pakiet z informacją o błędzie
//    if(pingThread.isFinished() || videoThread.isFinished() || openCvThread.isFinished()){
//        qDebug()<<"Jest błąd";
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
    if(!ui->checkBox->isChecked() || ui->protocolType->currentText()=="FREEZE TEST"){
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
    if(!ui->portCheckBox->isChecked() || ui->protocolType->currentText()=="FREEZE TEST"){
        ui->listenPort->setDisabled(true);
    }else{
        ui->listenPort->setDisabled(false);
    }
}

void MainWindow::nameCheckBoxClicked()
{
    if(!ui->nameCheckBox->isChecked() || ui->protocolType->currentText()=="FREEZE TEST"){
		ui->nameField->setDisabled(true);
	}else{
		ui->nameField->setDisabled(false);
	}
}

void MainWindow::streamPortChckxbClicked()
{
    if(!ui->streamPortChckbx->isChecked() || ui->protocolType->currentText()=="FREEZE TEST"){
        ui->portField->setDisabled(true);
    }else{
        ui->portField->setDisabled(false);
    }
}

void MainWindow::checkVideoStream(ErrorEnums err){

    if(err==ErrorEnums::FREEZE_ERROR)
    {
        freezeError = ErrorEnums::FREEZE_ERROR;
        if(PingWorker::checkConnection(ui->ip_addr->text()) == ErrorEnums::CONNECTION_ERROR)
        {
            msg.setText("Połączenie zostało zerwane");
        }
        else
        {
            msg.setText("Wykryto zamrożenie obrazu");
        }

        msg.exec();
//        openCvThread.quit();
//        openCvThread.wait();
        on_stop_cap_button_clicked();
    }

}

void MainWindow::checkPing(ErrorEnums err)
{
    if(err==ErrorEnums::CONNECTION_ERROR){
        connectionError = ErrorEnums::CONNECTION_ERROR;
        msg.setText("Połączenie zostało zerwane");
        msg.exec();
        qDebug()<<"quit()";
        pingThread.quit();
        qDebug()<<"wait()";
        pingThread.wait();
        qDebug()<<"po wait()";
        on_stop_cap_button_clicked();
    }
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

void MainWindow::checkCapStopped()
{
    ui->videoLabel->setPixmap(QPixmap(absFilePath));
    ui->videoLabel->setScaledContents(true);
}

void MainWindow::credentialsCheck(ErrorEnums err)
{
    connectionError = err;
    if(err==ErrorEnums::CREDENTIALS_ERROR)
    {
        qDebug()<<"Błąd poświadczeń";
    }
}

void MainWindow::getPingParams(double lathency, double jitter)
{
    !isnan(jitter) ? jitter : jitter=0.0;
    ui->jitterLabel->setText(QString::number(jitter));
    ui->lathency_label->setText(QString::number(lathency));
    lathencyVal = QString::number(lathency);
    jitterVal = QString::number(jitter);
}

void MainWindow::getVideoInfo(int width, int height, QString codec)
{
    ui->resolutionLabel->setText(QString::number(width)+"x"+QString::number(height));
    ui->codec_label->setText(codec);
    resolutionVal = QString::number(width)+"x"+QString::number(height);
    codecVal = codec;
}

void MainWindow::waitForRequest(int socPort)
{

    if(!server->listen(QHostAddress::Any, socPort))
    {
        qDebug()<<"Serwer nie został uruchomiony";
    }
    else
    {
        qDebug()<<"Serwer uruchomiony";
    }
}


void MainWindow::newConnection()
{

    response = "";
    mutex.lock();
    response = "Adres IP urządzenia: "+ui->ip_addr->text()+
            "\nPort: "+(ui->portField->text().trimmed().isEmpty() ? "domyślny" : ui->portField->text())+
            "\nRozdzielczość: "+resolutionVal+
            "\nKodek: "+codecVal+
            "\nPing: "+lathencyVal+
            "\nJitter: "+jitterVal+
            "\nRóżnica ramek: "+diffVal+"\n"
            ;
    QTcpSocket *socket = server->nextPendingConnection();

    socket->write(response.toUtf8());
    socket->flush();

    socket->waitForBytesWritten(3000);
    mutex.unlock();

    socket->close();
}

void MainWindow::getDiffLevel(QString diff)
{
    ui->diffLabel->setText(diff);
    diffVal = diff;
}
