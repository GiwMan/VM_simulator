#ifndef pageInfo_h_
#define pageInfo_h_

typedef struct PageInfo {
    int pid;
    unsigned int pageNum;
    char status;
} pageInfo;

void createPage(pageInfo *pinfo, int pid, unsigned int pNum, char status);


#endif