#ifndef secondchance_h_
#define secondchance_h_

#include "./PageTable.h"

typedef struct SecondChance {
    int pid;
    unsigned int pageNum;
    char status;
    int bitRef;
} SCinfo;



void setSCinfo(SCinfo *scinfoArr, SCinfo *scinfo);

void SC_simulation(HPT *h1, HPT *h2, int maxFrames,int q, int maxTraces);

#endif