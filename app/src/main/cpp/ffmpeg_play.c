//
// Created by cheng on 2017/3/11.
//
#include "com_example_cheng_ffmpegdemo_MyVideoPlayer.h"
#include <android/log.h>
#include <unistd.h>

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"seven",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"seven",FORMAT,##__VA_ARGS__);

//编码
#include "ffmpeg/libavcodec/avcodec.h"
//封装格式处理
#include "ffmpeg/libavformat/avformat.h"
//像素处理 缩放
#include "ffmpeg/libswscale/swscale.h"

//C++的代码也要用C来编译
#include "libyuv.h";

#include <android/native_window.h>
#include <android/native_window_jni.h>

JNIEXPORT void JNICALL Java_com_example_cheng_ffmpegdemo_VideoUtils_decode
        (JNIEnv *env, jclass jcls, jstring input_str, jstring output_str) {
    AVFormatContext *pFormatCtx;
    const char *input = (*env)->GetStringUTFChars(env, input_str, NULL);
    const char *output = (*env)->GetStringUTFChars(env, output_str, NULL);

    //1.注册组件
    av_register_all();

    //封装格式上下文
    pFormatCtx = avformat_alloc_context();

    //2.打开输入视频文件
    if (avformat_open_input(&pFormatCtx, input, NULL, NULL) != 0) {
        LOGE("Couldn't open input stream.\n");
        return;
    }
    //3.获取视频信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("%s", "获取视频信息失败");
        return;
    }

    //视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
    int video_stream_idx = -1;
    int i = 0;
    for (; i < pFormatCtx->nb_streams; i++) {
        //根据类型判断，是否是视频流
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

    //4.获取视频解码器
    AVCodecContext *pCodeCtx = pFormatCtx->streams[video_stream_idx]->codec;
    AVCodec *pCodec = avcodec_find_decoder(pCodeCtx->codec_id);
    if (pCodec == NULL) {
        LOGE("%s", "无法解码");
        return;
    }

    //5.打开解码器
    if (avcodec_open2(pCodeCtx, pCodec, NULL) < 0) {
        LOGE("%s", "解码器无法打开");
        return;
    }

    //编码数据
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    //像素数据（解码数据）
    AVFrame *frame = av_frame_alloc();
    AVFrame *yuvFrame = av_frame_alloc();

    //只有指定了AVFrame的像素格式、画面大小才能真正分配内存
    //缓冲区分配内存
    uint8_t *out_buffer = (uint8_t *) av_malloc(
            avpicture_get_size(AV_PIX_FMT_YUV420P, pCodeCtx->width, pCodeCtx->height));
    //设置yuvframe的属性（像素格式、宽高、像素格式）
    avpicture_fill((AVPicture *) yuvFrame,
                   out_buffer,
                   AV_PIX_FMT_YUV420P,
                   pCodeCtx->width,
                   pCodeCtx->height);


    //输出文件
    FILE *fp_yuv = fopen(output, "wb");

    //用于像素格式转换或者缩放
    struct SwsContext *sws_ctx = sws_getContext(
            pCodeCtx->width, pCodeCtx->height, pCodeCtx->pix_fmt,
            pCodeCtx->width, pCodeCtx->height, AV_PIX_FMT_YUV420P,
            SWS_BILINEAR, NULL, NULL, NULL);

    int len, got_frame, framecount = 0;
    //6.一阵一阵读取压缩的视频数据AVPacket
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        //解码AVPacket->AVFrame
        len = avcodec_decode_video2(pCodeCtx, frame, &got_frame, packet);

        //Zero if no frame could be decompressed
        //非零，正在解码
        if (got_frame) {
            //frame->yuvFrame (YUV420P)
            //转为指定的YUV420P像素帧
            sws_scale(sws_ctx,
                      frame->data, frame->linesize, 0, frame->height,
                      yuvFrame->data, yuvFrame->linesize);

            //向YUV文件保存解码之后的帧数据
            //AVFrame->YUV
            //一个像素包含一个Y
            int y_size = pCodeCtx->width * pCodeCtx->height;
            fwrite(yuvFrame->data[0], 1, y_size, fp_yuv);
            fwrite(yuvFrame->data[1], 1, y_size / 4, fp_yuv);
            fwrite(yuvFrame->data[2], 1, y_size / 4, fp_yuv);

            LOGI("解码%d帧", framecount++);
        }

        av_free_packet(packet);
    }

    fclose(fp_yuv);

    av_frame_free(&frame);
    avcodec_close(pCodeCtx);
    avformat_free_context(pFormatCtx);


    //释放
    (*env)->ReleaseStringUTFChars(env, input_str, input);
    (*env)->ReleaseStringUTFChars(env, output_str, output);
}

