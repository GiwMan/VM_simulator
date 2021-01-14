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
} Queue;

Qnode *createqNode(p_listNode* node);

void initQueue(Queue *q);

bool emptyQueue(Queue q);

void enQueue(Queue *q, p_listNode *node);

Qnode *deQueue(Queue *q);

void printQueue(Queue q);

void deleteQueue(Queue *q);

#endif 