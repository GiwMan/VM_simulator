#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/PageTable.h"

p_listNode* createListNode(  int pid, 
                            unsigned int addr, 
                            char mode, 
                            int c  ) 
{
    p_listNode *newNode = malloc(sizeof(p_listNode));

    newNode->pid = pid;
    newNode->pageNum = addr;
    newNode->status = mode;
    newNode->count = c;
    newNode->next = NULL;

    return newNode;
}

/* hash function */
unsigned int hashInt(unsigned int n, int tableSize) { return ((n >> 12) + n) % tableSize; }

void initHPT( HPT *hpt,char *algo, int frames) {
  
    hpt->pageFaults = 0;
    hpt->nReads = hpt->nWrites = 0;
    hpt->diskWrites = 0;
    hpt->frames = frames;
    hpt->size = frames;
    
    hpt->algorithm = malloc(sizeof(char) * strlen(algo) + 1);
    strcpy(hpt->algorithm, algo);

    hpt->arr = (bucket **) malloc( (hpt->size) * sizeof(bucket *) );
    for(int i = 0; i < hpt->size; i++) {
        hpt->arr[i] = (bucket *) malloc(sizeof(bucket));
        hpt->arr[i]->head = NULL;
        hpt->arr[i]->last = NULL;
    }
}

bool fullTable(int count, int maxFrames) {
    return (count > maxFrames) ? true : false;
}

void push(p_listNode **head,p_listNode **last ,p_listNode* data) {
    
    if((*last) == NULL) {
        (*head) = (*last) = data;
        return;
    }

    (*last)->next = data;
    (*last) = data;
}

void insertPageHelper(HPT *hpt, p_listNode* node) {

    int pos = hashInt(node->pageNum, hpt->size);
    // printf("page %d pid %d hash %d %c %d\n",node->pageNum,node->pid ,pos, node->status, node->count);
    // list insertion
    push(&hpt->arr[pos]->head, &hpt->arr[pos]->last ,node);

    if(node->status == 'W')
        hpt->nWrites++;
    else 
        hpt->nReads++;
}

int pageExists(HPT hpt, int pageNum) {
    unsigned int index = hashInt(pageNum, hpt.size);

    p_listNode *list = hpt.arr[index]->head;

    // page doesn't exist
    if(list == NULL) {
        // return -1
        return -2;
    } else {
        // traverse bucket list
        while(list) {
            if(list->pageNum == pageNum)
                return 1;

            list = list->next;
        }
    }

    return -1;
}

int insertHashTable(HPT *hpt, p_listNode *node) 
{
    int pageExist = pageExists(*hpt, node->pageNum);

    if(pageExist == 1) {
        
        return PAGE_EXISTS;
    } else {
        // page fault
        hpt->pageFaults++;

        insertPageHelper(hpt, node);

        return PAGE_INSERTED;
        // }
    }
}

void updateHashTablePage(HPT *hpt, p_listNode node) {
    unsigned int index = hashInt(node.pageNum, hpt->size);
    p_listNode *list = hpt->arr[index]->head;

    // traverse list of buckets
    while(list) {
        if(list->pageNum == node.pageNum)
        {
            list->count = node.count;

            if(node.status == 'W')
                hpt->nWrites++;
            else 
                hpt->nReads++;

            return ;
        }

        list = list->next;
    }    
}

void deleteHashTablePage(HPT *hpt, p_listNode node) {
    unsigned int index = hashInt(node.pageNum, hpt->size);
    p_listNode *list = hpt->arr[index]->head;
    p_listNode *prev = NULL;

    //  printf("Delete page at index %d \n", index);

    if(list == NULL) {
        printf("problem occured!\n");
        exit(-1);
    }

    if(list != NULL && list->pageNum == node.pageNum) {
        
        if(hpt->arr[index]->last == hpt->arr[index]->head) {
            hpt->arr[index]->head = hpt->arr[index]->last = list->next;
        } else
            hpt->arr[index]->head = list->next;

        free(list);

        // update reads/writes
        if(node.status == 'W') {
            hpt->diskWrites++;
        }

        return ;
    }

    // traverse list of buckets
    while(list && list->pageNum != node.pageNum) {
        prev = list;
        list = list->next;
    }

    prev->next = list->next;
    free(list);

    if(node.status == 'W') {
        hpt->diskWrites++;
    }

}

p_listNode *deleteHeadNode(p_listNode **head, p_listNode **last) {
    p_listNode *temp = *head;
    p_listNode *current;

    if((*head == *last) && ( (*head) != NULL ) ) {

        current = temp;
        // printf("deleting node with page number %d and count = %d\n", temp->pageNum, temp->count);
        free(temp);
        temp = NULL;

        (*head) = (*last) = NULL;
        
        return current;
    } else {
        if(temp != NULL) {
            *head = temp->next;

            current = temp;
            free(temp);

            temp = NULL;

            return current;
        }
    }

    return NULL;
}

/**
 * Find victim for Second chance algorithm
 */
int findClockVictim(HPT hpt, p_listNode *node) {
    int min = (int) INFINITY;
    int pos = -1;
    
    for(int i = 0; i < hpt.size; i++) {
        p_listNode *temp = hpt.arr[i]->head;

        if(temp != NULL) {
            min = temp->count;
            pos = i;
        }
        for(int j = i + 1; j < hpt.size; j++) {
            temp = hpt.arr[j]->head;
            
            // traverse the list if needed
            while( (temp != NULL)  && (temp->secondChance == false)) {
                if(temp->count < min) {
                    min = temp->count;
                    pos = j;
                }
            }
            // if(temp != NULL) {
            //     if( (temp->secondChance == false) &&  (min > temp->count)) {
            //         min = temp->count;
            //         pos = j;
            //     }
            // }
        }
        // we have found min
        break;
    }

    return pos;
}



void printList(p_listNode *node) {
    while(node != NULL) {
        printf("    Page \"%d\" %c with count %d and pid : %d ", 
                node->pageNum,node->status, node->count, node->pid);
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

void deletePageList(p_listNode **head) {
    p_listNode *current = *head;
    p_listNode *temp;

    while(current != NULL) {
        temp = current->next;
        free(current);

        current = temp;
    }

    *head  = NULL;
}

void deleteBuckets(bucket **b, int size) {
    for(int i = 0; i < size; i++) {
        free(b[i]);
    }
    free(b);
}

void deleteHPT(HPT *hpt) {
    // for(int i = 0; i < hpt->size; i++) {
    //     deletePageList(&hpt->arr[i]->head);
    //     hpt->arr[i]->last = NULL;
    //     // free(hpt->arr[i]);
    // }

    deleteBuckets(hpt->arr, hpt->size);
    // free(hpt->arr);
   
    free(hpt->algorithm);
}


