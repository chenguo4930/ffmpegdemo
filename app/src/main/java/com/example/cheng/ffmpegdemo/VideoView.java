/*
 * VideoView    2017-03-12
 * Copyright(c) 2017 Chengguo Co.Ltd. All right reserved.
 *
 */
package com.example.cheng.ffmpegdemo;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * 视频播放界面，视频绘制的画布
 *
 * @author cheng
 * @version 1.0.0
 * @since 2017-03-12
 */
public class VideoView extends SurfaceView{

    private MyVideoPlayer player;

    public VideoView(Context context) {
        super(context);
        init();
    }

    public VideoView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public VideoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init(){
        SurfaceHolder holder = getHolder();
        holder.setFormat(PixelFormat.RGBA_8888);
    }

    public void setPlayer(MyVideoPlayer player) {
        this.player = player;
    }
}

























