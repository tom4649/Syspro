#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "btree.h"

void *fun1(void *tree){
    tnode * t= (tnode *)tree;
    for(int i=1;i<10000;i++)if(i%8==0)btree_insert(i,t);
    pthread_exit(NULL);
}
void *fun2(void *tree){
    tnode * t= (tnode *)tree;
    for(int i=1;i<10000;i++)if(i%8==1)btree_insert(i,t);
    pthread_exit(NULL);
}
void *fun3(void *tree){
    tnode * t= (tnode *)tree;
    for(int i=1;i<10000;i++)if(i%8==2)btree_insert(i,t);
    pthread_exit(NULL);
}
void *fun4(void *tree){
    tnode * t= (tnode *)tree;
    for(int i=1;i<10000;i++)if(i%8==3)btree_insert(i,t);
    pthread_exit(NULL);
}
void *fun5(void *tree){
    tnode * t= (tnode *)tree;
    for(int i=1;i<10000;i++)if(i%8==4)btree_insert(i,t);
    pthread_exit(NULL);
}
void *fun6(void *tree){
    tnode * t= (tnode *)tree;
    for(int i=1;i<10000;i++)if(i%8==5)btree_insert(i,t);
    pthread_exit(NULL);
}
void *fun7(void *tree){
    tnode * t= (tnode *)tree;
    for(int i=1;i<10000;i++)if(i%8==6)btree_insert(i,t);
    pthread_exit(NULL);
}
void *fun8(void *tree){
    tnode * t= (tnode *)tree;
    for(int i=1;i<10000;i++)if(i%8==7)btree_insert(i,t);
    pthread_exit(NULL);
}
int main(){
    tnode *tree = btree_create();
    pthread_t thread1,thread2,thread3,thread4,thread5,thread6,thread7,thread8;
    if(pthread_create(&thread1,NULL,fun1,tree)!=0){
        printf("error: pthread_create\n");
        exit(1);
    }
    if(pthread_create(&thread2,NULL,fun2,tree)!=0){
        printf("error: pthread_create\n");
        exit(1);
    }
    if(pthread_create(&thread3,NULL,fun3,tree)!=0){
        printf("error: pthread_create\n");
        exit(1);
    }
    if(pthread_create(&thread4,NULL,fun4,tree)!=0){
        printf("error: pthread_create\n");
        exit(1);
    }
    if(pthread_create(&thread5,NULL,fun5,tree)!=0){
        printf("error: pthread_create\n");
        exit(1);
    }
    if(pthread_create(&thread6,NULL,fun6,tree)!=0){
        printf("error: pthread_create\n");
        exit(1);
    }
    if(pthread_create(&thread7,NULL,fun7,tree)!=0){
        printf("error: pthread_create\n");
        exit(1);
    }
    if(pthread_create(&thread8,NULL,fun8,tree)!=0){
        printf("error: pthread_create\n");
        exit(1);
    }
    if(pthread_join(thread1,NULL)!=0){
        printf("error: pthread_join\n");
        exit(1);
    }
    if(pthread_join(thread2,NULL)!=0){
        printf("error: pthread_join\n");
        exit(1);
    }
    if(pthread_join(thread3,NULL)!=0){
        printf("error: pthread_join\n");
        exit(1);
    }
    if(pthread_join(thread4,NULL)!=0){
        printf("error: pthread_join\n");
        exit(1);
    }
    if(pthread_join(thread5,NULL)!=0){
        printf("error: pthread_join\n");
        exit(1);
    }
    if(pthread_join(thread6,NULL)!=0){
        printf("error: pthread_join\n");
        exit(1);
    }
    if(pthread_join(thread7,NULL)!=0){
        printf("error: pthread_join\n");
        exit(1);
    }
    if(pthread_join(thread8,NULL)!=0){
        printf("error: pthread_join\n");
        exit(1);
    }
    btree_dump(tree);
    return 0;
}