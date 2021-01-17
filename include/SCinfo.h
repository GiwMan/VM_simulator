#ifndef secondchance_h_
#define secondchance_h_

// #include "./HashedPageTable.h"
#include "./PageInfo.h"

typedef struct SecondChance {
    pageInfo *pageData;
    int bitRef;
} SCinfo;

void createSCinfo(SCinfo *scinfo, pageInfo *data);

int getPage(SCinfo *pageArr, int page, int size);

// void setSCinfo(SCinfo *scinfoArr, SCinfo *scinfo);

// void SC_simulation(HPT *h1, HPT *h2, int maxFrames,int q, int maxTraces);

#endif