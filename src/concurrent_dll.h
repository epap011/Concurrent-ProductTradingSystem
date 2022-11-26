#ifndef CONCURRENT_DLL_H
#define CONCURRENT_DLL_H
#include <pthread.h>

struct DoubleLinkedList {
    struct DLLNode *head;
    struct DLLNode *tail;
};

struct DLLNode {
    int productID;
    pthread_mutex_t lock;
    struct DLLNode *next;
    struct DLLNode *prev;
};

struct DoubleLinkedList* createDLL();
struct DLLNode* createDLLNode(int productID);
int listInsert(struct DoubleLinkedList *dll, struct DLLNode *node);
int listDelete(struct DoubleLinkedList *dll, int productID);
int DLLSize(struct DoubleLinkedList *dll);
int DLLProductIdSum(struct DoubleLinkedList *dll);
void printDLL(struct DoubleLinkedList *dll);

#endif /*CONCURRENT_DLL_H*/