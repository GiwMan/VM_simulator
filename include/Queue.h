#ifndef queue_h_
#define queue_h_

#include <stdbool.h>

#include "./HashedPageTable.h"

typedef struct Qnode {
    struct PageListNode *data;
    // pageInfo *data; 
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

p_listNode *enQueue(Queue *q, unsigned int page, int pid, char mode, int count);

Qnode *getFront(Queue q);

void deQueue(Queue *q);

void updateQueue(Queue *q, unsigned int page, int pid, char mode, int count);

void printQueue(Queue q);

void deleteQueue(Queue *q);

#endif 

            