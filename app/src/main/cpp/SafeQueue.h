//
// Created by wangrenxing on 2019-12-24.
//

#ifndef NEWBIEFFMPEG_SAFEQUEUE_H
#define NEWBIEFFMPEG_SAFEQUEUE_H

#include <pthread.h>
#include <queue>
using namespace std;

template <typename T>
class SafeQueue {
    typedef void (* ReleaseHandler)(T** value);
    typedef void (* SyncHandler)(queue<T>* q);

private:
    queue<T> q;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond;
    ReleaseHandler releaseHandler = NULL;
    bool working = false;

    SyncHandler syncHandler = NULL;

public:
    SafeQueue() {
        pthread_mutex_init(this->mutex, NULL);
        pthread_cond_init(this->cond, NULL);
    }
    virtual ~SafeQueue() {
        pthread_mutex_destroy(this->mutex);
        pthread_cond_destroy(this->cond);
        int size = q.size();
        for(int i=0; i<size; i++ ) {
            T v = q.front();
            if( releaseHandler && v ) {
                releaseHandler(&v);
            }
        }
    }
    void setWorking(bool working) {
        this->working = working;
    }
    void setReleaseHandler(ReleaseHandler h) {
        this->releaseHandler = h;
    }
    void setSyncHandler(SyncHandler h) {
        this->syncHandler = h;
    }

    void sync();
    void push(T value);
    bool pop(T& value);
};

#endif //NEWBIEFFMPEG_SAFEQUEUE_H
