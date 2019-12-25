//
// Created by wangrenxing on 2019-12-25.
//
#include "common.h"
extern "C" {
#include "libavutil/error.h"
}
void printError(const char *msg, int code) {
    LOGE("%s %s", msg, av_err2str(code));
}
