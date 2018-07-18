#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H
#include <QThread>
#include "ui_mainwindow.h"
#include <stdio.h>
#include <iostream>
#include "errorenums.h"
#include <QTimer>

using namespace std;

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

class VideoWorker: public QObject{

    Q_OBJECT
public:
    VideoWorker();
    ~VideoWorker();
    static ErrorEnums checkCredentials(QString url);
private:
    AVFormatContext *formatContext = NULL;
    AVCodecContext *codecContextOriginal = NULL;
    AVCodecContext *codecContext = NULL;
    AVCodec *codec = NULL;
    AVFrame *frame = NULL;
    AVFrame *frameRGB = NULL;
    AVPacket packet;
    int frameFinished;
    char *stream;
    int videoStream, i, iloscBajtow;
    uint8_t *bufor = NULL;
    struct SwsContext *sws_ctx = NULL;
    QString url;
    QTimer *timer;
    bool grabFrame;
    QString tmpText;

private slots:
    void grabFrameFn();
    
public slots:
    void stopVideo();
    void processVideo(const QString url);
signals:
    void returnData();
    void returnError(const ErrorEnums err);
    void sendVideoParams(const int width, const int height, const QString codec);
};

#endif // VIDEOTHREAD_H
