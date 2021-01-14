#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>


#include "../include/PageTable.h"
#include "../include/Queue.h"

#define PAGE_SIZE 4096
#define MAX_TRACE 50

int main(int argc, char **argv) {
    
    char *algorithm = argv[1];
    int maxFrames = atoi(argv[2]);
    int quantum = atoi(argv[3]);
    int temp = atoi(argv[3]); // this is a temp quantum 
    int address = 0; // holds the address read from the trace files
    char status = 0; // holds 'R' or 'W' 
    bool change = false;

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

    Queue q; // struct for LRU implementation

    if(strcmp(algorithm,"LRU") == 0) {
        initQueue(&q);
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
    int count1 = 0, count2 = 0;
    pid_t pid = 0;
    while(count1 + count2 < MAX_TRACE) { 

        if(!change) {
            traceFile = trace_file1;
            pid = 1;
            count = count1;
        } else {
            traceFile = trace_file2;
            pid = 2;
            count = count2;
        }

        fscanf(traceFile,"%x %c", &address, &status);
        // printf("%d %c\n",address >> 12, status);

        // create page list node 
        p_node = createListNode( pid,  
                                 address >> 12,
                                 status,
                                 count );
        // printf("Pid = %d count = %d, page = %d\n",p_node->pid, p_node->count, p_node->pageNum);

        // insert in hashed page table according
        // to pid number
        // printf("pid = %d\n", pid);
        if(pid == 1) {
            insertPage( &hpt_p1, p_node);
            count1++;
        } else {
            insertPage( &hpt_p2, p_node);
            count2++;
        }
        // printf("quantum = %d\n", quantum);
        // increase time counter    
        
        quantum--;

        // read from the other trace file
        if(quantum == 0) {
            change = !change;
            quantum = temp;
        }
    }

    // printf("count1 = %d, count2 = %d\n", count1, count2);

    printStats(hpt_p1);
    printStats(hpt_p2);
   
    // free
    deleteHPT(&hpt_p1);
    deleteHPT(&hpt_p2);

    fclose(trace_file1);
    fclose(trace_file2);
    
    // deallocate Hashed Page Table structure
    // for(int i = 0; i < ptable->frames; i++)
    //     if(ptable->arr[i]) {
    //         // delete list
    //         deltePageList(&ptable->arr[i]);
    //     }
}