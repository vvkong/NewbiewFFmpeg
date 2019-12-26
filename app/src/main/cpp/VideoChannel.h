//
// Created by wangrenxing on 2019-12-24.
//

#ifndef NEWBIEFFMPEG_VIDEOCHANNEL_H
#define NEWBIEFFMPEG_VIDEOCHANNEL_H

#include "BaseChannel.h"
extern "C" {
#include "libswscale/swscale.h"
};
class VideoChannel: public BaseChannel {

public:
    typedef void (*RenderFunction)(uint8_t* data, int linesize, int w, int h);
private:
    SwsContext* swsContext = NULL;
    RenderFunction renderFunction = NULL;

public:
    VideoChannel(AVCodecContext* codecContext, int streamIdx): BaseChannel(codecContext, streamIdx) {
        swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
                codecContext->width, codecContext->height, AV_PIX_FMT_RGBA,
                SWS_BILINEAR, NULL, NULL, NULL);
    }
    ~VideoChannel() {
        if( swsContext ) {
            sws_freeContext(swsContext);
        }
        LOGD(" ~VideoChannel %s", "");
    }

    void setRenderFunction(void (*renderFunction)(uint8_t *, int, int, int));

protected:
    void doFrame();
};
#endif //NEWBIEFFMPEG_VIDEOCHANNEL_H
