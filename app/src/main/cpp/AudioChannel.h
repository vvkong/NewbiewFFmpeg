//
// Created by wangrenxing on 2019-12-24.
//

#ifndef NEWBIEFFMPEG_AUDIOCHANNEL_H
#define NEWBIEFFMPEG_AUDIOCHANNEL_H

#include "BaseChannel.h"

class AudioChannel: public BaseChannel {
public:
    AudioChannel(AVCodecContext *codecContext, int streamIdx)
            : BaseChannel(codecContext, streamIdx) {

    }
    ~AudioChannel() {
    }
    void doDecode();

    void doFrame();
};
#endif //NEWBIEFFMPEG_AUDIOCHANNEL_H
