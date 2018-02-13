#include "videothread.h"
#include <QDebug>

VideoThread::VideoThread(QString url, Ui::MainWindow *ui)
{
    this->url = url;
    this->ui = ui;
}

VideoThread::~VideoThread()
{
    avcodec_close(c);
    av_free(c);
    av_free(picture);
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
    libvlc_release(instance);
}

void VideoThread::run(){
//    processVideo();
    streamProcess();
    exec();
}

void VideoThread::processVideo()
{
    qDebug()<<"process video";

    instance = libvlc_new(0, NULL);
    mp = libvlc_media_player_new(instance);
    qDebug()<<"instancja?";
//    QString addr_exp = "rtsp://admin:admin@"+ip+":554";
    QByteArray byteArray = url.toUtf8();
    const char *ch_arr = byteArray.data();
    m = libvlc_media_new_location(instance, ch_arr);
    libvlc_media_player_set_media(mp, m);
//    qDebug()<<"wys.: "+QString::number(libvlc_video_get_height(mp));
//    libvlc_media_player_set_xwindow(mp, winId);
    libvlc_media_player_play (mp);

//    qDebug()<<libvlc_media_get_type(mp);
    printf("wys.: %d", libvlc_audio_get_delay(mp));
    qDebug()<<"program działa"+libvlc_audio_get_delay(mp);
}

void VideoThread::streamProcess()
{
//    qDebug()<<"stream process";
//    this->c= NULL;/*
//    int frame, got_picture, len;
//    AVFrame *picture;
//    uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
//    char buf[1024];
//    AVPacket avpkt;*/

////    avcodec_init();

//    av_register_all();
//    avformat_network_init();

//    if(avformat_open_input(&c, "rtsp://admin:admin@192.168.8.10:554",NULL,NULL) != 0){
//        return EXIT_FAILURE;
//    }

//    if(avformat_find_stream_info(c,NULL) < 0){
//        return EXIT_FAILURE;
//    }

//    av_init_packet(&avpkt);


//    memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

//    codec = avcodec_find_decoder(CODEC_ID_H264);
//    if (!codec) {
//        fprintf(stderr, "codec not found\n");
//        exit(1);
//    }

//    c= avcodec_alloc_context3(codec);
//    picture= avcodec_alloc_frame();

//    if(codec->capabilities&CODEC_CAP_TRUNCATED) c->flags|= CODEC_FLAG_TRUNCATED;

//    if (avcodec_open2(c, codec, NULL) < 0) {
//        fprintf(stderr, "could not open codec\n");
//        exit(1);
//    }

//    qDebug()<<"jestem tutaj";
//    frame=0;
//    for(;;){
//        avpkt.data = inbuf;
//            while (avpkt.size > 0) {
//                len = avcodec_decode_video2(c, picture, &got_picture, &avpkt);
//                if (len < 0) {
//                    fprintf(stderr, "Error while decoding frame %d\n", frame);
//                    exit(1);
//                }
//                if (got_picture) {
//                    printf("saving frame %3d\n", frame);
//                    fflush(stdout);

//                    /* the picture is allocated by the decoder. no need to
//                       free it */
////                    snprintf(buf, sizeof(buf), outfilename, frame);
////                    pgm_save(picture->data[0], picture->linesize[0], c->width, c->height, buf);
//                    frame++;
//                }
//                avpkt.size -= len;
//                avpkt.data += len;
//            }
//    }

//    qDebug()<<"a teraz tutaj";

//    len = avcodec_decode_video2(c, picture, &got_picture, &avpkt);
//        if (got_picture) {
//            printf("saving last frame %3d\n", frame);
//            fflush(stdout);

////            qDebug()<<QString::number(c->bit_rate);

//            /* the picture is allocated by the decoder. no need to
//               free it */
////            snprintf(buf, sizeof(buf), outfilename, frame);
////            pgm_save(picture->data[0], picture->linesize[0],c->width, c->height, buf);
//            frame++;
//        }

////        fclose(f);


}

void VideoThread::test()
{
    AVFormatContext* context = avformat_alloc_context();
//    AVCodecContext* ccontext = avcodec_alloc_context3();

    av_register_all();
    avformat_network_init();

    if(avformat_open_input(&context,url.toUtf8().data(),NULL,NULL) != 0){
        QThread::quit();
//        return EXIT_FAILURE;
    }

    if(avformat_find_stream_info(context,NULL) < 0){
        QThread::quit();
//        return EXIT_FAILURE;
    }

    AVPacket packet;
    av_init_packet(&packet);

    AVFormatContext* oc = avformat_alloc_context();

}
