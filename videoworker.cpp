#include "videoworker.h"

VideoWorker::VideoWorker()
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(grabFrameFn()), Qt::DirectConnection);
}

VideoWorker::~VideoWorker()
{

}

void VideoWorker::grabFrameFn()
{
    int len;
    int codecError = 0;
    

//    if(av_read_frame(formatContext, &packet)<0)

    while(av_read_frame(formatContext, &packet)>=0)
    {
        if (packet.stream_index == videoStream)
        {
            len = avcodec_decode_video2(codecContext, frame, &frameFinished, &packet);

            if(frameFinished)
            {
                sws_scale(
                        sws_ctx, (uint8_t const * const *)frame->data,
                        frame->linesize, 0, codecContext->height,
                        frameRGB->data, frameRGB->linesize
                        );

                QString tmpText(codecContext->codec->name);
           emit sendVideoParams(codecContext->width, codecContext->height, tmpText);
                break;

            }


        }

        av_packet_unref(&packet);
    }
//    while(av_read_frame(formatContext, &packet)>=0)
//            {
//        qDebug()<<"TEST!@#";
//                if(packet.stream_index==videoStream)
//                {
//                    qDebug()<<"test123123";
//                    avcodec_decode_video2(codecContext, frame, &frameFinished, &packet);

//                    if(frameFinished)
//                    {
//                        qDebug()<<"koniec ramki";
//                        cout<<frame->key_frame;
//                        sws_scale(
//                                sws_ctx, (uint8_t const * const *)frame->data,
//                                frame->linesize, 0, codecContext->height,
//                                frameRGB->data, frameRGB->linesize
//                                );

//                        QString tmpText(codecContext->codec_name);
//                        emit sendVideoParams(codecContext->width, codecContext->height, tmpText);

//                        break;
//                    }

//                }

//                av_packet_unref(&packet);
//            }
}

void VideoWorker::processVideo(const QString url)
{
    int len;
    // qDebug()<<"jedziemy";
//    grabFrame = true;
    int codecError = 0;
    // qDebug()<<"rtesda";
//    connect(timer, SIGNAL(timeout()), this, SLOT(grabFrame()), Qt::DirectConnection);
    
        avformat_network_init();
        av_register_all();      //rejestrowanie wszystkich dostępnych formatów i kodeków w celu automatycznego wiązania ich ze strumieniem wideo

        if(avformat_open_input(&formatContext, url.toUtf8().data(), NULL, NULL)!=0)
        {
            // qDebug()<<"Błąd otwierania strumienia"<<endl;
//            emit returnError(ErrorEnums::CREDENTIALS_ERROR);
        }
        else
        {
            if(avformat_find_stream_info(formatContext, NULL)<0)
            {
                // qDebug()<<"Nie można znaleźć informacji o strumieniu";
    //            return -1;
            }

            
            //WYŚWIETLENIE WSZYSTKIEGO W STRUMIENIU BŁĘDÓW
            
            av_dump_format(formatContext, 0, url.toUtf8().data(), 0);

            videoStream = -1;

            videoStream = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);


            if(videoStream==-1)
            {
                // qDebug()<<"bład strumienia 127";
//                return -1;      //nie znaleziono strumienia wideo
            }

            codecContextOriginal = formatContext->streams[videoStream]->codec;

            codec = avcodec_find_decoder(codecContextOriginal->codec_id);
            if(codec==NULL)
            {

                // qDebug()<<"Nie wspierany kodek";
                stopVideo();
    //            return -1;
            }

            codecContext = avcodec_alloc_context3(codec);
            if(avcodec_copy_context(codecContext, codecContextOriginal)!=0)
            {
                // qDebug()<<"Nie można skopiować CodecContext";
    //            return -1;
            }

            if(avcodec_open2(codecContext, codec, NULL)<0)
            {
                // qDebug()<<"Nie można otworzyć kodeka";
    //            return -1;
            }

            frame = av_frame_alloc();
            frameRGB = av_frame_alloc();

            if(frameRGB==NULL){
                // qDebug()<<"błąd ramki RGB";
    //            return -1;
            }
avformat_network_deinit();
            iloscBajtow = avpicture_get_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height);

            bufor = (uint8_t *)av_malloc(iloscBajtow*sizeof(uint8_t));

            avpicture_fill((AVPicture *)frameRGB, bufor, AV_PIX_FMT_RGB24, codecContext->width, codecContext->height);

            sws_ctx = sws_getContext(
                    codecContext->width,
                    codecContext->height,
                    codecContext->pix_fmt,
                    codecContext->width,
                    codecContext->height,
                    AV_PIX_FMT_RGB24,
                    SWS_BILINEAR,
                    NULL,
                    NULL,
                    NULL
                    );

            i=0;
            
            timer->start(5);      //co sekundę będą gromadzone informacje ze strumienia

//            while(1)
//            {
//                QApplication::processEvents();
//                if(grabFrame==false) break;
//                qDebug()<<av_read_frame(formatContext, &packet);
//                while(av_read_frame(formatContext, &packet)>=0)
//                {
//                    if(packet.stream_index==videoStream)
//                    {
//                        len = avcodec_decode_video2(codecContext, frame, &frameFinished, &packet);

//                        if(len < 0)
//                        {
//                            qDebug()<<"Błąd kodeka";
//                            codecError++;
//                        }

//                        if(frameFinished)
//                        {
//                            cout<<frame->key_frame;
//                            sws_scale(
//                                    sws_ctx, (uint8_t const * const *)frame->data,
//                                    frame->linesize, 0, codecContext->height,
//                                    frameRGB->data, frameRGB->linesize
//                                    );

//                            qDebug()<<codecContext->codec->name;
//                            QString tmpText(codecContext->codec->name);
//                            emit sendVideoParams(codecContext->width, codecContext->height, tmpText);
////                            qDebug()<<"Ramka";
////                            if(codecError!=0){
////                                qDebug()<<"był błąd";
////                            }
//                            break;
//                        }

//                    }

//                    av_packet_unref(&packet);
//                }
//            }

        }

}

/**
 * @brief VideoWorker::stopVideo
 * Metoda wywoływana podczas zamykania wątku - zwalniane są wszystkie zasoby
 */
void VideoWorker::stopVideo()
{
    timer->stop();
//    grabFrame = false;
    av_frame_free(&frameRGB);
    av_frame_free(&frame);

    avcodec_close(codecContext);
    avcodec_close(codecContextOriginal);

    avformat_close_input(&formatContext);
    avformat_network_deinit();
}

ErrorEnums VideoWorker::checkCredentials(QString url)
{

    static AVFormatContext *formatContextTest = NULL;
    avformat_network_init();
    av_register_all();
    if(avformat_open_input(&formatContextTest, url.toUtf8().data(), NULL, NULL)!=0)
    {
        cout<<"Błąd otwierania strumienia"<<endl;
        avformat_network_deinit();
        return ErrorEnums::CREDENTIALS_ERROR;
    }
    else
    {
        avformat_close_input(&formatContextTest);
        avformat_network_deinit();
        return ErrorEnums::NO_ERROR;
    }
}
