#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H
#include <QThread>
#include <vlc/libvlc.h>
#include <vlc/vlc.h>
#include <vlc/libvlc_media.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include "libavutil/mathematics.h"
#include <stdio.h>

#define INBUF_SIZE 4096

class VideoThread: public QThread
{
public:
    VideoThread(QString ip, int winId);
    ~VideoThread();
    void run() override;
    void processVideo();
    void streamProcess();
    void test();
private:
    libvlc_instance_t *instance;
    libvlc_media_t *m;
    libvlc_media_player_t *mp;
    QString ip;
    int winId;
    AVCodec *codec;
    AVCodecContext *c;
    int frame, got_picture, len;
    AVFrame *picture;
    uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
    char buf[1024];
    AVPacket avpkt;

};

#endif // VIDEOTHREAD_H