JNIEXPORT void JNICALL
Java_com_example_cheng_ffmpegdemo_MyVideoPlayer_render(JNIEnv *env, jobject instance,
                                                       jstring input_str, jobject surface) {

    const char *input = (*env)->GetStringUTFChars(env, input_str, NULL);

    //1.注册组件
    av_register_all();
    //封装格式上下文
    AVFormatContext *pFormatCtx = avformat_alloc_context();

    //2.打开输入视频文件
    if (avformat_open_input(&pFormatCtx, input, NULL, NULL) != 0) {
        LOGE("Couldn't open input stream.\n");
        return;
    }
    //3.获取视频信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("%s", "--1----获取视频信息失败");
        return;
    }

    //视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
    int video_stream_idx = -1;
    int i = 0;
    for (; i < pFormatCtx->nb_streams; i++) {
        //根据类型判断，是否是视频流
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

    //4.获取视频解码器
    AVCodecContext *pCodeCtx = pFormatCtx->streams[video_stream_idx]->codec;
    AVCodec *pCodec = avcodec_find_decoder(pCodeCtx->codec_id);
    if (pCodec == NULL) {
        LOGE("%s", "---2-----无法解码");
        return;
    }

    //5.打开解码器
    if (avcodec_open2(pCodeCtx, pCodec, NULL) < 0) {
        LOGE("%s", "---3-----解码器无法打开");
        return;
    }

    //编码数据
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    //像素数据（解码数据）
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();

    //native 绘制
    //窗体
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    //绘制时的缓冲区
    ANativeWindow_Buffer outBuffer;

    int len, got_frame, framecount = 0;
    //6.一阵一阵读取压缩的视频数据AVPacket
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        //解码AVPacket->AVFrame
        len = avcodec_decode_video2(pCodeCtx, yuv_frame, &got_frame, packet);

        //Zero if no frame could be decompressed
        //非零，正在解码
        if (got_frame) {
            LOGE("解码%帧", framecount++);
            //lock
            //设置缓存区的属性（宽、高、像素格式）
            ANativeWindow_setBuffersGeometry(nativeWindow,
                                             pCodeCtx->width,
                                             pCodeCtx->height,
                                             WINDOW_FORMAT_RGBA_8888);

            ANativeWindow_lock(nativeWindow, &outBuffer, NULL);


            //设置yuvframe的属性（像素格式、宽高、像素格式）
            //rgb_frame缓冲区与outBuffer.bits是同一块内存
            avpicture_fill((AVPicture *) rgb_frame,
                           outBuffer.bits,
                           PIX_FMT_RGBA,
                           pCodeCtx->width,
                           pCodeCtx->height);

            //YUV->RGBA_8888
            I420ToARGB(yuv_frame->data[0], yuv_frame->linesize[0],
                       yuv_frame->data[2], yuv_frame->linesize[2],
                       yuv_frame->data[1], yuv_frame->linesize[1],
                       rgb_frame->data[0], rgb_frame->linesize[0],
                       pCodeCtx->width, pCodeCtx->height);

            //unlock
            ANativeWindow_unlockAndPost(nativeWindow);

            usleep(1000 * 16);
        }

        av_free_packet(packet);
    }

    ANativeWindow_release(nativeWindow);
    av_frame_free(&yuv_frame);
    avcodec_close(pCodeCtx);
    avformat_free_context(pFormatCtx);

    //释放
    (*env)->ReleaseStringUTFChars(env, input_str, input);
}



