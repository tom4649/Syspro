#ifndef BTREE_H
#define BTREE_H

typedef struct tnode{
    int num;
    struct tnode *left ,*right;
} tnode;
tnode *btree_create();
void btree_insert(int v,tnode* t);
void btree_destroy(tnode *t);
void btree_dump(tnode *t);

#endif
