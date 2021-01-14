#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/Queue.h"

Qnode *createqNode(p_listNode *node) {
    struct Qnode *newNode = (struct Qnode *) malloc(sizeof(struct Qnode));
    newNode->pageData = node;
    newNode->next= NULL;

    return newNode;
}

void initQueue(Queue *q) { q->front = q->rear = NULL; }

bool emptyQueue(Queue q) {
    return (q.front == q.rear) ? true : false;
}

void enQueue(Queue *q, p_listNode *node) {
    Qnode *newNode = createqNode(node);

    if(emptyQueue(*q)) {
        q->front = q->rear = newNode;
        return ;
    }

    q->rear->next = newNode;
    q->rear = newNode;
}

Qnode *deQueue(Queue *q) {
    if(emptyQueue(*q)) {
        return NULL;
    }

    Qnode *prev = q->front;
    Qnode *toDelete = q->front;

    q->front = q->front->next;

    // empty queue
    if(q->front == NULL)
        q->rear = NULL;

    free(prev);

    return toDelete;
}

void printQueue(Queue q) {
    if(emptyQueue(q)) {
        print("Empty Queue\n");
        return ;
    } 

    Qnode *current = q.front;

    while(current) {
        print("Page %d, pid %d, count %d\n", 
            current->pageData->pageNum, current->pageData->pid,
            current->pageData->count
            );

        current = current->next;
    }
}

void deleteQueue(Queue *q) {
    if(emptyQueue(*q)) {
        printf("Empty Queue\n");
        return ;
    }

    Qnode *current = q->front;
    Qnode *temp;

    while(current) {
        temp = current->next;
        free(current); 

        current = current->next;
    }

    q->front = q->rear = NULL;
}

