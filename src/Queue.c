#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/Queue.h"

Qnode *createqNode(p_listNode *node) {
    struct Qnode *newNode = (struct Qnode *) malloc(sizeof(struct Qnode));
    newNode->data = node;
    newNode->next= NULL;

    return newNode;
}

void initQueue(Queue *q) { q->front = q->rear = NULL; q->size = 0; }

bool emptyQueue(Queue q) {
    return (q.size == 0) ? true : false;
}

bool maxFramesReached(Queue q, int frames) {
    return (q.size >= frames) ? true : false;
}

p_listNode *enQueue(Queue *q, unsigned int page, int pid, char mode, int count) {
    p_listNode *newPage = createPageListNode(page, pid, mode, count);
    Qnode *newNode = createqNode(newPage);

    if(emptyQueue(*q) == true) {
        // printf("empty queue\n");
        q->front = q->rear = newNode;
        q->size++;
        return newPage;
    }
    
    q->rear->next = newNode;
    q->rear = newNode;
    
    q->size++;

    return newPage;
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
    free(prev->data->pageInfo);
    free(prev->data);
    free(prev);
       
    q->size--;
}

void updateQueue(Queue *q, unsigned int page, int pid, char mode, int count) {
    if(emptyQueue(*q) == true)
        return ;
    
    Qnode *current = q->front;
    Qnode *prev = NULL;

    if( (current != NULL) && 
        (current->data->pageInfo->pageNum == page) )
    {
        // change position of head node
        // and move head to read
        q->front = current->next;
        
        current->next = q->rear->next;
        q->rear->next = current;
        q->rear = current;
        
        return ;
        // enQueue(q, page, pid, mode, count);
        // return;
    }

    while ( (current) != NULL && 
            (current->data->pageInfo->pageNum != page) )
            // (current->pageData->pid != node->pid)   )
    {
        prev = current;
        current = current->next;
    }

    if(current == NULL) return ;

    if(current->next == NULL)//= current->next;
    {    
        current->data->count = count;

        return ;
    } else {
        prev->next = current->next;

        current->next=q->rear->next;
        q->rear->next = current;
        q->rear = q->rear->next;

        // q->size--;
    }
}

void printQueue(Queue q) {
    if(emptyQueue(q)) {
        printf("Empty Queue\n");
        return ;
    } 

    Qnode *current = q.front;

    while(current) {
        printf("Page %d, pid %d, count %d\n", 
            current->data->pageInfo->pageNum, current->data->pageInfo->pid,
            current->data->count
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
        // free(current->pageData);
        free(current->data->pageInfo);
        free(current->data);
        free(current); 

        current = temp;
    }

    q->front = q->rear = NULL;
}

