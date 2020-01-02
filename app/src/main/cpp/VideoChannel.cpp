//
// Created by wangrenxing on 2019-12-24.
//
#include "VideoChannel.h"
#include <unistd.h>
extern "C" {
#include "libavutil/imgutils.h"
#include <libavutil/time.h>
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
            // 注意fps、timeBase直接取流里面的值
            double fixDelay = 1 / fps;
            double extraDelay = frame->repeat_pict / (2 * fps);
            double delay = fixDelay + extraDelay;

            double clock = frame->best_effort_timestamp * av_q2d(timeBase);

            LOGD("======= 等待, audio clock: %f, video clock: %f", audioChannel->clock, clock);

            if( clock > 0 && audioChannel ) {
                if( audioChannel->clock > clock ) {
                    // 丢帧加速前进
                    if( fabs(audioChannel->clock - clock) > 0.05 ) {
                        LOGD("======= 丢帧, %s", "");
                        av_frame_free(&frame);
                        frames.sync();
                        continue;
                    } else {
                        delay = 0;
                    }
                } else {
                    delay += (clock - audioChannel->clock);
                    LOGD("======= 等待, delay: %lf", delay);
                }
            }
            if( delay > 0 ) {
                av_usleep(delay*1000000);
            }

            sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, dstData, dstLinesize);
            if( renderFunction ) {
                renderFunction(dstData[0], dstLinesize[0], dstW, dstH);
            }
            av_frame_free(&frame);
        }
    }
    av_free(dstData[0]);
}

void VideoChannel::setRenderFunction(void (*renderFunction)(uint8_t *, int, int, int)) {
    VideoChannel::renderFunction = renderFunction;
}
