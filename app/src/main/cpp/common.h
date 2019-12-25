//
// Created by wangrenxing on 2019-12-24.
//

#ifndef NEWBIEFFMPEG_COMMON_H
#define NEWBIEFFMPEG_COMMON_H

#include <android/log.h>

#define LOGD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, "bad-boy", fmt, __VA_ARGS__)
#define LOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, "bad-boy", fmt, __VA_ARGS__)

#define DELETE(p) if(p) { delete(p); p = NULL; }

#endif //NEWBIEFFMPEG_COMMON_H
