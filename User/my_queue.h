#ifndef _MY_QUEUE_H
#define _MY_QUEUE_H

#ifdef __cplusplus
extern "C"
{
#endif

struct queue_s {
    char *queue_buf;
    int front, rear;
    int queue_max_element;
};
extern struct queue_s rxd_queue;

#define QUEUE_NORMAL 0
#define QUEUE_FULL   1
#define QUEUE_EMPTY  2

extern void init_queue(struct queue_s *queue_name);
extern char in_queue(struct queue_s *queue_name, char in_data);
extern char out_queue(struct queue_s *queue_name, char *out_data);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
