/*
 * MyVideoPlayer    2017-03-12
 * Copyright(c) 2017 Chengguo Co.Ltd. All right reserved.
 *
 */
package com.example.cheng.ffmpegdemo;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;
import android.view.Surface;

/**
 * 视频播放的播放器
 *
 * @author cheng
 * @version 1.0.0
 * @since 2017-03-12
 */
public class MyVideoPlayer {

    //绘制
    public native void render(String input, Surface surface);

    public native void sound(String input, String output);

    public native void play(String input,Surface surface);


    /**
     * 创建一个AudioTrack对象，用于播放
     *
     * @return
     */
    public AudioTrack createAudioTrack(int sampleRateInHz, int nb_channels) {

        //固定格式的音频码流
         int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
        Log.e("seven", "----nb_channerls = " + nb_channels);
        //声道布局 , 立体声，至少双声道，单声道可能有问题，单声道很少见
        int channelConfig  = AudioFormat.CHANNEL_IN_STEREO;
        if (nb_channels == 1){
            channelConfig = AudioFormat.CHANNEL_OUT_MONO;
        }

        int bufferSizeInBytes = AudioTrack.getMinBufferSize(
                sampleRateInHz,
                channelConfig,
                audioFormat);

        AudioTrack audioTrack = new AudioTrack(
                AudioManager.STREAM_MUSIC,
                sampleRateInHz,
                channelConfig,
                audioFormat,
                bufferSizeInBytes,
                AudioTrack.MODE_STREAM);

//        //播放
//        audioTrack.play();
//        //写入PCM
//        audioTrack.write();

        return audioTrack;
    }

    static {
        System.loadLibrary("avutil-54");
        System.loadLibrary("swresample-1");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avformat-56");
        System.loadLibrary("swscale-3");
        System.loadLibrary("postproc-53");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("native-lib");
    }
}