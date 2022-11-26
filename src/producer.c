#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include "concurrent_dll.h"

/*Shared Variables*/
struct DoubleLinkedList *product_list;

/*Function Declarations*/
void* insertAllProductsAtList(void* thread_producer_data);
void DLLValidation(int N);

typedef struct ThreadProducerData {
    pthread_barrier_t *barrier;
    int number_of_products;
    int thread_id;
}ThreadProducerData;

int main() {
    pthread_t          *producer_thread_ids;
    pthread_barrier_t   barrier;
    ThreadProducerData *thread_producer_data;
    int N;

    printf("Enter N: ");
    scanf("%d", &N);

    product_list        = createDLL();
    producer_thread_ids = (pthread_t*)malloc(sizeof(pthread_t) * N);

    pthread_barrier_init(&barrier, NULL, N);

    for(int i = 0; i < N; i++) {
        thread_producer_data = (struct ThreadProducerData *)malloc(sizeof(struct ThreadProducerData));
        thread_producer_data->barrier = &barrier;
        thread_producer_data->number_of_products = N;
        thread_producer_data->thread_id = i;
        pthread_create(&producer_thread_ids[i], NULL, insertAllProductsAtList, (void *)thread_producer_data);
    }

    for(int i = 0; i < N; i++) {
        pthread_join(producer_thread_ids[i], NULL);
    }

    pthread_barrier_destroy(&barrier);
}

/*Creates a number (N) of products and insert them at the list*/
void* insertAllProductsAtList(void* thread_producer_data) {
    int N   = ((struct ThreadProducerData*)thread_producer_data)->number_of_products;
    int tid = ((struct ThreadProducerData*)thread_producer_data)->thread_id;
    struct DLLNode *product;

    for(int i = 0; i < N; i++) {
        product = createDLLNode(i*N+tid);
        listInsert(product_list, product);
    }

    pthread_barrier_wait(((struct ThreadProducerData*)thread_producer_data)->barrier);
    if(tid == 0) {
        DLLValidation(N);
        printDLL(product_list);
    }
}

void DLLValidation(int N) {
    int found_sum = DLLProductIdSum(product_list);
    printf("List sum check  (expected: %d , found: %d)\n", ((N*N)*(N*N-1))/2, found_sum);

    int found_size = DLLSize(product_list);
    printf("List size check (expected: %d , found: %d)\n", N*N, found_size);
}