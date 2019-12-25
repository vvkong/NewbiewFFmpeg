//
// Created by wangrenxing on 2019-12-24.
//

#ifndef NEWBIEFFMPEG_VIDEOCHANNEL_H
#define NEWBIEFFMPEG_VIDEOCHANNEL_H

#include "BaseChannel.h"

class VideoChannel: public BaseChannel {

public:
    VideoChannel(AVCodecContext* codecContext, int streamIdx): BaseChannel(codecContext, streamIdx) {

    }
    ~VideoChannel() {
        BaseChannel::~BaseChannel();
    }
};
#endif //NEWBIEFFMPEG_VIDEOCHANNEL_H
