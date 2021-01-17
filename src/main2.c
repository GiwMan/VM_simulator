#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#include "../include/HashedPageTable.h"
#include "../include/Queue.h"
#include "../include/SCinfo.h"
#include "../include/PageInfo.h"


#define PAGE_SIZE 4096
#define MAX_TRACE 25

void insertPage(HPT *hpt1, HPT *hpt2, Queue *q,unsigned int page, int pid, char mode, int count) {
    
    if(pid == 1) {

        if(pageExists(*hpt1, page) == 1) {
            // updates
            // p_listNode *updatedNode = 
            updateQueue(q, page, pid, mode, count);

            // if(updatedNode)
                // updateHashTablePage(hpt1, updatedNode);
            updateHashTablePage(hpt1, page, mode, count);
            // else {
                // perror("updatedNode");
                // exit(-1);
            // }

        } else {
            hpt1->pageFaults++;

            if(maxFramesReached(*q, hpt1->frames) == true) {
                // printf("max frames = %d %d\n", q->size, hpt1->frames);
                // we need to delete victim
                Qnode *front = getFront(*q);

                // printf("Deleting page %d with pid %d\n", 
                    // front->data->pageInfo->pageNum, front->data->pageInfo->pid);

                if(front->data->pageInfo->pid == 1) {
                    deleteHashTablePage(hpt1, front->data);
                } else {
                    deleteHashTablePage(hpt2, front->data);
                }

                deQueue(q);

                p_listNode *toInsert = enQueue(q, page, pid, mode, count);

                
                // printf("Inserting into Table: %d, count : %d status %c\n", 
                    // toInsert->pageInfo->pageNum,toInsert->count, toInsert->pageInfo->status);

                insertPageHelper(hpt1, toInsert);

            } else {
                
                p_listNode *toInsert = enQueue(q, page, pid, mode, count);

                // printf("Inserting into Table: %d, count : %d status %c\n", 
                    //  toInsert->pageInfo->pageNum,toInsert->count, toInsert->pageInfo->status);

                insertPageHelper(hpt1, toInsert);
            }
        }
    } else {
        // for pid = 2
        if(pageExists(*hpt2, page) == 1) {
            // updates
            // printf("page %d exists! \n", page);
            updateQueue(q, page, pid, mode, count);

            // if(updatedNode)
                // updateHashTablePage(hpt1, updatedNode);
            updateHashTablePage(hpt2, page, mode, count);

            // p_listNode *updatedNode = updateQueue(q, page, pid, mode, count);

            // (ifupdatedNode)
            //     updateHashTablePage(hpt2, updatedNode);
            // else {
            //     perror("updatedNode");
            //     exit(-1);
            // }

        } else {
            hpt2->pageFaults++;

            if(maxFramesReached(*q, hpt2->frames) == true) {
                // printf("max frames = %d %d\n", q->size, hpt2->frames);
                // we need to delete victim
                Qnode *front = getFront(*q);

                // printf("Deleting page %d with pid %d\n", 
                //     front->data->pageInfo->pageNum, front->data->pageInfo->pid);

                if(front->data->pageInfo->pid == 1) {
                    deleteHashTablePage(hpt1, front->data);
                } else {
                    deleteHashTablePage(hpt2, front->data);
                }

                deQueue(q);

                p_listNode *toInsert = enQueue(q, page, pid, mode, count);

                
                // printf("Inserting into Table: %d, count : %d status %c\n", 
                //     toInsert->pageInfo->pageNum,toInsert->count, toInsert->pageInfo->status);

                insertPageHelper(hpt2, toInsert);

            } else {
                
                p_listNode *toInsert = enQueue(q, page, pid, mode, count);

                // printf("Inserting into Table: %d, count : %d status %c\n", 
                //      toInsert->pageInfo->pageNum,toInsert->count, toInsert->pageInfo->status);

                insertPageHelper(hpt2, toInsert);
            }
        }
    }
}

