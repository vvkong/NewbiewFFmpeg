#include <jni.h>
#include <string>
#include <pthread.h>
#include "NewbiewFFmpeg.h"
#include "common.h"
#include "JavaCallHelper.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>

static ANativeWindow* g_window = NULL;
static NewbiewFFmpeg* g_ffmpeg = NULL;
static JavaVM* g_javaVm;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

int JNI_OnLoad(JavaVM* javaVm, void* reserved) {
    LOGD("JniOnLoad..... %s","");
    g_javaVm = javaVm;
    return JNI_VERSION_1_6;
}

static void releaseWindow() {
    pthread_mutex_lock(&g_mutex);
    if( g_window ) {
        ANativeWindow_release(g_window);
        g_window = NULL;
    }
    pthread_mutex_unlock(&g_mutex);
}

static void RenderFunction(uint8_t* data, int linesize, int w, int h) {
    LOGD("RenderFunction...... w: %d, h: %d", w, h);
    pthread_mutex_lock(&g_mutex);
    ANativeWindow_Buffer buffer;
    if( g_window ) {
        int ww = ANativeWindow_getWidth(g_window);
        int wh = ANativeWindow_getHeight(g_window);
        int aw, ah;
        if( ww * h > wh * w ) {
            ah = h;
            aw = ah * ww / wh;
        } else {
            aw = w;
            ah = aw * wh / ww;
        }

        if( 0 == ANativeWindow_setBuffersGeometry(g_window, aw, ah, AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM) ) {
            LOGD("RenderFunction. cccc..... w: %d, h: %d", w, h);
            ANativeWindow_lock(g_window, &buffer, NULL);
            uint8_t* dst = static_cast<uint8_t *>(buffer.bits);
            int dstSize = buffer.stride * 4;
            uint8_t* src = data;
            int srcSize = linesize;
            for( int i=0; i<h; i++ ) {
                memcpy(dst, src, srcSize);
                dst += dstSize;
                src += srcSize;
            }
            ANativeWindow_unlockAndPost(g_window);
        } else {
            LOGE("ANativeWindow_setBuffersGeometry fail.", "");
        }
    }
    pthread_mutex_unlock(&g_mutex);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_godot_newbieffmpeg_NewbiePlayer_nativePrepare(JNIEnv* env, jobject thiz, jstring uri) {
    jboolean isCopy = JNI_FALSE;
    const char* url = env->GetStringUTFChars(uri, &isCopy);
    JavaCallHelper* helper = new JavaCallHelper(g_javaVm, env, thiz);
    g_ffmpeg = new NewbiewFFmpeg(url, helper);
    g_ffmpeg->setRenderFunction(RenderFunction);
    g_ffmpeg->prepare();
    env->ReleaseStringUTFChars(uri, url);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_godot_newbieffmpeg_NewbiePlayer_nativeSetSurface(JNIEnv *env, jobject thiz,
                                                          jobject surface) {
    releaseWindow();
    pthread_mutex_lock(&g_mutex);
    g_window = ANativeWindow_fromSurface(env, surface);
    pthread_mutex_unlock(&g_mutex);
}

extern "C"
JNIEXPORT jboolean JNICALL Java_com_godot_newbieffmpeg_NewbiePlayer_nativeStart(JNIEnv* env, jobject thiz) {
    jboolean ret = JNI_TRUE;
    if( g_ffmpeg ) {
        g_ffmpeg->start();
    } else {
        ret = JNI_FALSE;
    }
    return ret;
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
    releaseWindow();
    return JNI_TRUE;
}