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

int HTInsert(struct HTNode** hash_table, int hash_table_size, struct HTNode* bucket);
int HTDelete(struct HTNode** hash_table, int hash_table_size, int productID);
int HTSearch(struct HTNode** hash_table, int hash_table_size, int productID);
int HTSize(struct HTNode** hash_table, int size);
long int HTProductIDSum(struct HTNode** hash_table, int size);
void printHT(struct HTNode** hash_table, int size);
struct HTNode** createHT(int size);
struct HTNode* createHTNode(int productID);

#endif /*CONCURRENT_HT_H*/