package com.godot.newbieffmpeg;

import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.Keep;

/**
 * @Desc 说明
 * @Author Godot
 * @Date 2019-12-24
 * @Version 1.0
 * @Mail 809373383@qq.com
 */
public class NewbiePlayer implements SurfaceHolder.Callback {

    private Listener listener;
    private SurfaceHolder holder;
    private String dataSource;

    static {
        /*
        System.loadLibrary("avformat-58");
        System.loadLibrary("avcodec-58");
        System.loadLibrary("swscale-5");
        System.loadLibrary("avutil-56");
        System.loadLibrary("swresample-3");
        */
        System.loadLibrary("newbieplayer");
    }

    public void setListener(Listener listener) {
        this.listener = listener;
    }

    public void setDataSource(String dataSource) {
        this.dataSource = dataSource;
    }

    public void setSurfaceView(SurfaceView surfaceView) {
        holder = surfaceView.getHolder();
        holder.addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        nativeSetSurface(holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
    }

    public void prepare() {
        Log.d("bad-boy", "prepare....");
        nativePrepare(dataSource);
    }

    /**
     * 此2方法供native回调上来
     */
    @Keep
    public void onPrepared() {
        if( listener != null ) {
            listener.onPrepared();
        }
    }
    @Keep
    public void onError(int code) {
        if( listener != null ) {
            listener.onError(code);
        }
    }

    public void start() {
        nativeStart();
    }

    public void stop() {

    }

    public void release() {
        if( holder != null ) {
            holder.removeCallback(this);
        }
        nativeRelease();
    }

    private native void nativeSetSurface(Surface surface);
    public native int nativePrepare(String dataSource);
    public native int nativeStart();
    public native int nativeStop();
    // 确保在主线程调用哦
    public native int nativeRelease();


    public interface Listener {
        void onPrepared();
        void onError(int errCode);
    }
}
