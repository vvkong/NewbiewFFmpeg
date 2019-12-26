//
// Created by wangrenxing on 2019-12-24.
//
#include "VideoChannel.h"
#include <unistd.h>
extern "C" {
#include "libavutil/imgutils.h"
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
        if( frames.pop(frame) ) {
            if( !isPlaying ) {
                av_frame_free(&frame);
                break;
            }
            sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height,
                    dstData, dstLinesize);
            if( renderFunction ) {
                renderFunction(dstData[0], dstLinesize[0], dstW, dstH);
                usleep(16*1000);
            }
        }
    }

    av_free(dstData[0]);
}

void VideoChannel::setRenderFunction(void (*renderFunction)(uint8_t *, int, int, int)) {
    VideoChannel::renderFunction = renderFunction;
}
