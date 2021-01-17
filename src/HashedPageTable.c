#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/HashedPageTable.h"
#include "../include/Queue.h"

// hash function
unsigned int hashInt(unsigned int n, int tableSize) { return ((n >> 12) + n) % tableSize; }

p_listNode *createPageListNode(unsigned int page, int pid, char status, int count) {
    p_listNode *newNode = (p_listNode *) malloc(sizeof(p_listNode));
    newNode->pageInfo = malloc(sizeof(pageInfo ));
    newNode->pageInfo->pageNum = page;
    newNode->pageInfo->pid = pid;
    newNode->pageInfo->status = status;

    newNode->count = count;
    newNode->next = NULL;

    return newNode;
}

void initHPT( HPT *hpt,char *algo, int frames) {
    hpt->pageFaults = 0;
    hpt->nReads = hpt->nWrites = 0;
    hpt->diskWrites = 0;
    hpt->frames = frames;
    hpt->size = frames / 2;
    
    hpt->algorithm = malloc(sizeof(char) * strlen(algo) + 1);
    strcpy(hpt->algorithm, algo);

    hpt->arr = (bucket **) malloc( (hpt->size) * sizeof(bucket *) );
    for(int i = 0; i < hpt->size; i++) {
        hpt->arr[i] = (bucket *) malloc(sizeof(bucket));
        hpt->arr[i]->head = NULL;
        hpt->arr[i]->last = NULL;
    }
}

int pageExists(HPT hpt, unsigned int pageNum) {
    unsigned int index = hashInt(pageNum, hpt.size);

    p_listNode *list = hpt.arr[index]->head;

    // page doesn't exist
    if(list == NULL) {
        // return -1
        return -2;
    } else {
        // traverse bucket list
        while(list) {
            if(list->pageInfo->pageNum == pageNum)
                return 1;

            list = list->next;
        }
    }

    return -1;
}

void deleteHashTablePage(HPT *hpt, p_listNode *node) {
    unsigned int index = hashInt(node->pageInfo->pageNum, hpt->size);
    p_listNode *list = hpt->arr[index]->head;
    p_listNode *prev = NULL;

    // printf("Delete page at index %d \n", index);q

    if(list == NULL) {
        printf("problem occured!\n");
        exit(-1);
    }

    if(list != NULL && list->pageInfo->pageNum == node->pageInfo->pageNum) {
        
        if(hpt->arr[index]->last == hpt->arr[index]->head) {
            hpt->arr[index]->head = hpt->arr[index]->last = NULL;
        } else
            hpt->arr[index]->head = list->next;
        
        if(hpt->arr[index]->head == NULL)
            hpt->arr[index]->last = NULL;

        // free(list);

        // update reads/writes
        if(node->pageInfo->status == 'W') {
            hpt->diskWrites++;
        }

        return ;
    }

    // traverse list of buckets
    while(list && list->pageInfo->pageNum != node->pageInfo->pageNum) {
        prev = list;
        list = list->next;
    }

    if(list == NULL) {
        perror("error while deleting in hash table!\n");
        exit(-1);
    };

    if(list->next == NULL) {
        hpt->arr[index]->last = prev;
        prev->next = NULL;
    } else 
        prev->next = list->next;
    
    // free(list);

    if(node->pageInfo->status == 'W') {
        hpt->diskWrites++;
    }
}

void updateHashTablePage(HPT *hpt, unsigned int page, char status, int count) {
    unsigned int index = hashInt(page, hpt->size);
    p_listNode *list = hpt->arr[index]->head;

    // traverse list of buckets
    while(list) {

        if(list->pageInfo->pageNum == page)
        {
            list->count = count;

            if(status == 'W')
                hpt->nWrites++;
            else 
                hpt->nReads++;

            return ;
        }

        list = list->next;
    }    
}

void deleteByPage(HPT *hpt, unsigned int page, char status) {
    unsigned int index = hashInt(page, hpt->size);
    p_listNode *list = hpt->arr[index]->head;
    p_listNode *prev = NULL;

    // printf("Delete page at index %d \n", index);q

    if(list == NULL) {
        printf("problem occured!\n");
        exit(-1);
    }

    if(list != NULL && list->pageInfo->pageNum == page) {
        
        if(hpt->arr[index]->last == hpt->arr[index]->head) {
            hpt->arr[index]->head = hpt->arr[index]->last = NULL;
        } else
            hpt->arr[index]->head = list->next;
        
        if(hpt->arr[index]->head == NULL)
            hpt->arr[index]->last = NULL;
        
        free(list->pageInfo);
        free(list);

        // update reads/writes
        if(status == 'W') {
            hpt->diskWrites++;
        }

        return ;
    }

    // traverse list of buckets
    while(list && list->pageInfo->pageNum != page) {
        prev = list;
        list = list->next;
    }

    if(list == NULL) {
        perror("error while deleting in hash table!\n");
        exit(-1);
    };

    if(list->next == NULL) {
        hpt->arr[index]->last = prev;
        prev->next = NULL;
    } else 
        prev->next = list->next;
    
    free(list->pageInfo);
    free(list);

    if(status == 'W') {
        hpt->diskWrites++;
    }
}

void push(p_listNode **head,p_listNode **last ,p_listNode* data) {
    
    if((*head) == NULL)  {
        (*head) = (*last) = data;
        return;
    }

    (*last)->next = data;
    (*last) = data;
}

void insertPageHelper(HPT *hpt, p_listNode *node) {
    int pos = hashInt(node->pageInfo->pageNum, hpt->size);
    // printf("Insertign page %d pid %d hash %d %c %d\n",
        // node->pageInfo->pageNum,node->pageInfo->pid ,pos, node->pageInfo->status, node->count);
    // list insertion
    push(&hpt->arr[pos]->head, &hpt->arr[pos]->last, node);

    if(node->pageInfo->status == 'W')
        hpt->nWrites++;
    else 
        hpt->nReads++;
}

void printList(p_listNode *node) {
    while(node != NULL) {
        printf("    Page \"%d\" %c with count %d and pid : %d ", 
                node->pageInfo->pageNum,node->pageInfo->status, node->count, node->pageInfo->pid);
        if(node->next != NULL)
            printf("->  ");

        node = node->next;
    }
    printf("\n");
}

void printStats(HPT hpt) {
    printf("\nThis is the Hashed Page Table\n");
	printf("Algorithm=%s, PageFaults[%d], reads[%d], writes[%d], disk_writes[%d], max_frames[%d]\n",
		hpt.algorithm, hpt.pageFaults, hpt.nReads, hpt.nWrites, hpt.diskWrites, hpt.frames);
	
    for(int i = 0; i < hpt.size; i++) {
        if(hpt.arr[i]->head != NULL) {
            printf("i = %d\n    ", i);
            printList(hpt.arr[i]->head);
        }
    }
}

void deleteHPT(HPT *hpt) {
    for(int i = 0; i < hpt->size; i++) {
        // deletePageList(&hpt->arr[i]->head);
        // hpt->arr[i]->last = NULL;
        free(hpt->arr[i]);
    }
    free(hpt->arr);

    free(hpt->algorithm);
}

void deleteHPT_SC(HPT *hpt) {
    for(int i = 0; i < hpt->size; i++) {
        
        // delete bucket list
        p_listNode *current = hpt->arr[i]->head;
        p_listNode *temp;

        while(current) {
            temp = current->next;
            free(current->pageInfo);
            free(current);

            current = temp;
        }
        hpt->arr[i]->head = hpt->arr[i]->last = NULL;

        free(hpt->arr[i]);
    }
    free(hpt->arr);

    free(hpt->algorithm);
}