void LRUsimulation(char *algorithm, int maxFrames, int q, int maxTraces) {
    FILE *trace_file1, *trace_file2, *trace_file;
    int pid; // pid of proccesses
    int count = 0; // counter of pages
    bool change = false; // for reading from another trace
    int temp = q;
    int address;
    char status;

    /**
     * File opening
    **/
    if((trace_file1 = fopen("./files/bzip.trace","r")) == NULL) {
        perror("fopen");
        exit(-1);
    }

    if((trace_file2 = fopen("./files/gcc.trace","r")) == NULL) {
        perror("fopen");
        exit(-1);
    }

    // init hash tables
    HPT h1, h2;

    initHPT(&h1, algorithm, maxFrames);
    initHPT(&h2, algorithm, maxFrames);

    Queue q_lru; // LRU struct
    pageInfo pInfo;
    p_listNode *lruNode;

    // init queue
    initQueue(&q_lru);
    unsigned int page;

    // start reading traces and inserting pages
    // into hash tables and LRU structure
    while(count < MAX_TRACE) {

        if(!change) {
            trace_file = trace_file1;
            pid = 1;
        } else {
            trace_file = trace_file2;
            pid = 2;
        }

        fscanf(trace_file,"%x %c", &address, &status);
        page = address >> 12;
        // createPage(&pInfo, pid, address >> 12, status);
        // lruNode = createPageListNode(&pInfo, count);
        insertPage(&h1, &h2, &q_lru, page, pid, status, count);
        // insert page in structures
        // pageInsertion(&h1, &h2, &q_lru, lruNode, pid, count);

        count++; // increase page counter
        q--;

        // read from the other trace
        if(q == 0) {
            change = !change;
            q = temp; // read q again
        }
    }

    printf("\nPrinting Structures:\n");
    printQueue(q_lru);
    printf("\n  ");
    printStats(h1);
    printStats(h2);

    deleteQueue(&q_lru);

    deleteHPT(&h1);
    deleteHPT(&h2);

    fclose(trace_file1);
    fclose(trace_file2);

    return ;
}

// void insertPageSC(HPT *h1, HPT *h2, SCinfo *pageArr, unsigned int page, int pid, char status, int size) {

//     if(pid == 1) {

//         prinitf("\nFor %d: ", page);
//         if(pageExists(*h1, page) == 1) {
            
//             printf("No page fault!");
//             int pos = getVictim(pageArr, pageArr, size);

//             if(pos >= 0) {
//                 pageArr[pos].bitRef = 1;
//             } else {
//                 perror("getVictim()");
//                 exit(-1);
//             }
//         } else {
//             h1->pageFaults++;

//             // if(pageArr[])
//         }
        
//     }
// }

