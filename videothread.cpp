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
}
