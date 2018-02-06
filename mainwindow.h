#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include "pingthread.h"
#include "videothread.h"
#include "opencvthread.h"
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void sendFrame(int code);
    ~MainWindow();

private slots:
    void on_stop_cap_button_clicked();
    void on_start_cap_button_clicked();
    void checkThreads();

private:
    Ui::MainWindow *ui;
    QShortcut *startShortcut;
    QShortcut *stopShortcut;
    PingThread *pingThread;
    VideoThread *videoThread;
    OpencvThread *opencvThread;
    QTimer *timer;
};

#endif // MAINWINDOW_H
