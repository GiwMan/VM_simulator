#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/PageInfo.h"

void createPage(pageInfo *pinfo, int pid, unsigned int pNum, char status) {
    pinfo->pid = pid;
    pinfo->pageNum = pNum;
    pinfo->status = status;
}
