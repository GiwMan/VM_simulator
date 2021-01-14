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

void enQueue(Queue *q, p_listNode *node);

Qnode *getFront(Queue q);

void deQueue(Queue *q);

void updateQueue(Queue *q, p_listNode *node);

void printQueue(Queue q);

void deleteQueue(Queue *q);

#endif 
/*
else {
                // LRU replacement
                Qnode *front = getFront(q_lru);

                printf("Deleting page %d with pid = %d\n", front->pageData->pageNum, front->pageData->pid);                

                printQueue(q_lru);

                // enqueue new node
                enQueue(&q_lru, p_node);

                // insert the new node
                insertPageHelper(&hpt_p2, p_node);

                if(p_node->status == 'W')
                    hpt_p1.nWrites++;
                else 
                    hpt_p1.nReads++;

                 // delete from hash table old node
                if(front->pageData->pid == 1) {
                   
                    deleteHashTablePage(&hpt_p1, *front->pageData);
                    
                } else {
                    deleteHashTablePage(&hpt_p2, *front->pageData);
                }

                // delete node from queue
                deQueue(&q_lru);
            } */
            