/*
JNIEXPORT void JNICALL
Java_com_example_cheng_ffmpegdemo_MyVideoPlayer_render(JNIEnv *env, jobject instance,
                                                       jstring input_str, jobject surface) {


    const char *input_cstr = (*env)->GetStringUTFChars(env, input_str, NULL);

//1.注册组件
    av_register_all();

//封装格式上下文
    AVFormatContext *pFormatCtx = avformat_alloc_context();

//2.打开输入视频文件
    if (avformat_open_input(&pFormatCtx, input_cstr, NULL, NULL) != 0) {
        LOGE("%s", "打开输入视频文件失败");
        return;
    }
//3.获取视频信息
    if (
            avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("%s", "获取视频信息失败");
        return;
    }

//视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
    int video_stream_idx = -1;
    int i = 0;
    for (; i < pFormatCtx->
            nb_streams;
           i++) {
//根据类型判断，是否是视频流
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

//4.获取视频解码器
    AVCodecContext *pCodeCtx = pFormatCtx->streams[video_stream_idx]->codec;
    AVCodec *pCodec = avcodec_find_decoder(pCodeCtx->codec_id);
    if (pCodec == NULL) {
        LOGE("%s", "无法解码");
        return;
    }

//5.打开解码器
    if (
            avcodec_open2(pCodeCtx, pCodec, NULL) < 0) {
        LOGE("%s", "解码器无法打开");
        return;
    }

//编码数据
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));

//像素数据（解码数据）
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();

//native绘制
//窗体
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
//绘制时的缓冲区
    ANativeWindow_Buffer outBuffer;

    int len, got_frame, framecount = 0;
//6.一阵一阵读取压缩的视频数据AVPacket
    while (
            av_read_frame(pFormatCtx, packet
            ) >= 0) {
//解码AVPacket->AVFrame
        len = avcodec_decode_video2(pCodeCtx, yuv_frame, &got_frame, packet);

//Zero if no frame could be decompressed
//非零，正在解码
        if (got_frame) {
            LOGI("解码%d帧", framecount++);
//lock
//设置缓冲区的属性（宽、高、像素格式）
            ANativeWindow_setBuffersGeometry(nativeWindow, pCodeCtx
                    ->width, pCodeCtx->height, WINDOW_FORMAT_RGBA_8888);
            ANativeWindow_lock(nativeWindow,
                               &outBuffer, NULL);

//设置rgb_frame的属性（像素格式、宽高）和缓冲区
//rgb_frame缓冲区与outBuffer.bits是同一块内存
            avpicture_fill((AVPicture
            *) rgb_frame, outBuffer.bits, PIX_FMT_RGBA, pCodeCtx->width, pCodeCtx->height);

//YUV->RGBA_8888
            I420ToARGB(yuv_frame->data[0], yuv_frame->linesize[0],
                       yuv_frame->data[2], yuv_frame->linesize[2],
                       yuv_frame->data[1], yuv_frame->linesize[1],
                       rgb_frame->data[0], rgb_frame->linesize[0],
                       pCodeCtx->width, pCodeCtx->height);

//unlock
            ANativeWindow_unlockAndPost(nativeWindow);

            usleep(1000 * 16);

        }

        av_free_packet(packet);
    }

    ANativeWindow_release(nativeWindow);
    av_frame_free(&yuv_frame);
    avcodec_close(pCodeCtx);
    avformat_free_context(pFormatCtx);

    (*env)->
            ReleaseStringUTFChars(env, input_str, input_cstr
    );
}  */


















