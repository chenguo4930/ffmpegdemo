//
// Created by cheng on 2017/3/23.
//
#include <malloc.h>
#include <android/log.h>
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"jason",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"jason",FORMAT,##__VA_ARGS__);

#ifndef FFMPEGDEMO_QUEUE_H
#define FFMPEGDEMO_QUEUE_H

typedef struct _Queue Queue;

//分配队列元素内存的函数
typedef void *(*queue_fill_func)();

//释放队列中元素所占用的内存
typedef void *(*queue_free_func)(void *elem);

/**
 * 初始化队列
 * @param size
 * @param fill_func
 * @return
 */
Queue *queue_init(int size, queue_fill_func fill_func);

/**
 * 销毁队列
 * @param queue
 * @param free_func
 */
void queue_free(Queue *queue, queue_free_func free_func);

/**
 * 获取下一个索引位置
 * @param queue
 * @param current
 * @return
 */
int queue_get_next(Queue *queue, int current);

/**
 * 队列压入元素
 * @param queue
 * @return
 */
void *queue_push(Queue *queue);

/**
 * 弹出元素
 * @param queue
 * @return
 */
void *queue_pop(Queue *queue);

#endif //FFMPEGDEMO_QUEUE_H
