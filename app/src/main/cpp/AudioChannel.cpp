//
// Created by wangrenxing on 2019-12-24.
//

#include "AudioChannel.h"


// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    LOGD("bqPlayerCallback.....%s","");
    AudioChannel* audioChannel = static_cast<AudioChannel *>(context);
    int size = audioChannel->getPcm();
    //LOGD("getPcm(): %d", size);
    if( size > 0 ) {
        SLresult result = (*bq)->Enqueue(bq, audioChannel->outBuf, size / 2);
        if (SL_RESULT_SUCCESS != result) {
        }
    }
}
/**
 * 参考ndk-sample中native-audio工程代码实现
 * @return
 */
bool AudioChannel::initOpenSLES() {
    SLresult result;
    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if(SL_RESULT_SUCCESS != result) {
        LOGE("slCreateEngine fail.%s", "");
        return false;
    }
    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS != result) {
        LOGE("(*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE); fail%s", "");
        return false;
    }
    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if(SL_RESULT_SUCCESS != result) {
        LOGE("(*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine) fail.%s", "");
        return false;
    }
    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    if(SL_RESULT_SUCCESS != result) {
        LOGE("(*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req) fail.%s", "");
        return false;
    }
    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS != result) {
        LOGE("(*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE) fail.%s", "");
        return false;
    }
    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("(*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb) fail.%s", "");
        return false;
    }
    result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb, &reverbSettings);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("(*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb, &reverbSettings) fail.%s", "");
        //return false;
    }
    // ignore unsuccessful result codes for environmental reverb, as it is optional for this example

    // create buffer queue audio player
    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    /**
     * SLuint32 		formatType;
        SLuint32 		numChannels;
        SLuint32 		samplesPerSec;
        SLuint32 		bitsPerSample;
        SLuint32 		containerSize;
        SLuint32 		channelMask;
        SLuint32		endianness;
     */
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_44_1,
                                   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_RIGHT|SL_SPEAKER_FRONT_LEFT, SL_BYTEORDER_LITTLEENDIAN};

    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    /*
     * create audio player:
     *     fast audio does not support when SL_IID_EFFECTSEND is required, skip it
     *     for fast audio case
     */
    const SLInterfaceID ids3[2] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,/*SL_IID_MUTESOLO,*/};
    const SLboolean req3[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,/*SL_BOOLEAN_TRUE,*/ };

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk, 2, ids3, req3);
    if(SL_RESULT_SUCCESS != result) {
        LOGE("CreateAudioPlayer fail.%s", "");
        return false;
    }
    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS != result) {
        LOGE("(*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE) fail.%s", "");
        return false;
    }
    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    if(SL_RESULT_SUCCESS != result) {
        LOGE("(*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay) fail.%s", "");
        return false;
    }
    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue);
    if(SL_RESULT_SUCCESS != result) {
        LOGE("(*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue) fail.%s", "");
        return false;
    }
    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, this);
    if(SL_RESULT_SUCCESS != result) {
        LOGE("(*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, this) fail.%s", "");
        return false;
    }

    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    if(SL_RESULT_SUCCESS != result) {
        LOGE("(*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING) fail.%s", "");
        return false;
    }

    bqPlayerCallback(bqPlayerBufferQueue, this);
    return true;
}

int AudioChannel::getPcm() {
    int dataSize = 0;
    if( isPlaying ) {
        AVFrame* frame = NULL;
        // 如何安全退出
        bool ret = frames.pop(frame);
        LOGD("AudioChannel frames.pop(frame) %s", "");
        if( !isPlaying ) {
            av_frame_free(&frame);
            return dataSize;
        }
        if( ret ) {
            int delay = swr_get_delay(swrContext, frame->nb_samples);
            int outCount = av_rescale_rnd(delay+frame->nb_samples, out_sample_rate, frame->sample_rate, AV_ROUND_UP);
            int sample = swr_convert(swrContext, (&outBuf), outCount,
                        const_cast<const uint8_t **>(frame->data), frame->nb_samples);
            dataSize = sample * 2 * 2;
        }
    }
    return dataSize;

}
void AudioChannel::doFrame() {
    if( initOpenSLES() ) {
        LOGD("initOpenSLES success.%s", "");
    } else {
        LOGD("initOpenSLES fail.%s", "");
    }
}


