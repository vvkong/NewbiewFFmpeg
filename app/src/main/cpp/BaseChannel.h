//
// Created by wangrenxing on 2019-12-24.
//

#ifndef NEWBIEFFMPEG_BASECHANNEL_H
#define NEWBIEFFMPEG_BASECHANNEL_H

#include "common.h"

extern "C" {
#include <libavcodec/avcodec.h>
};
class BaseChannel {
private:
    AVCodecContext* codecContext = NULL;
    int streamIdx = -1;
public:
    BaseChannel(AVCodecContext* codecContext, int streamIdx) {
        this->codecContext = codecContext;
        this->streamIdx = streamIdx;
    }

    virtual ~BaseChannel() {
        LOGD("BaseChannel:: ~BaseChannel, codecContext: %#x", codecContext);
        if( codecContext ) {
            if( avcodec_is_open(codecContext) ) {
                avcodec_close(codecContext);
            }
            avcodec_free_context(&codecContext);
            codecContext = NULL;
        }
        LOGD("BaseChannel:: ~BaseChannel success", "");
    }
};
#endif //NEWBIEFFMPEG_BASECHANNEL_H
