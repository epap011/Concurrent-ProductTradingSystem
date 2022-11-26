#ifndef CONCURRENT_HT_H
#define CONCURRENT_HT_H
#include <pthread.h>

struct HTNode {
    int productID;
    pthread_mutex_t lock;
};

int HTInsert();
int HTDelete();
int HTSearch();
void printHT();
struct HTNode** createHT(int size);
struct HTNode* createHTNode(int productID);

#endif /*CONCURRENT_HT_H*/