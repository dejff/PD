#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <sys/socket.h>
#include "pingworker.h"
#include "videoworker.h"
#include "opencvworker.h"
//#include "socketworker.h"
#include "freezeworker.h"
#include <QTimer>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
#include <QFileInfo>
#include <cmath>
#include <QTcpServer>
#include <QTcpSocket>

#define DEFAULT_PORT 50000

using namespace cv;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    const QString IMAGE_PATH = "./no_video.jpg";
    explicit MainWindow(QWidget *parent = 0);
    void sendFrame(int code);
    void waitForRequest(int port);
    ~MainWindow();

private slots:
    void on_stop_cap_button_clicked();
    void on_start_cap_button_clicked();
    void checkThreads();		//dostosować do poprawek w wątkach - może będzie potrzeba sprawdzania tylko jednej zmiennej
    void checkBoxClicked();
    void portCheckBoxClicked();
    void nameCheckBoxClicked();
    void newConnection();

public slots:
    //SLOTY NASŁUCHUJĄCE INFORMACJI ZWROTNEJ Z WĄTKÓW O TYM CZY WYSTĄPIŁ JAKIŚ BŁĄD
    void checkPing(ErrorEnums err);
    void checkVideoStream(ErrorEnums err);
    void checkFreezeThread(QString string);
    void getVideoFrame(Mat frame);
    void checkCapStopped();
    void credentialsCheck(ErrorEnums err);
    void getPingParams(double lathency, double jitter);
    void getVideoInfo(int width, int height, QString codec);
    void getDiffLevel(QString diff);

private:
    QTcpServer *server;
    QTcpSocket *socket;
    QString response;
    QMutex mutex;
    QMessageBox msg;
    ErrorEnums credentialError, connectionError, freezeError;
    QImage img;
    QString port, url, credentials, absFilePath;
    QString jitterVal, lathencyVal, codecVal, resolutionVal, ipVal, portVal;
    Ui::MainWindow *ui;
    QShortcut *startShortcut;
    QShortcut *stopShortcut;
    PingWorker *pingWorker;
    OpencvWorker *opencvWorker;
//    FreezeWorker *freezeWorker;       //tu jest problem ze strumieniem z innego komputera
    VideoWorker *videoWorker;
//    SocketWorker *socketWorker;
    QTimer *timer, *pingTimer, *openCvTimer, *videoTimer, *socketTimer;
    QThread pingThread, openCvThread, videoThread, socketThread, freezeThread;

signals:
    //SYGNAŁY PRZESYŁAJĄCE SYGNAŁY DO WĄTKÓW PODCZAS INICJALIZACJI
    void capturePing(const QString &);
    void playStream(const QString &);
    void runVideoCodec(const QString &);
    void listenSocket(const QString &);
    void parametersResponse(const QString &);
};

#endif // MAINWINDOW_H
