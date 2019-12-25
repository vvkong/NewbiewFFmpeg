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
    typedef void (* ReleaseHandler)(T& value);
    typedef void (* SyncHandler)(queue<T>* q);

private:
    queue<T> q;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    ReleaseHandler releaseHandler = NULL;
    bool working = true;

    SyncHandler syncHandler = NULL;

public:
    SafeQueue(ReleaseHandler releaseHandler = NULL) {
        this->releaseHandler = releaseHandler;
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
    }
    virtual ~SafeQueue() {
        clear();
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }

    void clear() {
        pthread_mutex_lock(&mutex);
        while( !q.empty() ) {
            T v = q.front();
            q.pop();
            if( releaseHandler && v ) {
                releaseHandler(v);
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    void setWorking(bool working) {
        pthread_mutex_lock(&mutex);
        this->working = working;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    void setReleaseHandler(ReleaseHandler h) {
        this->releaseHandler = h;
    }
    void setSyncHandler(SyncHandler h) {
        this->syncHandler = h;
    }

    int size() {
        return q.size();
    }

    bool isEmpty() {
        return q.empty();
    }
    /**
     * c++模板类申明、实现别分离，gcc不同意
     * @param value
     */
    void push(T value) {
        pthread_mutex_lock(&mutex);
        if( working ) {
            q.push(value);
            pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&mutex);
    }

    /**
     * c++模板类申明、实现别分离，gcc不同意
     * @param value
     * @return
     */
    bool pop(T& value) {
        bool ret = false;
        pthread_mutex_lock(&mutex);
        while( working && q.empty() ) {
            pthread_cond_wait(&cond, &mutex);
        }
        if( !q.empty() ) {
            value = q.front();
            q.pop();
            ret = true;
        }
        pthread_mutex_unlock(&mutex);
        return ret;
    }

    void sync() {
        pthread_mutex_lock(&mutex);
        if( syncHandler ) {
            syncHandler(&q);
        }
        pthread_mutex_unlock(&mutex);
    }
};

#endif //NEWBIEFFMPEG_SAFEQUEUE_H
