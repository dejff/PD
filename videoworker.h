#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H
#include <QThread>
#include <vlc/libvlc.h>
#include <vlc/vlc.h>
#include <vlc/libvlc_media.h>
#include "ui_mainwindow.h"
#include "libavutil/mathematics.h"
#include <stdio.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#define INBUF_SIZE 4096

class VideoWorker: public QThread
{
public:
    VideoWorker(QString url, Ui::MainWindow *ui);
    ~VideoWorker();
    void run() override;
    void processVideo();
//    void streamProcess();
//    void test();
    void stopVideo();
private:
    Ui::MainWindow *ui;
    libvlc_instance_t *instance;
    libvlc_media_t *m;
    libvlc_media_player_t *mp;
    QString url;
    AVCodec *codec;
    AVCodecContext *c;
    int frame, got_picture, len;
    AVFrame *picture;
    uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
    char buf[1024];
    AVPacket avpkt;

};

#endif // VIDEOTHREAD_H
