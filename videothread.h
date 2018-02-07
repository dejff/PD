#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H
#include <QThread>
#include <vlc/libvlc.h>
#include <vlc/vlc.h>
#include <vlc/libvlc_media.h>

class VideoThread: public QThread
{
public:
    VideoThread(QString ip);
    ~VideoThread();
    void run() override;
    void processVideo();
private:
    QString ip;
};

#endif // VIDEOTHREAD_H
