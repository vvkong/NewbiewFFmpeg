//
// Created by wangrenxing on 2019-12-24.
//

#ifndef NEWBIEFFMPEG_VIDEOCHANNEL_H
#define NEWBIEFFMPEG_VIDEOCHANNEL_H

#include "BaseChannel.h"
#include "AudioChannel.h"

extern "C" {
#include "libswscale/swscale.h"
};
class VideoChannel: public BaseChannel {

public:
    typedef void (*RenderFunction)(uint8_t* data, int linesize, int w, int h);
private:
    SwsContext* swsContext = NULL;
    RenderFunction renderFunction = NULL;
    AudioChannel* audioChannel = NULL;
    double fps;
    static void dropFrame(queue<AVFrame*>* frames) {
        if( frames ) {
            if(!frames->empty()) {
                AVFrame* frame = frames->front();
                if( frame ) {
                    frames->pop();
                    av_frame_free(&frame);
                }
            }
        }
    }
public:
    VideoChannel(AVCodecContext* codecContext, int streamIdx, AVRational timeBase, double fps): BaseChannel(codecContext, streamIdx, timeBase) {
        this->fps = fps;
        swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
                codecContext->width, codecContext->height, AV_PIX_FMT_RGBA,
                SWS_BILINEAR, NULL, NULL, NULL);
        frames.setSyncHandler(dropFrame);
    }
    ~VideoChannel() {
        if( swsContext ) {
            sws_freeContext(swsContext);
        }
        LOGD(" ~VideoChannel %s", "");
    }
    void setAudioChannel(AudioChannel* audioChannel) {
        this->audioChannel = audioChannel;
    }
    void setRenderFunction(void (*renderFunction)(uint8_t *, int, int, int));

protected:
    void doFrame();
};
#endif //NEWBIEFFMPEG_VIDEOCHANNEL_H
