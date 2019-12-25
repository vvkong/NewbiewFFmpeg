//
// Created by wangrenxing on 2019-12-24.
//

#ifndef NEWBIEFFMPEG_NEWBIEWFFMPEG_H
#define NEWBIEFFMPEG_NEWBIEWFFMPEG_H

#include <cstring>
#include "common.h"
#include "VideoChannel.h"
#include "JavaCallHelper.h"

class NewbiewFFmpeg {

private:
    char* dataSource;
    JavaCallHelper* helper;
    VideoChannel* videoChannel = NULL;
    VideoChannel* audioChannel = NULL;

public:
    NewbiewFFmpeg(const char* dataSource, JavaCallHelper* helper);
    ~NewbiewFFmpeg();
    void prepare();
    void _prepare();
};
#endif //NEWBIEFFMPEG_NEWBIEWFFMPEG_H
