#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct tnode {
    int num;
    struct tnode *left ,*right;
} tnode;;

tnode *btree_create(){
    tnode *n = malloc(sizeof(tnode));
    n->num=0;n->left=NULL;n->right=NULL;
    return n;
}

void btree_insert(int v, tnode *t){
    tnode *pos;pos=t;
    
    if(pos->num < v){
        if(pos->right ==NULL){
            pthread_mutex_lock(&mutex);
            if(pos->right!=NULL){
                pthread_mutex_unlock(&mutex);
                btree_insert(v,pos->right);
                return;
            }
            tnode *tmp = btree_create();tmp->num=v;
            pos->right = tmp;
            pthread_mutex_unlock(&mutex);
            return;
        }
        else {/*pthread_mutex_unlock(&mutex);*/btree_insert(v,pos->right);return;}
    }
    else if(pos->left == NULL){
        pthread_mutex_lock(&mutex);
        if(pos->right!=NULL){
            pthread_mutex_unlock(&mutex);
            btree_insert(v,pos->right);
            return;
        }
        tnode *tmp=btree_create();
        tmp ->num =v;
        pos->left= tmp;
        pthread_mutex_unlock(&mutex);
        return;
    }
    else {/*pthread_mutex_unlock(&mutex);*/btree_insert(v,pos->left);return;}
    pthread_mutex_unlock(&mutex);
    return;
}

void btree_destroy(tnode *t){
    if(t->left !=NULL)btree_destroy(t->left);
    if(t->right !=NULL)btree_destroy(t->right);
    free(t);return;
}

void btree_dump(tnode *t){
    if(t->left != NULL)btree_dump(t->left);
    printf("%d\n",t->num);
    if(t->right != NULL)btree_dump(t->right);
    return;
}