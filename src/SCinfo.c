#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/SCinfo.h"

void createSCinfo(SCinfo *scinfo, unsigned int page, int pid, char mode) {
    scinfo->pageNum = page;
    scinfo->pid = pid;
    scinfo->status = mode;
    scinfo->bitRef = 0;
}

int searchByPage(SCinfo **scinfoArr, SCinfo scinfo, int size) {
    for(int i = 0; i < size; i++) {
        
        if(scinfoArr[i]) {
            if(scinfoArr[i]->pageNum == scinfo.pageNum && 
                scinfoArr[i]->pid == scinfo.pid )
            {
                return i;
            }    
        }
    }

    return -1;
}

void SC_simulation(HPT *h1, HPT *h2, int maxFrames,int q, int maxTraces) {
    bool change = false;
    FILE *traceFile = NULL, *trace_file1, *trace_file2;
    int pid = 0; // pid number
    int address = 0; // holds the address for trace files
    char status = ' ';

    // create an array of SCinfo
    SCinfo **scinfoArr = (SCinfo **) malloc(sizeof(struct SCinfo *) * maxFrames);
    for(int i = 0; i < maxFrames; i++) {
        scinfoArr[i] = (SCinfo *) malloc(sizeof(struct SCinfo*));
        scinfoArr[i]->pageNum = -1;
        scinfoArr[i]->pid = -1;
        scinfoArr[i]->status = 0;
        scinfoArr[i]->bitRef = 0;
    }
    

    if((trace_file1 = fopen("./files/bzip.trace","r")) == NULL) {
        perror("fopen");
        exit(-1);
    }

    if((trace_file2 = fopen("./files/gcc.trace","r")) == NULL) {
        perror("fopen");
        exit(-1);
    }

    SCinfo scinfo; // create an info node
    int count = 0;
    int victimptr = 0;
    for(int i = 0; i < maxTraces; i++) {

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
        // create scinfo with some data
        createSCinfo(&scinfo, address >> 12, pid, status);
        

        if(pid == 1) {

            printf("\nFor %d:", scinfo.pageNum);
            if(pageExists(*h1, scinfo.pageNum) == 1) {
                printf("No page fault!");

                int pos = searchByPage(scinfoArr, scinfo, maxFrames);

                if(pos == -1) {
                    printf("page doesn't exist!\n");
                    exit(-1);
                } else {
                    // set bitRef
                    scinfoArr[pos]->bitRef = 1;
                }

            } else {
                h1->pageFaults++;

                // scinfoArr[victimptr] = &scinfo;

                if(scinfoArr[victimptr]->bitRef == 1) {
                // https://www.programming9.com/programs/c-programs/285-page-replacement-programs-in-c
                    do {
                        // set bitRef back to 0
                        scinfoArr[victimptr]->bitRef = 0;
                        victimptr++;
                        if(victimptr == maxFrames)
                            victimptr = 0;

                        } while(scinfoArr[victimptr]->bitRef != 0);
                }

                if(scinfoArr[victimptr]->bitRef == 0) {
                    scinfoArr[victimptr]->pageNum = scinfo.pageNum;
                    scinfoArr[victimptr]->pid = scinfo.pid;
                    scinfoArr[victimptr]->status = scinfo.status;
                    scinfoArr[victimptr]->bitRef = 1;

                    victimptr++;

                    printf("edw page = %d\n", scinfoArr[victimptr-1]->pageNum);
                }
                
            }

            if(victimptr == maxFrames)
                victimptr = 0;
        }

    }   
    printf("\n\n");
    for(int i = 0; i < maxFrames; i++) {
        printf("Page %d: \n", scinfoArr[i]->pageNum);        
    }

}


