#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <sys/socket.h>
#include "pingworker.h"
#include "videoworker.h"
#include "opencvworker.h"
#include "socketworker.h"
#include "freezeworker.h"
#include <QTimer>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
#include <QFileInfo>

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
    ~MainWindow();

private slots:
    void on_stop_cap_button_clicked();
    void on_start_cap_button_clicked();
    void checkThreads();		//dostosować do poprawek w wątkach - może będzie potrzeba sprawdzania tylko jednej zmiennej
    void checkBoxClicked();
    void portCheckBoxClicked();
    void nameCheckBoxClicked();

public slots:
    //SLOTY NASŁUCHUJĄCE INFORMACJI ZWROTNEJ Z WĄTKÓW O TYM CZY WYSTĄPIŁ JAKIŚ BŁĄD
    void checkPing(QString string);
    void checkVideoStream(QString string);
    void checkFreezeThread(QString string);
    void getVideoFrame(Mat frame);

private:
    QImage img;
    QString port, url, credentials, absFilePath;
    Ui::MainWindow *ui;
    QShortcut *startShortcut;
    QShortcut *stopShortcut;
    PingWorker *pingWorker;
    OpencvWorker *opencvWorker;
//    FreezeWorker *freezeWorker;
    VideoWorker *videoWorker;
//    SocketThread *socketWorker;
    QTimer *timer, *pingTimer, *openCvTimer, *videoTimer, *socketTimer;
    QThread pingThread, openCvThread, videoThread, socketThread, freezeThread;

signals:
    //SYGNAŁY PRZESYŁAJĄCE SYGNAŁY DO WĄTKÓW PODCZAS INICJALIZACJI
    void capturePing(const QString &);
    void playStream(const QString &);
};

#endif // MAINWINDOW_H
