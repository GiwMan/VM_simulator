#ifndef queue_h_
#define queue_h_

#include <stdbool.h>

#include "./PageTable.h"

typedef struct Qnode {
    struct PageListNode *pageData;
    struct Qnode *next;
} Qnode;

typedef struct Queue {
    struct Qnode *front, *rear;
    int size;
} Queue;

Qnode *createqNode(p_listNode* node);

void initQueue(Queue *q);

bool emptyQueue(Queue q);

bool maxFramesReached(Queue q, int frames);

void enQueue(Queue *q, p_listNode *node, bool LRU);

Qnode *getFront(Queue q);

void deQueue(Queue *q, bool LRU);

void updateQueue(Queue *q, p_listNode *node, bool LRU);

void printQueue(Queue q);

void deleteQueue(Queue *q);

#endif 

            