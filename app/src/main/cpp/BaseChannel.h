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
        isPlaying = false;
        packets.setWorking(false);
        frames.setWorking(false);
        if( codecContext ) {
            if( avcodec_is_open(codecContext) ) {
                avcodec_close(codecContext);
            }
            avcodec_free_context(&codecContext);
            codecContext = NULL;
        }
        LOGD(" ~BaseChannel %s", "");
    }

    int getStreamIdx() const {
        return streamIdx;
    }

    virtual void start() {
        if( !isPlaying ) {
            LOGD("%#x, start..... %s", this, "");
            isPlaying = true;
            packets.setWorking(true);
            frames.setWorking(true);
            pthread_t pt1, pt2;
            pthread_create(&pt1, NULL, decodeFun, this);
            pthread_create(&pt1, NULL, frameFun, this);
        }
    }

private:
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
        LOGD("%#x, decodeFun....", args);
        channel->doDecode();
        return 0;
    }

protected:
    virtual void doDecode() {
        int ret;
        AVPacket* pkt;
        AVFrame* frame;
        int decodePacketCount = 0;
        while( isPlaying ) {
            if( packets.pop(pkt) ) {
                LOGD("%#x, Channel::decode....decodePacketCount: %d", this,  ++decodePacketCount);
                ret = avcodec_send_packet(codecContext, pkt);
                if( ret < 0  ) {
                    printError("avcodec_send_packet fail.", ret);
                    av_packet_free(&pkt);
                    continue;
                }
                ret = 1;
                LOGD("after send, %d", ret);
                while( ret ) {
                    frame = av_frame_alloc();
                    ret = avcodec_receive_frame(codecContext, frame);
                    if( AVERROR(EAGAIN) == ret || AVERROR_EOF == ret ) {
                        av_frame_free(&frame);
                        break;
                    } else if( ret < 0 ) {
                        av_frame_free(&frame);
                        printError("avcodec_receive_frame fail.", ret);
                        return;
                    }
                    LOGD("push frame, %d", frames.size());
                    frames.push(frame);
                }
            } else {
            }
        }
    }

    virtual void doFrame() = 0;
};
#endif //NEWBIEFFMPEG_BASECHANNEL_H
