#ifndef BB_H
#define BB_H

pthread_mutex_t mutex ;
pthread_cond_t cond ;
typedef struct bounded_buffer{
    int buf[10];
    int num;
} bounded_buffer;
bounded_buffer *bb_create();
int bb_get(bounded_buffer* bb);
int bb_put(bounded_buffer* bb,int n);
void bb_destroy(bounded_buffer *bb);
#endif