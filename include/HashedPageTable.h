#ifndef Hashed_Page_H_
#define Hashed_Page_H_

#include <stdbool.h>

#include "./PageInfo.h"
// #include "./Queue.h"

#define MAX_BUCKETS 13

#define PAGE_EXISTS 1
#define FULL_TABLE 2
#define PAGE_INSERTED 3

/* data of bucket */
typedef struct PageListNode{
    struct PageInfo *pageInfo;
    int count;
    struct PageListNode *next;
} p_listNode;

/* buckets of hash table */
typedef struct Bucket{
    p_listNode *head;
    p_listNode *last;
} bucket;

/* hash table struct */
typedef struct HashedPageTable {
    int pageFaults;
    int nReads;
    int nWrites;
    int frames;
    int diskWrites;
    char *algorithm;
    int size;
    bucket **arr;
} HPT;

unsigned int hashInt( unsigned int n, int frames );

p_listNode *createPageListNode(unsigned int page, int pid, char status, int count);

void initHPT(HPT *hpt, char *algo, int frames);

void push(p_listNode **head,p_listNode **last, p_listNode* data);

void insertPageHelper(HPT *hpt, p_listNode *node);

// void insertPageByPageInfo(HPT *hpt, pageInfo *pinfo);

int pageExists(HPT hpt, unsigned int pageNum);

void updateHashTablePage(HPT *hpt, unsigned int page, char status, int count);

void deleteHashTablePage(HPT *hpt, p_listNode *node);

void deleteByPage(HPT *hpt, unsigned int page, char status);

void printList(p_listNode *node);

void printStats(HPT hpt);

void deleteHPT(HPT *hpt);

void deleteHPT_SC(HPT *hpt);

#endif