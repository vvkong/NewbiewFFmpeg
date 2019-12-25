#include <jni.h>
#include <string>
#include <pthread.h>
#include "NewbiewFFmpeg.h"
#include "common.h"
#include "JavaCallHelper.h"

static NewbiewFFmpeg* g_ffmpeg = NULL;
static JavaVM* g_javaVm;

int JNI_OnLoad(JavaVM* javaVm, void* reserved) {
    LOGD("JniOnLoad..... %s","");
    g_javaVm = javaVm;
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_godot_newbieffmpeg_NewbiePlayer_nativePrepare(JNIEnv* env, jobject thiz, jstring uri) {
    jboolean isCopy = JNI_FALSE;
    const char* url = env->GetStringUTFChars(uri, &isCopy);
    JavaCallHelper* helper = new JavaCallHelper(g_javaVm, env, thiz);
    g_ffmpeg = new NewbiewFFmpeg(url, helper);
    g_ffmpeg->prepare();
    env->ReleaseStringUTFChars(uri, url);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_godot_newbieffmpeg_NewbiePlayer_nativeSetSurface(JNIEnv *env, jobject thiz,
                                                          jobject surface) {

}

extern "C"
JNIEXPORT void JNICALL Java_com_godot_newbieffmpeg_NewbiePlayer_nativeStart(JNIEnv* env, jobject thiz) {

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_godot_newbieffmpeg_NewbiePlayer_nativeStop(JNIEnv *env, jobject thiz) {

    return JNI_TRUE;
}

/**
 * 大坑，忘记写返回值，结果是个非法操作，百思不得其解。切记切记
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_godot_newbieffmpeg_NewbiePlayer_nativeRelease(JNIEnv *env, jobject thiz) {
    DELETE(g_ffmpeg);
    return JNI_TRUE;
}