void SCsimulation(char *algorithm, int maxFrames, int q, int maxTrace) {
    FILE *trace_file1, *trace_file2, *trace_file;
    int pid; // pid of proccesses
    int count = 0; // counter of pages
    bool change = false; // for reading from another trace
    int temp = q;
    int address;
    char status;

    /**
     * File opening
    **/
    if((trace_file1 = fopen("./files/bzip.trace","r")) == NULL) {
        perror("fopen");
        exit(-1);
    }

    if((trace_file2 = fopen("./files/gcc.trace","r")) == NULL) {
        perror("fopen");
        exit(-1);
    }

    // init hash tables
    HPT h1, h2;

    // Second Chance struct
    SCinfo pageArr[maxFrames];

    for(int i = 0; i < maxFrames; i++) {
        pageArr[i].pageData = NULL;
        pageArr[i].bitRef = 0;
    }

    initHPT(&h1, algorithm, maxFrames);
    // initHPT(&h2, algorithm, maxFrames);

    int page = 0; // holds the actual page number
    int victimptr = 0;
    bool maxFramesReached = false;
    for(int i = 0; i < maxTrace; i++) {
        
        if(!change) {
            trace_file = trace_file1;
            pid = 1;
        } else {
            trace_file = trace_file2;
            pid = 2;
        }

        fscanf(trace_file,"%x %c", &address, &status);
        page = address >> 12;

        if(pid == 1) {

            printf("\nFor %d: ", page);
            if(pageExists(h1, page) == 1) {
                
                printf("No page fault!");
                int pos = getPage(pageArr, page, maxFrames);

                if(pos >= 0) {
                    pageArr[pos].bitRef = 1;
                } else {
                    perror("getVictim()");
                    exit(-1);
                }

                // update hash table
                updateHashTablePage(&h1, page, status, 0);
            } else {
                h1.pageFaults++;

                if(pageArr[victimptr].bitRef == 1) {
                    
                    do{
                        pageArr[victimptr].bitRef = 0;
                        victimptr++;
                        if(victimptr == maxFrames)
                            victimptr = 0;

                    } while(pageArr[victimptr].bitRef != 0);
                }
                int pageToDelete;
                int pidOfDeleted;
                char statusOfDeleted;

                if(pageArr[victimptr].bitRef == 0) {

                    pageArr[victimptr].bitRef = 1;
                    if(pageArr[victimptr].pageData == NULL) {
                        // create a page
                        pageArr[victimptr].pageData = (pageInfo *) malloc(sizeof(pageInfo));
                        pageArr[victimptr].pageData->pageNum = page;
                        pageArr[victimptr].pageData->pid = pid;
                        pageArr[victimptr].pageData->status = status;
                    } else {
                        // get old data
                        pageToDelete = pageArr[victimptr].pageData->pageNum;
                        pidOfDeleted = pageArr[victimptr].pageData->pid;
                        statusOfDeleted = pageArr[victimptr].pageData->status;

                        pageArr[victimptr].pageData->pageNum = page;
                        pageArr[victimptr].pageData->pid = pid;
                        pageArr[victimptr].pageData->status = status;
                    }

                    // if max frames reached
                    // delete from hash table
                    if(maxFramesReached == true) {

                        printf("Deleting from hash table page %d\n", page);

                        if(pidOfDeleted == 1) {
                            deleteByPage(&h1, pageToDelete, statusOfDeleted);
                        } else {
                            deleteByPage(&h2, pageToDelete, statusOfDeleted);
                        }

                        // insert new page
                        p_listNode *toInsert = createPageListNode(page, pid, status, 0);
                        insertPageHelper(&h1, toInsert);

                    } else {
                        // insert in hash table
                        p_listNode *toInsert = createPageListNode(page, pid, status, 0);
                        insertPageHelper(&h1, toInsert);
                    }

                    victimptr++;
                }
            }

            if(victimptr == maxFrames) {
                victimptr = 0;
                maxFramesReached = true;
            }
    }


        // insertPage(&h1, &h2, &q_lru, page, pid, status, count);
        // insert page in structures
        // pageInsertion(&h1, &h2, &q_lru, lruNode, pid, count);


        q--;
        // read from the other trace
        if(q == 0) {
            change = !change;
            q = temp; // read q again
        }

    }

    printf("\nPrint Structures: \n");
    for(int i = 0; i < maxFrames; i++) {
        printf("    Page: %d status %c\n", 
            pageArr[i].pageData->pageNum, pageArr[i].pageData->status);
    }
    printf("\n");
    printStats(h1);

    deleteHPT_SC(&h1);

    for(int i = 0; i < maxFrames; i++) 
        free(pageArr[i].pageData);

    fclose(trace_file1);
    fclose(trace_file2);
}


int main(int argc, char **argv) {
    char *algorithm = argv[1];
    int maxFrames = atoi(argv[2]);
    int quantum = atoi(argv[3]);
    int maxTrace = MAX_TRACE;

    if(argc < 4) {
        printf("Usage of: %s <algorithm> <mem_size> <quantum> <max_ref>\n", argv[0]);
        exit(1);
    }

    if(strcmp(algorithm, "LRU") == 0) {
        LRUsimulation(algorithm, maxFrames, quantum, maxTrace);
    } else if(strcmp(algorithm, "SC") == 0) {
        SCsimulation(algorithm, maxFrames, quantum, maxTrace);
    } else {
        printf("Error: Wrong algorithm given!");
        exit(1);
    }

    return 0;
}
