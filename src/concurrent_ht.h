/**********************************/
/* @author Papageorgiou Efthymios */
/* @version v.1                   */
/**********************************/

#ifndef CONCURRENT_HT_H
#define CONCURRENT_HT_H
#include <pthread.h>

struct HTNode {
    int productID;
    pthread_mutex_t lock;
};

int HTInsert(struct HTNode** hash_table, int hash_table_size, int key);
int HTDelete(struct HTNode** hash_table, int hash_table_size, int key);
int HTSearch(struct HTNode** hash_table, int hash_table_size, int key);
int HTSize(struct HTNode** hash_table, int size);
long int HTProductIDSum(struct HTNode** hash_table, int size);
void printHT(struct HTNode** hash_table, int size);
struct HTNode** createHT(int size);
void destroyAllMutexesAtHashTable(struct HTNode** hash_table, int size);

#endif /*CONCURRENT_HT_H*/