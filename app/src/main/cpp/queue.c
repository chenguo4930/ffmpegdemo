//
// Created by cheng on 2017/3/23.
//

#include "queue.h"

/**
 * 队列，这里主要用于存放AVPacket的指针
 * 这里，使用生产者消费者模式来使用队列，至少需要2个队列实例，分别用来存储音频AVPacke和视频AVPacket
 * 1. 生产者：read_stream线程负责不断的读取视频文件中AVPacket，分别放入两个队列中
 * 2. 消费者：
 *      1） 视频解码，从视频AVPacket Queue中获取元素，解码，绘制
 *      2） 音频解码，从音频AVPacket Queue中获取元素，解码，播放
 */
struct _Queue {
    //长度
    int size;
    //任意类型的指针数组，这里每一个元素都是AVPacket指针，总共有size个
    //AVPacket **packets;
    void **tab;

    //push或则pop元素时需要按照先后顺序，依次进行
    int next_to_write;
    int next_to_read;

    int *ready;
};

/**
 * 初始化队列
 * @param size
 * @param fill_func
 * @return
 */
Queue *queue_init(int size, queue_fill_func fill_func) {
    Queue *queue = (Queue *) malloc(sizeof(Queue));
    queue->size = size;
    queue->next_to_write = 0;
    queue->next_to_read = 0;
    //数组开辟空间
    queue->tab = malloc(sizeof(*queue->tab) * size);
    for (int i = 0; i < size; ++i) {
        queue->tab[i] = fill_func();
    }
    return queue;
}

/**
 * 销毁队列
 * @param queue
 * @param free_func
 */
void queue_free(Queue *queue, queue_free_func free_func) {
    for (int i = 0; i < queue->size; ++i) {
        free_func((void *) queue->tab[i]);
    }
    free(queue->tab);
    free(queue);
}

/**
 * 获取下一个索引位置
 * @param queue
 * @param current
 * @return
 */
int queue_get_next(Queue *queue, int current) {
    return (current + 1) % queue->size;
}

/**
 * 队列压入元素
 * @param queue
 * @return
 */
void *queue_push(Queue *queue){
    int current = queue->next_to_write;
    queue->next_to_write = queue_get_next(queue,current);
    LOGE("-------queue_push queue:%#x,%d",queue,current);
    return queue->tab[current];
}

/**
 * 弹出元素
 * @param queue
 * @return
 */
void *queue_pop(Queue *queue){
    int current =queue->next_to_read;
    queue->next_to_read = queue_get_next(queue,current);
    LOGI("------queue_pop queue:%#x, %d",queue,current);
    return queue->tab[current];
}