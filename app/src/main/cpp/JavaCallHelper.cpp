//
// Created by wangrenxing on 2019-12-24.
//

#include "JavaCallHelper.h"
#include "common.h"

JavaCallHelper::JavaCallHelper(JavaVM* jvm, JNIEnv* env, jobject instance) {
    LOGD("JavaCallHelper::JavaCallHelper jvm: %#x, env: %#x", jvm, env);
    this->javaVm = jvm;
    this->env = env;
    // 跨线程时需要用全局引用
    this->instance = env->NewGlobalRef(instance);
    jclass clz = env->GetObjectClass(instance);
    this->onErrorId = env->GetMethodID(clz, "onError", "(I)V");
    this->onPreparedId = env->GetMethodID(clz, "onPrepared", "()V");
}

JavaCallHelper::~JavaCallHelper() {
    LOGD("JavaCallHelper::~JavaCallHelper env: %#x", this->env);
    this->env->DeleteGlobalRef(instance);
    LOGD("JavaCallHelper::~JavaCallHelper success", "");
}

void JavaCallHelper::onError(CommonError code, bool isMainThread) {
    if( isMainThread ) {
        env->CallVoidMethod(instance, onErrorId, (int)code);
    } else {
        JNIEnv* tmpEnv;
        javaVm->AttachCurrentThread(&tmpEnv, NULL);
        tmpEnv->CallVoidMethod(instance, onErrorId, (int)code);
        javaVm->DetachCurrentThread();
    }
}

void JavaCallHelper::onPrepared(bool isMainThread) {
    LOGD("JavaCallHelper::onPrepared,isMainThread: %d", isMainThread);
    LOGD("JavaCallHelper::onPrepared,instance: %#x, onPreparedId: %d, javaVm %#x", instance, onPreparedId, javaVm);
    if( isMainThread ) {
        env->CallVoidMethod(instance, onPreparedId);
    } else {
        JNIEnv* tmpEnv;
        if( JNI_OK == javaVm->AttachCurrentThread(&tmpEnv, NULL) ) {
            LOGD("JavaCallHelper::onPrepared,tmpEnv: %#x", tmpEnv);
            tmpEnv->CallVoidMethod(instance, onPreparedId);
            javaVm->DetachCurrentThread();
        } else {
            LOGE("javaVm->AttachCurrentThread fail. %s","");
        }
    }
}