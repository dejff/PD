#include "videothread.h"

VideoThread::VideoThread(QString ip)
{
    this->ip = ip;
}

VideoThread::~VideoThread()
{

}

void VideoThread::run(){

}

void VideoThread::processVideo()
{
    libvlc_instance_t *instance;
    libvlc_media_t *m;
    instance = libvlc_new(0, NULL);
    QString addr_exp = "rtsp://admin:admin@"+ip+":554";
    QByteArray byteArray = addr_exp.toUtf8();
    const char *ch_arr = byteArray.data();
    m = libvlc_media_new_location(instance, ch_arr);
}
