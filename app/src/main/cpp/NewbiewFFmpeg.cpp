//
// Created by wangrenxing on 2019-12-24.
//

#include <pthread.h>
#include "NewbiewFFmpeg.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
}

static void printError(char *msg, int code = 0) {
    LOGE("%s %s", msg, av_err2str(code));
}

NewbiewFFmpeg::NewbiewFFmpeg(const char* dataSource, JavaCallHelper* helper): helper(helper) {
    int len = strlen(dataSource);
    this->dataSource = new char[len+1];
    strcpy(this->dataSource, dataSource);
}

NewbiewFFmpeg::~NewbiewFFmpeg() {
    LOGD("~NewbiewFFmpeg %s", "");
    if( dataSource ) {
        delete []dataSource;
        dataSource = NULL;
    }
    LOGD("~NewbiewFFmpeg helper: %#x, videoChannel: %#x", helper, videoChannel);
    DELETE(helper)
    DELETE(videoChannel)
}


static void* prepareFun(void* args) {
    LOGD(".... in prepareFun %s", "");
    NewbiewFFmpeg* ffmpeg = static_cast<NewbiewFFmpeg *>(args);
    ffmpeg->_prepare();
    return 0;
}

void NewbiewFFmpeg::prepare() {
    pthread_t pt;
    pthread_create(&pt, NULL, prepareFun, this);
    LOGD("NewbiewFFmpeg::prepare... %s", "");
}

void NewbiewFFmpeg::_prepare() {
    LOGD("dataSource: %s", dataSource);

    // 网络初始化
    avformat_network_init();

    int ret;
    // 打开文件
    AVFormatContext* ifmtCtx = NULL;
    if( (ret=avformat_open_input(&ifmtCtx, dataSource, NULL, NULL)) < 0 ) {
        printError("avformat_open_input fail", ret);
        helper->onError(ERR_OPEN_INPUT, false);
        return;
    }
    // 查看媒体信息
    if( (ret=avformat_find_stream_info(ifmtCtx, NULL)) < 0 ) {
        printError("avformat_find_stream_info fail", ret);
        helper->onError(ERR_FIND_STREAM_INFO, false);
        return;
    }
    // 获取音视频流
    for( int i=0; i<ifmtCtx->nb_streams; i++ ) {
        AVStream* stream = ifmtCtx->streams[i];

        AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
        if( !codec ) {
            printError("avcodec_find_decoder fail");
            helper->onError(ERR_FIND_DECODER, false);
            return;
        }
        AVCodecContext* ctx = avcodec_alloc_context3(codec);
        if( !ctx ) {
            printError("avcodec_alloc_context3 fail");
            helper->onError(ERR_CODEC_ALLOC_CONTEXT, false);
            return;
        }
        if( (ret=avcodec_open2(ctx, codec, NULL)) < 0 ) {
            printError("avcodec_open2 fail", ret);
            helper->onError(ERR_CODEC_OPEN, true);
        }

        if( AVMEDIA_TYPE_VIDEO == stream->codecpar->codec_type ) {
            videoChannel = new VideoChannel(ctx, i);
        } else if( AVMEDIA_TYPE_AUDIO == stream->codecpar->codec_type ) {
        } else {
        }
    }

    if( (!videoChannel) && (!audioChannel) ) {
        printError("couldn't find video and audio.");
        helper->onError(ERR_NOT_FIND_VIDEO_AND_AUDIO, false);
    }

    helper->onPrepared(false);
}