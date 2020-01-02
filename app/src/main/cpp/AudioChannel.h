//
// Created by wangrenxing on 2019-12-24.
//

#ifndef NEWBIEFFMPEG_AUDIOCHANNEL_H
#define NEWBIEFFMPEG_AUDIOCHANNEL_H

#include "BaseChannel.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
extern "C" {
#include "libswresample/swresample.h"
};

class AudioChannel: public BaseChannel {
private:
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;
    SLObjectItf bqPlayerObject = NULL;
    SLPlayItf bqPlayerPlay;
    bool initOpenSLES();
    bool initOpenSLES_2();

    SwrContext* swrContext = NULL;
    const int64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    const AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    const int out_sample_rate = 44100;

public:
    uint8_t* outBuf = NULL;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
    int getPcm();
    double clock;

public:
    AudioChannel(AVCodecContext *codecContext, int streamIdx,AVRational timeBase)
            : BaseChannel(codecContext, streamIdx, timeBase) {
        swrContext = swr_alloc_set_opts(NULL, out_ch_layout, out_sample_fmt, out_sample_rate,
                codecContext->channel_layout, codecContext->sample_fmt, codecContext->sample_rate,
                0, NULL);
        swr_init(swrContext);
        outBuf = static_cast<uint8_t *>(malloc(out_sample_rate * 4));
    }
    ~AudioChannel() {
        LOGD(" ~AudioChannel begin %s", "");
        /** 确保 能释放bqPlayerObject，如还在队列回调等待中，则会阻塞释放 **/
        isPlaying = false;
        frames.push(av_frame_alloc());

        if( outBuf ) {
            free(outBuf);
            outBuf = NULL;
        }
        if( swrContext ) {
            if( swr_is_initialized(swrContext) ) {
                swr_close(swrContext);
            }
            swr_free(&swrContext);
            swrContext = NULL;
        }
        // destroy buffer queue audio player object, and invalidate all associated interfaces
        if (bqPlayerObject != NULL) {
            (*bqPlayerObject)->Destroy(bqPlayerObject);
            LOGD(" ~AudioChannel 444 %s", "");
            bqPlayerObject = NULL;
            bqPlayerPlay = NULL;
            bqPlayerBufferQueue = NULL;
        }
        LOGD(" ~AudioChannel 333 %s", "");
        // destroy output mix object, and invalidate all associated interfaces
        if (outputMixObject != NULL) {
            (*outputMixObject)->Destroy(outputMixObject);
            outputMixObject = NULL;
            outputMixEnvironmentalReverb = NULL;
        }
        LOGD(" ~AudioChannel 444 %s", "");
        // destroy engine object, and invalidate all associated interfaces
        if (engineObject != NULL) {
            (*engineObject)->Destroy(engineObject);
            engineObject = NULL;
            engineEngine = NULL;
        }
        LOGD(" ~AudioChannel end %s", "");
    }

    void doFrame();
};
#endif //NEWBIEFFMPEG_AUDIOCHANNEL_H
