#ifndef PAGETABLE_H_
#define PAGETABLE_H_

#include <stdbool.h>

#define MAX_BUCKETS 13

#define PAGE_EXISTS 1
#define FULL_TABLE 2
#define PAGE_INSERTED 3

/* data of bucket */
typedef struct PageListNode{
    int pid;
    unsigned int pageNum;
    char status;
    int count;
    bool secondChance;
    struct PageListNode *next;
} p_listNode;

/* buckets of hash table */
typedef struct Bucket{
    p_listNode *head;
    p_listNode *last;
} bucket;

typedef bucket *bucketPtr;

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


// typedef HPT* htablePtr;

// void initEntry(entryPtr *pageEntry, p_data data);

/* setter for data of linked list */ 
p_listNode* createListNode(  int pid, 
                            unsigned int addr, 
                            char mode, 
                            int c  );

/* list functions */
// void push(p_listNode** head,p_listNode **end,p_listNode data);
void push(p_listNode **head,p_listNode **last, p_listNode* data);

void printList(p_listNode *head);
// void append(p_listNode **end, p_listNode data);
void replaceVictim(HPT *hpt, int indexOfVictim,p_listNode *node);
// entryPtr deletePageNode(entryPtr *head, int key);

void deletePageList(p_listNode **head);


/* hash function */
unsigned int hashInt(   unsigned int n, 
                        int frames );

void initHPT(   HPT *hpt,
                char *algo, 
                int frames );

bool fullTable( int count,
                int maxFrames );

void insertPageHelper(  HPT* pt, 
                        p_listNode* data );

int insertHashTable(HPT *hpt,
                    p_listNode *node);

int pageExists(HPT hpt, int pageNum);

void updateHashTablePage(HPT *hpt, p_listNode node);

void deleteHashTablePage(HPT *hpt, p_listNode node);

void printStats(HPT hpt);

void deleteHPT(HPT *hpt);

void deleteBuckets(bucket **b, int size);

int findClockVictim(HPT hpt, p_listNode *node);

#endif