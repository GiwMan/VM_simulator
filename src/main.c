#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>


#include "../include/PageTable.h"
#include "../include/Queue.h"

#define PAGE_SIZE 4096
#define MAX_TRACE 1000

void LRUsimulation(p_listNode *pnode,HPT *hpt_p1,HPT *hpt_p2,Queue *q_lru, int maxFrames);


int main(int argc, char **argv) {
    
    char *algorithm = argv[1];
    int maxFrames = atoi(argv[2]);
    int quantum = atoi(argv[3]);
    int temp = atoi(argv[3]); // this is a temp quantum 
    int address = 0; // holds the address read from the trace files
    char status = 0; // holds 'R' or 'W' 
    bool change = false;
    bool LRU = false;

    FILE *traceFile,*trace_file1, *trace_file2; // for opening files
    traceFile = NULL; // set to NULL

    if(argc < 4) {
        printf("Usage of: %s <algorithm> <mem_size> <quantum> <max_ref>\n", argv[0]);
        exit(1);
    }

    /**
     * Files opening 
     */
    if((trace_file1 = fopen("./files/bzip.trace","r")) == NULL) {
        perror("fopen");
        exit(-1);
    }

    if((trace_file2 = fopen("./files/gcc.trace","r")) == NULL) {
        perror("fopen");
        exit(-1);
    }

    /**
     * Page Table creation
     * Page Entry init
     */
    // bucket *hpt;
    HPT hpt_p1; // hashed page table for process_1
    HPT hpt_p2; // hashed page table for process_2
    p_listNode *p_node; //  this node is to be inserted in hash tables

    Queue q_lru; // struct for LRU implementation

    if(strcmp(algorithm,"LRU") == 0) {
        initQueue(&q_lru);
        LRU = true;
    } else if(strcmp(algorithm,"SC") == 0) {
        ;
    } else {
        printf("Error. Wrong algorithm given!\n");
        exit(-1);
    }

    // initialization of hash tables
    initHPT(&hpt_p1,
            algorithm, 
            maxFrames / 2 );

    initHPT(&hpt_p2,
            algorithm, 
            maxFrames / 2);
      
    /**
     * Start reading traces
     */

    int count = 0;
    pid_t pid = 0;
    while(count < MAX_TRACE) { 

        if(!change) {
            traceFile = trace_file1;
            pid = 1;
            // count = count1;
        } else {
            traceFile = trace_file2;
            pid = 2;
            // count = count2;
        }

        fscanf(traceFile,"%x %c", &address, &status);
        // printf("%d %c\n",address >> 12, status);

        // create page list node 
        p_node = createListNode( pid,  
                                 address >> 12,
                                 status,
                                 count );
        // printf("Pid = %d count = %d, page = %d\n",p_node->pid, p_node->count, p_node->pageNum);

        if(LRU) {
            LRUsimulation(p_node, &hpt_p1, &hpt_p2, &q_lru, maxFrames);
        } else {

        }

        // increase time inserted
        count++;
              
        quantum--;
        // read from the other trace file
        if(quantum == 0) {
            change = !change;
            quantum = temp;
        }
    }

    printStats(hpt_p1);
    printStats(hpt_p2);
   
    // free
    deleteHPT(&hpt_p1);
    deleteHPT(&hpt_p2);

    deleteQueue(&q_lru);

    fclose(trace_file1);
    fclose(trace_file2);
}

// void func(HPT *hpt1, HPT *hpt2, Queue *q_lru, int maxFrames, p_listNode *p_node) {
//     int checkHashedTable; // insertion check in hashed table

//     if(maxFramesReached(*q_lru, maxFrames) == true) {

//         // we need to delete victim
//         Qnode *front = getFront(*q_lru);

//         // insert new node in hash table
//         if(pageExists(*hpt1, p_node->pageNum) == 1) {
//             // update hash table
//             updateHashTablePage(hpt1, *p_node);

//             // update queue
//             updateQueue(q_lru, p_node);

               
//         } else {
//             insertPageHelper(hpt1, p_node);

//             if(front->pageData->pid == 1) {
//             // delete node from hash table
                
//                 deleteHashTablePage(hpt1, *front->pageData);
//             } else {
//                 deleteHashTablePage(hpt2, *front->pageData);
//             }
//                 // enqueue new node
//                 enQueue(q_lru, p_node);

