//
// Created by wangrenxing on 2019-12-24.
//

#ifndef NEWBIEFFMPEG_BASECHANNEL_H
#define NEWBIEFFMPEG_BASECHANNEL_H

#include "common.h"
#include "SafeQueue.h"

extern "C" {
#include <libavcodec/avcodec.h>
};

class BaseChannel {
private:
    int streamIdx = -1;
protected:
    AVCodecContext* codecContext = NULL;
    bool isPlaying = false;
public:
    SafeQueue<AVPacket*> packets;
    SafeQueue<AVFrame*> frames;

public:
    BaseChannel(AVCodecContext* codecContext, int streamIdx) {
        this->codecContext = codecContext;
        this->streamIdx = streamIdx;
        this->packets = SafeQueue<AVPacket*>(releasePktHandler);
        this->frames = SafeQueue<AVFrame*>(releaseFrameHandler);
    }

    void pushPacket(AVPacket* packet) {
        packets.push(packet);
    }

    static void releaseFrameHandler(AVFrame*& frame) {
        av_frame_free(&frame);
    }

    static void releasePktHandler(AVPacket*& pkt) {
        av_packet_free(&pkt);
    }

    virtual ~BaseChannel() {
        if( codecContext ) {
            if( avcodec_is_open(codecContext) ) {
                avcodec_close(codecContext);
            }
            avcodec_free_context(&codecContext);
            codecContext = NULL;
        }
    }

    int getStreamIdx() const {
        return streamIdx;
    }

    /*
     * 切记返回值
     */
    static void* frameFun(void* args) {
        BaseChannel* channel = static_cast<BaseChannel *>(args);
        channel->doFrame();
        return 0;
    }
    /*
     * 切记返回值
     */
    static void* decodeFun(void* args) {
        BaseChannel* channel = static_cast<BaseChannel *>(args);
        channel->doDecode();
        return 0;
    }

    void start() {
        if( !isPlaying ) {
            isPlaying = true;
            pthread_t pt1, pt2;
            pthread_create(&pt1, NULL, decodeFun, this);
            pthread_create(&pt1, NULL, frameFun, this);
        }
    }

    virtual void doDecode() = 0;
    virtual void doFrame() = 0;
};
#endif //NEWBIEFFMPEG_BASECHANNEL_H
