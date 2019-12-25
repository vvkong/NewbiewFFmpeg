//
// Created by wangrenxing on 2019-12-24.
//

#ifndef NEWBIEFFMPEG_JAVACALLHELPER_H
#define NEWBIEFFMPEG_JAVACALLHELPER_H
#include <jni.h>
#include "CommonError.h"
#include "common.h"

class JavaCallHelper {
private:
    JavaVM* javaVm = NULL;
    JNIEnv* env = NULL;
    jobject instance;
    jmethodID onErrorId;
    jmethodID onPreparedId;

public:
    JavaCallHelper(JavaVM* jvm, JNIEnv* env, jobject instance);
    ~JavaCallHelper();
    void onError(CommonError code, bool isMainThread);
    void onPrepared(bool isMainThread);

};
#endif //NEWBIEFFMPEG_JAVACALLHELPER_H
