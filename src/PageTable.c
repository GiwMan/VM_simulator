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
/*
entryPtr deletePageNode(entryPtr *head, int key) {
    entryPtr current = *head; 
    entryPtr prev, temp;

    if(current && current->data.count == key) {
        temp  = current;
        *head = current->nextPage;
        free(current);

        return temp;
    }

    while(current && current->data.count != key) {
        prev = current;
        current = current->nextPage;
    }   

    if(current == NULL) return NULL;

    prev->nextPage = current->nextPage;

    temp = current;

    free(current);

    return temp; // return the deleted node
}


void deltePageList(entryPtr *head) {
    entryPtr current = *head;
    entryPtr temp;

    while(current != NULL) {
        temp = current->nextPage;
        free(current);
        current = temp;
    }

    *head = NULL;
}
*/


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

p_listNode *updatePageNode(HPT *hpt, p_listNode* node) {
    int hashValue = hashInt(node->pageNum, hpt->size);

    // printf("Inserting page : %d with count %d in hash = %d with pid %d\n", node->pageNum, node->count, hashValue, node->pid);
    p_listNode *list = hpt->arr[hashValue]->head;
    
    p_listNode *current, *prev;

    // if list is not empty
    if( list != NULL) {

        // check for head node
        if(list != NULL && (list->pageNum == node->pageNum) ) {
            
            current = list;
            if(list->next != NULL) {
                hpt->arr[hashValue]->head = list->next;

                free(list);

                insertPageHelper(hpt, node);
            } 
            
            list->count = node->count;
            if(node->status == 'W')
                hpt->nWrites++;
            else
                hpt->nReads++;

            return list;
        } 
        
        current = list;
        while( list != NULL && list->pageNum != node->pageNum) {
            prev = list;
            list = list->next;
        }

        if(list != NULL && list->next == NULL) {
            // no need to delete current node, just to update it
            //  printf("previous count = %d\n", list->count);
            list->count = node->count;

            if(node->status == 'W')
                hpt->nWrites++;
            else
                hpt->nReads++;
            // printf("new count = %d\n", node.count);

            return list;
        } else if (list == NULL) {
            return NULL;
        } else {
            prev->next = list->next;

            current = list;
            current->next = NULL;
            // delete the node with same pageNumber found
            free(list);

            // push the node at the end
            insertPageHelper(hpt, node);

            if(node->status == 'W')
                hpt->nWrites++;
            else
                hpt->nReads++;

            return current;
        } 
    }
    return NULL;
}

void push(p_listNode **head,p_listNode **last ,p_listNode* data) {
    
    if((*last) == NULL) {
        (*head) = (*last) = data;
        return;
    }

    (*last)->next = data;
    (*last) = data;
}

void append(p_listNode **head, p_listNode *data) {
    p_listNode *end = *head;

    if(*head == NULL) {
        // printf("mpike stin arxi!\n");
        *head = data;
        return;
    }

    while(end->next != NULL) {
        end = end->next;
    }

    end->next = data;
    return;
}


void insertPageHelper(HPT *hpt, p_listNode* node) {

    int pos = hashInt(node->pageNum, hpt->size);
    // printf("page %d pid %d hash %d %c %d\n",node->pageNum,node->pid ,pos, node->status, node->count);
    // list insertion
    push(&hpt->arr[pos]->head, &hpt->arr[pos]->last ,node);
}

void insertPage( HPT *hpt,
                 p_listNode* node )
{
    int count = node->count;
    int maxFrames = hpt->frames;
    int indexOfVictim = 0;
    
    // insertPageHelper(hpt, node);
    p_listNode *updatedNode = updatePageNode(hpt, node);
    // int minValue = -1;
    

    if(updatedNode != NULL) {
        // page found in hpt
        // and we updated the node
        // printf("Updated node: %d\n",updatedNode->count);
        // printf("page %d with pid %d already exists!\n", node->pageNum,node->pid);
        // if page with same number and pid inserted
        // then place the old node at
    
    } else {
        // page not found
        // insert it in the list of a bucket
        if(fullTable(count, maxFrames) == true) {
            // find the page with the minimum count
            // and replace it
            // printf("Before Inserting page %d with count %d pid = %d\n", node->pageNum, node->count, node->pid);

            // printStats(*hpt);
            indexOfVictim = findVictim(*hpt, node);

            if(indexOfVictim  >= 0) {
                replaceVictim(hpt, indexOfVictim, node);
            } else {
                perror("findVictim");
                // printf("error\n");
                exit(-1);
            }
            
        } else {
            
            // printf("\n page %d not found, insert it!\n", node->pageNum);
            // insert page 
            insertPageHelper(hpt, node);

            if(node->status == 'W')
                hpt->nWrites++;
            else 
                hpt->nReads++;
        }
        // increase page fault
        hpt->pageFaults++;
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

int findVictim(HPT hpt, p_listNode *node) {
    int min = (int) INFINITY;
    int pos = -1;

    for(int i = 0; i < hpt.size; i++) {
        p_listNode *temp = hpt.arr[i]->head;

        if(temp) {
            min = temp->count;
            pos = i;

            for(int j = i+1; j < hpt.size; j++) {
                if(hpt.arr[j]->head != NULL) {
                    
                    temp = hpt.arr[j]->head;

                    if(min > temp->count) {
                        min = temp->count;
                        pos = j;
                    }
                }
            }
            // we have found min
            break;
        }
    }
    return pos;
}

void replaceVictim(HPT *hpt,int indexOfVictim ,p_listNode *node) {
    
    // printf("Victim found at position %d with page = %d\n", indexOfVictim, node->pageNum);
    // if(hpt->arr[indexOfVictim]->head)
    //     printf("Deleting page : %d\n\n", hpt->arr[indexOfVictim]->head->pageNum);

    // let's delete the head node of the list
    // in position indexOfVictim
    if(hpt->arr[indexOfVictim]->head != NULL && 
        hpt->arr[indexOfVictim]->head->status == 'W') 
    {
        hpt->diskWrites++;
        // hpt->nWrites++;
    } 

    if(node->status == 'W') hpt->nWrites++;
    else hpt->nReads++;

    p_listNode *deletedNode = deleteHeadNode(&hpt->arr[indexOfVictim]->head, &hpt->arr[indexOfVictim]->last);
    insertPageHelper(hpt, node);
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
    for(int i = 0; i < hpt->size; i++) {
        deletePageList(&hpt->arr[i]->head);
        hpt->arr[i]->last = NULL;
        // free(hpt->arr[i]);
    }

    deleteBuckets(hpt->arr, hpt->size);
    // free(hpt->arr);
   
    free(hpt->algorithm);
}


