//TODO:
//1. Fix HT Insertion

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include "concurrent_dll.h"
#include "concurrent_ht.h"

/*Shared Variables*/
struct DoubleLinkedList *product_dll;
struct HTNode           ***consumer_hash_tables;

/*Function Declarations*/
void* insertAllProductsAtList(void* thread_producer_data);
void DLLVerification(int N);
void HTVerification(int N);
struct HTNode*** init_consumer_hash_tables(int consumers_size, int ht_size);
void* insertAllProductsAtHashTable(void* thread_data);
void printAllHashTables(int hash_tables_number, int hash_table_size);

typedef struct ThreadProducerData {
    pthread_barrier_t *product_insertion_barrier;
    pthread_barrier_t *verification_barrier;
    int number_of_products;
    int thread_id;
}ThreadProducerData;

int main() {
    pthread_t          *producer_thread_ids;
    pthread_barrier_t   product_insertion_barrier, verification_barrier;
    ThreadProducerData *thread_producer_data;
    int N;

    printf("Enter N: ");
    scanf("%d", &N);

    /*----------< P r o d u c e     P r o d u c t s >----------*/
    product_dll         = createDLL();
    producer_thread_ids = (pthread_t*)malloc(sizeof(pthread_t) * N);

    pthread_barrier_init(&product_insertion_barrier, NULL, N);
    pthread_barrier_init(&verification_barrier, NULL, N);

    /*Producer (threads) creation & dll product insertions*/
    for(int i = 0; i < N; i++) {
        thread_producer_data = (struct ThreadProducerData *)malloc(sizeof(struct ThreadProducerData));
        thread_producer_data->product_insertion_barrier = &product_insertion_barrier;
        thread_producer_data->verification_barrier      = &verification_barrier;
        thread_producer_data->number_of_products        = N;
        thread_producer_data->thread_id                 = i;
        pthread_create(&producer_thread_ids[i], NULL, insertAllProductsAtList, (void *)thread_producer_data);
    } 

    for(int i = 0; i < N; i++) {
        pthread_join(producer_thread_ids[i], NULL);
    }

    pthread_barrier_destroy(&product_insertion_barrier);
    pthread_barrier_destroy(&verification_barrier);

    /*----------< S e l l     P r o d u c t s >----------*/
    consumer_hash_tables = init_consumer_hash_tables(N/3, 4*N);
    producer_thread_ids  = (pthread_t*)malloc(sizeof(pthread_t) * N);

    pthread_barrier_init(&product_insertion_barrier, NULL, N);
    pthread_barrier_init(&verification_barrier, NULL, N);

    for(int i = 0; i < N; i++) {
        thread_producer_data = (struct ThreadProducerData*)malloc(sizeof(struct ThreadProducerData));
        thread_producer_data->product_insertion_barrier = &product_insertion_barrier;
        thread_producer_data->verification_barrier      = &verification_barrier;
        thread_producer_data->number_of_products        = N;
        thread_producer_data->thread_id                 = i;
        pthread_create(&producer_thread_ids[i], NULL, insertAllProductsAtHashTable, (void *)thread_producer_data);
    }

    for(int i = 0; i < N; i++) {
        pthread_join(producer_thread_ids[i], NULL);
    }

    pthread_barrier_destroy(&product_insertion_barrier);
    pthread_barrier_destroy(&verification_barrier);
}

/*Creates a number (N) of products and insert them at the list*/
void* insertAllProductsAtList(void* thread_producer_data) {
    int N   = ((struct ThreadProducerData*)thread_producer_data)->number_of_products;
    int tid = ((struct ThreadProducerData*)thread_producer_data)->thread_id;
    struct DLLNode *product;

    for(int i = 0; i < N; i++) {
        product = createDLLNode(i*N+tid);
        listInsert(product_dll, product);
    }

    pthread_barrier_wait(((struct ThreadProducerData*)thread_producer_data)->product_insertion_barrier);

    if(tid == 0) {
        DLLVerification(N);
        printDLL(product_dll);
        pthread_barrier_wait(((struct ThreadProducerData*)thread_producer_data)->verification_barrier);
    } else {
        pthread_barrier_wait(((struct ThreadProducerData*)thread_producer_data)->verification_barrier);
    }
}

void* insertAllProductsAtHashTable(void* thread_data) {
    int N   = ((struct ThreadProducerData*)thread_data)->number_of_products;
    int tid = ((struct ThreadProducerData*)thread_data)->thread_id;
    struct HTNode *product;
    int j = 0;
    for(int i = 0; i < N; i++) {
        product = createHTNode(tid*N+i);
        printf("thread %d put the item %d item at hash table %d\n", tid, tid*N+i, j%(N/3));
        HTInsert(consumer_hash_tables[j%(N/3)], 4*N, product);
        j++;
    }

    pthread_barrier_wait(((struct ThreadProducerData*)thread_data)->product_insertion_barrier);

    if(tid == 0) {
        HTVerification(N);
        printAllHashTables(N/3, 4*N);
        pthread_barrier_wait(((struct ThreadProducerData*)thread_data)->verification_barrier);
    } else {
        pthread_barrier_wait(((struct ThreadProducerData*)thread_data)->verification_barrier);
    }
}

void DLLVerification(int N) {
    int found_sum = DLLProductIdSum(product_dll);
    printf("List sum check  (expected: %d , found: %d)\n", ((N*N)*(N*N-1))/2, found_sum);

    int found_size = DLLSize(product_dll);
    printf("List size check (expected: %d , found: %d)\n", N*N, found_size);
}

void HTVerification(int N) {
    int total_sum = 0;

    for(int i = 0; i < N/3; i++) {
        printf("HT[%d] size check (expected: %d , found: %d)\n", i, 3*N, HTSize(consumer_hash_tables[i]));
    }

    for(int i = 0; i < N/3; i++) {
        total_sum += HTProductIDSum(consumer_hash_tables[i]);
    }
    printf("HT    sum  check (expected: %d , found: %d)\n", ((N*N)*(N*N-1))/2, total_sum);
}

struct HTNode*** init_consumer_hash_tables(int hash_tables_number, int ht_size) {
    struct HTNode ***consumer_hash_tables = (struct HTNode***)malloc(sizeof(struct HTNode**) * hash_tables_number);
    assert(consumer_hash_tables != NULL);
    
    for(int i = 0; i < hash_tables_number; i++) {
        consumer_hash_tables[i] = createHT(ht_size);
    }

    return consumer_hash_tables;
}

void printAllHashTables(int hash_tables_number, int hash_table_size) {
    for(int i = 0; i < hash_tables_number; i++) {
        printf("Hash Table %d\n", i);
        printHT(consumer_hash_tables[i], hash_table_size);
        printf("\n");
    }
}