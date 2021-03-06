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

    while(av_read_frame(formatContext, &packet)==0)
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
}

void VideoWorker::processVideo(const QString url)
{
    int len;
    
        avformat_network_init();
        av_register_all();      //rejestrowanie wszystkich dostępnych formatów i kodeków w celu automatycznego wiązania ich ze strumieniem wideo

        if(avformat_open_input(&formatContext, url.toUtf8().data(), NULL, NULL)!=0)
        {
            std::cout<<"Błąd otwierania strumienia"<<std::endl;
            exit(1);
        }
        else
        {
            if(avformat_find_stream_info(formatContext, NULL)<0)
            {
                std::cout<<"Nie można znaleźć informacji o strumieniu"<<std::endl;
                exit(1);
            }

            av_dump_format(formatContext, 0, url.toUtf8().data(), 0);

            videoStream = -1;

            videoStream = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);


            if(videoStream==-1)
            {
                std::cout<<"Błąd strumienia"<<std::endl;
                exit(1);
            }

            codecContextOriginal = formatContext->streams[videoStream]->codec;

            codec = avcodec_find_decoder(codecContextOriginal->codec_id);
            if(codec==NULL)
            {
                std::cout<<"Nie wspierany kodek"<<std::endl;
                stopVideo();
                exit(1);
            }

            codecContext = avcodec_alloc_context3(codec);
            if(avcodec_copy_context(codecContext, codecContextOriginal)!=0)
            {
                std::cout<<"Błąd kopiowania codecContext"<<std::endl;
                exit(1);
            }

            if(avcodec_open2(codecContext, codec, NULL)<0)
            {
                std::cout<<"Nie można otworzyć kodeka"<<std::endl;
                exit(1);
            }

            frame = av_frame_alloc();
            frameRGB = av_frame_alloc();

            if(frameRGB==NULL){
                std::cout<<"Błąd alokacji ramki wideo"<<std::endl;
                exit(1);
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

            timer->start(100);
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
