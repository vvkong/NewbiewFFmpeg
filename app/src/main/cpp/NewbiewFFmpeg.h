//
// Created by wangrenxing on 2019-12-24.
//

#ifndef NEWBIEFFMPEG_NEWBIEWFFMPEG_H
#define NEWBIEFFMPEG_NEWBIEWFFMPEG_H

#include <cstring>
#include "common.h"
#include "VideoChannel.h"
#include "JavaCallHelper.h"
#include "AudioChannel.h"

extern "C" {
#include "libavformat/avformat.h"
};
class NewbiewFFmpeg {

private:
    char* dataSource;
    JavaCallHelper* helper;
    AVFormatContext* ifmtCtx = NULL;
    VideoChannel* videoChannel = NULL;
    AudioChannel* audioChannel = NULL;
    VideoChannel::RenderFunction renderFunction = NULL;
    bool isPlaying = false;
public:
    NewbiewFFmpeg(const char* dataSource, JavaCallHelper* helper);
    ~NewbiewFFmpeg();

    void prepare();
    void _prepare();
    void start();
    void _start();

    void setRenderFunction(void (*fun)(uint8_t *, int, int, int));
};
#endif //NEWBIEFFMPEG_NEWBIEWFFMPEG_H
