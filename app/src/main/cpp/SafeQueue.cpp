//
// Created by wangrenxing on 2019-12-24.
//

#include "SafeQueue.h"

using namespace std;

template <class T>
void SafeQueue<T>::push(T value) {
    pthread_mutex_lock(mutex);
    if( working ) {
        q.push(value);
        pthread_cond_signal(cond);
    }
    pthread_mutex_unlock(mutex);
}

template <class T>
bool SafeQueue<T>::pop(T& value) {
    bool ret = false;
    pthread_mutex_lock(mutex);
    while( working && q.empty() ) {
        pthread_cond_wait(cond, mutex);
    }
    value = this->q.front();
    q.pop();
    pthread_mutex_unlock(mutex);
    return ret;
}

template <class T>
void SafeQueue<T>::sync() {
    pthread_mutex_lock(this->mutex);
    if( syncHandler ) {
        syncHandler(&q);
    }
    pthread_mutex_unlock(this->mutex);
}