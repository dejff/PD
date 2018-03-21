#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <sys/socket.h>
#include "pingthread.h"
#include "videothread.h"
#include "opencvthread.h"
#include "socketthread.h"
#include <QTimer>
#include <QMessageBox>

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
    void checkThreads();
    void checkBoxClicked();
    void portCheckBoxClicked();
	void nameCheckBoxClicked();
private:
    QImage img;
    QString port, url, credentials, absFilePath;
    Ui::MainWindow *ui;
    QShortcut *startShortcut;
    QShortcut *stopShortcut;
    PingThread *pingThread;
    VideoThread *videoThread;
    OpencvThread *opencvThread;
    SocketThread *socketThread;
    QTimer *timer;
};

#endif // MAINWINDOW_H
