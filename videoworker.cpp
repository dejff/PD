#include <qt5/QtCore/qlogging.h>

#include "videoworker.h"

VideoWorker::VideoWorker()
{

}

VideoWorker::~VideoWorker()
{

}

void VideoWorker::processVideo(const QString url)
{
    qDebug()<<"test";
    AVFormatContext *formatContext = NULL;
        AVCodecContext *codecContextOriginal = NULL;
        AVCodecContext *codecContext = NULL;
        AVCodec *codec = NULL;
        AVFrame *frame = NULL;
        AVFrame *frameRGB = NULL;
        AVPacket packet;
        int frameFinished;
        int videoStream, i, iloscBajtow;
        uint8_t *bufor = NULL;
        struct SwsContext *sws_ctx = NULL;

        avformat_network_init();
        av_register_all();      //rejestrowanie wszystkich dostępnych formatów i kodeków w celu automatycznego wiązania ich ze strumieniem wideo

        if(avformat_open_input(&formatContext, url.toUtf8().data(), NULL, NULL)!=0)
        {
            cout<<"Błąd otwierania strumienia"<<endl;
            emit returnError(ErrorEnums::CREDENTIALS_ERROR);
//            exit(-1);
//            return -1;
        }
        else
        {
            if(avformat_find_stream_info(formatContext, NULL)<0)
            {
                cout<<"Nie można znaleźć informacji o strumieniu";
    //            return -1;
            }

            av_dump_format(formatContext, 0, url.toUtf8().data(), 0);

            videoStream = -1;

            videoStream = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);


            if(videoStream==-1)
            {
//                return -1;      //nie znaleziono strumienia wideo
            }

            codecContextOriginal = formatContext->streams[videoStream]->codec;

            codec = avcodec_find_decoder(codecContextOriginal->codec_id);
            if(codec==NULL)
            {
                cout<<"Nie wspierany kodek";
                stopVideo();
    //            return -1;
            }

            codecContext = avcodec_alloc_context3(codec);
            if(avcodec_copy_context(codecContext, codecContextOriginal)!=0)
            {
                cout<<"Nie można skopiować CodecContext";
    //            return -1;
            }

            if(avcodec_open2(codecContext, codec, NULL)<0)
            {
                cout<<"Nie można otworzyć kodeka";
    //            return -1;
            }

            frame = av_frame_alloc();
            frameRGB = av_frame_alloc();

            if(frameRGB==NULL){
    //            return -1;
            }

            iloscBajtow = avpicture_get_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height);

            bufor = (uint8_t *)av_malloc(iloscBajtow*sizeof(uint8_t));

            avpicture_fill((AVPicture *)frameRGB, bufor, AV_PIX_FMT_RGB24, codecContext->width, codecContext->height);

            sws_ctx = sws_getContext(
                    codecContext->width,
                    codecContext->height,
                    codecContext->pix_fmt,
                    codecContext->width,
                    codecContext->height,
                    PIX_FMT_RGB24,
                    SWS_BILINEAR,
                    NULL,
                    NULL,
                    NULL
                    );

            i=0;

            while(av_read_frame(formatContext, &packet)>=0)
            {
                if(packet.stream_index==videoStream)
                {
                    avcodec_decode_video2(codecContext, frame, &frameFinished, &packet);

                    if(frameFinished)
                    {
                        cout<<frame->key_frame;
                        sws_scale(
                                sws_ctx, (uint8_t const * const *)frame->data,
                                frame->linesize, 0, codecContext->height,
                                frameRGB->data, frameRGB->linesize
                                );
                        
                        QString tmpText(codecContext->codec_name);
                        qDebug()<<codecContext->width;
                        emit sendVideoParams(codecContext->width, codecContext->height, tmpText);
                        
                        break;
                    }

                }

                av_packet_unref(&packet);
            }

        }

}



/**
 * @brief VideoWorker::stopVideo
 * Metoda wywoływana podczas zamykania wątku - zwalniane są wszystkie zasoby
 */
void VideoWorker::stopVideo()
{

    av_frame_free(&frameRGB);
    av_frame_free(&frame);

    avcodec_close(codecContext);
    avcodec_close(codecContextOriginal);

    avformat_close_input(&formatContext);
    avformat_network_deinit();
}
