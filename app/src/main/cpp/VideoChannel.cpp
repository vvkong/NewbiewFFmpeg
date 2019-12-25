//
// Created by wangrenxing on 2019-12-24.
//
#include "VideoChannel.h"
#include <unistd.h>
extern "C" {
#include "libavutil/imgutils.h"
}

void VideoChannel::doDecode() {
    int ret;
    AVPacket* pkt;
    AVFrame* frame;
    int decodePacketCount = 0;
    while( isPlaying ) {
        if( packets.pop(pkt) ) {
            LOGD("VideoChannel::decode....decodePacketCount: %d", ++decodePacketCount);
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

void VideoChannel::doFrame() {
    AVFrame* frame;
    uint8_t  *dstData[4];
    int dstLinesize[4];
    int ret;
    int dstW = codecContext->width;
    int dstH = codecContext->height;

    /* buffer is going to be written to rawvideo file, no alignment */
    if ((ret = av_image_alloc(dstData, dstLinesize, dstW, dstH, AV_PIX_FMT_RGBA, 1)) < 0) {
        printError("Could not allocate destination image", ret);
        return;
    }
    while( isPlaying ) {
        LOGD("VideoChannel::doFrame....%d", frames.size());

        if( frames.pop(frame) ) {
            sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height,
                    dstData, dstLinesize);
            if( renderFunction ) {
                renderFunction(dstData[0], dstLinesize[0], dstW, dstH);
            }
            usleep(16*1000);
        }
    }

    av_free(dstData[0]);
}

void VideoChannel::setRenderFunction(void (*renderFunction)(uint8_t *, int, int, int)) {
    VideoChannel::renderFunction = renderFunction;
}
