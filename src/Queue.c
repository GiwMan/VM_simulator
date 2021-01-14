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

void initQueue(Queue *q) { q->front = q->rear = NULL; q->size = 0; }

bool emptyQueue(Queue q) {
    return (q.size == 0) ? true : false;
}

bool maxFramesReached(Queue q, int frames) {
    return (q.size >= frames-1) ? true : false;
}

void enQueue(Queue *q, p_listNode *node) {
    Qnode *newNode = createqNode(node);

    if(emptyQueue(*q) == true) {
        q->front = q->rear = newNode;
        q->size++;
        return ;
    }

    q->rear->next = newNode;
    q->rear = newNode;
    q->size++;
}

Qnode *getFront(Queue q) {
    if(emptyQueue(q) == true)
        return NULL;

    return q.front;
}

void deQueue(Queue *q) {
    if(emptyQueue(*q)) {
        return ;
    }

    Qnode *prev = q->front;
    

    q->front = q->front->next;

    // empty queue
    if(q->front == NULL)
        q->rear = NULL;

    // free(prev->pageData);
    free(prev);
    
    q->size--;
}

void updateQueue(Queue *q, p_listNode *node) {
    if(emptyQueue(*q) == true)
        return ;
    
    Qnode *current = q->front;
    Qnode *prev = NULL;

    if( (current != NULL) && 
        (current->pageData->pageNum == node->pageNum) )
    {
        q->front = current->next;
        free(current);

        enQueue(q, node);
        return;
    }

    while ( (current) != NULL && 
            (current->pageData->pageNum != node->pageNum) )
            // (current->pageData->pid != node->pid)   )
    {
        prev = current;
        current = current->next;
    }

    if(current == NULL) return;

    //  if(prev != NULL)
    //     printf("current  = %d node = %d prev = %d count  = %d\n", current->pageData->pageNum, node->pageNum, prev->pageData->pageNum, node->count);

    // delete node
    if(current->next == NULL)//= current->next;
    {    
        current->pageData->count = node->count;
        // printf(" current->next Einai null \n");

        // prev->next = NULL;
        // prev->pageData->count = node->count;
       

        return;
    } else {
        prev->next = current->next;
        free(current);
    }
   
    // insert new node at the end
    enQueue(q, node);
}

void printQueue(Queue q) {
    if(emptyQueue(q)) {
        printf("Empty Queue\n");
        return ;
    } 

    Qnode *current = q.front;

    while(current) {
        printf("Page %d, pid %d, count %d\n", 
            current->pageData->pageNum, current->pageData->pid,
            current->pageData->count
            );

        current = current->next;
    }
}

void deleteQueue(Queue *q) {
    if(emptyQueue(*q) == true) {
        printf("Empty Queue\n");
        return ;
    }

    Qnode *current = q->front;
    Qnode *temp = NULL;

    while(current) {
        temp = current->next;
        free(current->pageData);
        free(current); 

        current = temp;
    }

    q->front = q->rear = NULL;
}