//                 // delete node from queue
//                 deQueue(q_lru);
//         }
//     } else {
//         // there is space to insert node
//         checkHashedTable = insertHashTable(hpt1, p_node);
//         if(checkHashedTable == PAGE_EXISTS) {
//             // printf("page %d exists\n", p_node->pageNum);
//             // update hash table
//             updateHashTablePage(hpt1, *p_node);

//             // update LRU queue
//             updateQueue(q_lru, p_node);

//         } else {
//             // insert new page in our structure 
//             // of LRU or SC
//             enQueue(q_lru, p_node);
//         }
//     }
// }

void LRUsimulation(p_listNode *p_node,HPT *hpt_p1,HPT *hpt_p2,Queue *q_lru, int maxFrames) {
    int pid = p_node->pid;
    int checkHashedTable = -1;

    if(pid == 1) {
        // check if there is empty slot to insert new page
        // both in HPT and queue
        
        
        if(maxFramesReached(*q_lru, maxFrames) == true) {
            
            // we need to delete victim
            Qnode *front = getFront(*q_lru);
            // printf("Deleting page %d, pid %d count = %d\n",front->pageData->pageNum, front->pageData->pid, front->pageData->count );

            // insert the new node in hash table with pid
            if(pageExists(*hpt_p1, p_node->pageNum) == 1) {
                // update hash table
                updateHashTablePage(hpt_p1, *p_node);

                // update queue
                updateQueue(q_lru, p_node);

               
            } else {
                insertPageHelper(hpt_p1, p_node);

                if(front->pageData->pid == 1) {
                // delete node from hash table
                
                    deleteHashTablePage(hpt_p1, *front->pageData);
                } else {
                    deleteHashTablePage(hpt_p2, *front->pageData);
                }
                    // enqueue new node
                    enQueue(q_lru, p_node);

                    // delete node from queue
                    deQueue(q_lru);
            }
        } else {
                
            checkHashedTable = insertHashTable(hpt_p1, p_node);
            if(checkHashedTable == PAGE_EXISTS) {
                // printf("page %d exists\n", p_node->pageNum);
                // update hash table
                updateHashTablePage(hpt_p1, *p_node);

                // update LRU queue
                updateQueue(q_lru, p_node);

            } else {
                // insert new page in our structure 
                // of LRU or SC
                enQueue(q_lru, p_node);
                    // printf("page %d inserted\n", p_node->pageNum);
                // printQueue(q_lru);
            }
        }
        // printf("\n\n");
        // printStats(*hpt_p1);
        // printQueue(*q_lru);
        //         printf("\n\n");

    } else {
        // for pid = 2
        if(maxFramesReached(*q_lru, maxFrames) == true) {
            // we need to delete victim
            
            // we need to delete victim
            Qnode *front = getFront(*q_lru);
            // printf("Deleting page %d, pid %d count %d\n",front->pageData->pageNum, front->pageData->pid, front->pageData->count );

            // insert the new node in hash table with pid
            if(pageExists(*hpt_p2, p_node->pageNum) == 1) {
                // printf("page %d exists 2\n", p_node->pageNum);

                updateHashTablePage(hpt_p2, *p_node);
                  // update queue
                updateQueue(q_lru, p_node);
            } else {
                insertPageHelper(hpt_p2, p_node);

                if(front->pageData->pid == 1) {
                // delete node from hash table
                    deleteHashTablePage(hpt_p1, *front->pageData);
                } else {
                    deleteHashTablePage(hpt_p2, *front->pageData);
                }
                    // enqueue new node
                    enQueue(q_lru, p_node);

                    // delete node from queue
                    deQueue(q_lru);
                }

                
        } else {
                
            checkHashedTable = insertHashTable(hpt_p2, p_node);
            if(checkHashedTable == PAGE_EXISTS) {
                // printf("page %d exists\n", p_node->pageNum);
                // update hash table
                updateHashTablePage(hpt_p2, *p_node);

                // update LRU queue
                updateQueue(q_lru, p_node);
            } else {
                // insert new page in our structure 
                // of LRU or SC
                enQueue(q_lru, p_node);
                    // printf("page %d inserted\n", p_node->pageNum);
                // printQueue(q_lru);
            }
        }
        // printf("\n\n");
        // printStats(*hpt_p1);
        // printQueue(*q_lru);
        //         printf("\n\n");

    }
}
