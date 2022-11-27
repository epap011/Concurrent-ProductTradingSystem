#ifndef CONCURRENT_HT_H
#define CONCURRENT_HT_H
#include <pthread.h>

struct HTNode {
    int productID;
    pthread_mutex_t lock;
};

int HTInsert(struct HTNode** hash_table, int hash_table_size, struct HTNode* bucket);
int HTDelete();
int HTSearch();
int HTSize(struct HTNode** hash_table);
int HTProductIDSum(struct HTNode** hash_table);
void printHT(struct HTNode** hash_table, int size);
struct HTNode** createHT(int size);
struct HTNode* createHTNode(int productID);

#endif /*CONCURRENT_HT_H*/