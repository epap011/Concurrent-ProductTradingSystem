//Todo
//Ta isws na min xreiazetai na xana ftiaxw threads gia to hash tables

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

#include "concurrent_dll.h"
#include "concurrent_ht.h"
#include "concurrent_stack.h"

#define DEBUG 0

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KWHT  "\x1B[37m"

/*---< Shared Variables >---*/
struct DoubleLinkedList *product_dll;            /* a double linked list   */
struct HTNode           ***consumer_hash_tables; /* a table of hash tables */
struct Stack            *stack;                  /* a stack                */

/*----< Function Declarations >---*/
void* playDifferentRoles(void* thread_data);

void* produceProducts(void* thread_producer_data);
void* sellProducts(void* thread_data);
void* detectBrokenProducts(void* thread_data);
void* repairBrokenProducts(void* thread_data);

void verifyProducedProducts(int N);
void verifySoldProducts(int N);
void verifyBrokenProducts(int N);
void verifyRepairedProducts(int N);

struct HTNode*** init_consumer_hash_tables(int consumers_size, int ht_size);
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
    int N, hash_table_size, hash_tables_number;

    printf("Enter N: ");
    scanf("%d", &N);

    /*----------< P r o d u c e     P r o d u c t s >----------*/
    /*-------------------------------------------------------*/
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
        pthread_create(&producer_thread_ids[i], NULL, produceProducts, (void *)thread_producer_data);
    } 

    for(int i = 0; i < N; i++) {
        pthread_join(producer_thread_ids[i], NULL);
    }

    pthread_barrier_destroy(&product_insertion_barrier);
    pthread_barrier_destroy(&verification_barrier);

    /*DEBUG PRINT*/
    if(DEBUG) printDLL(product_dll);

    /*----------< S e l l     P r o d u c t s >----------*/
    /*---------------------------------------------------*/
    hash_tables_number = N/3;
    hash_table_size    = 4*N;

    consumer_hash_tables = init_consumer_hash_tables(hash_tables_number, hash_table_size);
    producer_thread_ids  = (pthread_t*)malloc(sizeof(pthread_t) * N);

    pthread_barrier_init(&product_insertion_barrier, NULL, N);
    pthread_barrier_init(&verification_barrier, NULL, N);

    for(int i = 0; i < N; i++) {
        thread_producer_data = (struct ThreadProducerData*)malloc(sizeof(struct ThreadProducerData));
        thread_producer_data->product_insertion_barrier = &product_insertion_barrier;
        thread_producer_data->verification_barrier      = &verification_barrier;
        thread_producer_data->number_of_products        = N;
        thread_producer_data->thread_id                 = i;
        pthread_create(&producer_thread_ids[i], NULL, sellProducts, (void *)thread_producer_data);
    }

    for(int i = 0; i < N; i++) {
        pthread_join(producer_thread_ids[i], NULL);
    }

    pthread_barrier_destroy(&product_insertion_barrier);
    pthread_barrier_destroy(&verification_barrier);

    /*DEBUG PRINTS*/
    if(DEBUG) {
        printDLL(product_dll);
        printAllHashTables(hash_tables_number, hash_table_size);
    }

    /*----------< B r o k e n     P r o d u c t s >----------*/
    /*-------------------------------------------------------*/
    stack = createStack();

    pthread_barrier_init(&product_insertion_barrier, NULL, N);
    pthread_barrier_init(&verification_barrier, NULL, N);
    
    srand(time(NULL));

    for(int i = 0; i < N; i++) {
        thread_producer_data = (struct ThreadProducerData*)malloc(sizeof(struct ThreadProducerData));
        thread_producer_data->product_insertion_barrier = &product_insertion_barrier;
        thread_producer_data->verification_barrier      = &verification_barrier;
        thread_producer_data->number_of_products        = N;
        thread_producer_data->thread_id                 = i;
        pthread_create(&producer_thread_ids[i], NULL, detectBrokenProducts, (void *)thread_producer_data);
    }

    for(int i = 0; i < N; i++) {
        pthread_join(producer_thread_ids[i], NULL);
    }

    pthread_barrier_destroy(&product_insertion_barrier);
    pthread_barrier_destroy(&verification_barrier);

    /*DEBUG PRINT*/
    if(DEBUG) printStack(stack);

    /*----------< R e p a i r     P r o d u c t s >----------*/
    /*-------------------------------------------------------*/
    pthread_barrier_init(&product_insertion_barrier, NULL, N);
    pthread_barrier_init(&verification_barrier, NULL, N);

    for(int i = 0; i < N; i++) {
        thread_producer_data = (struct ThreadProducerData*)malloc(sizeof(struct ThreadProducerData));
        thread_producer_data->product_insertion_barrier = &product_insertion_barrier;
        thread_producer_data->verification_barrier      = &verification_barrier;
        thread_producer_data->number_of_products        = N;
        thread_producer_data->thread_id                 = i;
        pthread_create(&producer_thread_ids[i], NULL, repairBrokenProducts, (void *)thread_producer_data);
    }

    for(int i = 0; i < N; i++) {
        pthread_join(producer_thread_ids[i], NULL);
    }

    pthread_barrier_destroy(&product_insertion_barrier);
    pthread_barrier_destroy(&verification_barrier);

    /*DEBUG PRINTS*/
    if(DEBUG) {
        printStack(stack);
        printDLL(product_dll);
    }

}

void* playDifferentRoles(void* thread_data) {
    return NULL;
}

/*Creates a number (N) of products and insert them at the list*/
void* produceProducts(void* thread_producer_data) {
    int N   = ((struct ThreadProducerData*)thread_producer_data)->number_of_products;
    int tid = ((struct ThreadProducerData*)thread_producer_data)->thread_id;
    struct DLLNode *product;

    for(int i = 0; i < N; i++) {
        product = createDLLNode(i*N+tid);
        listInsert(product_dll, product);
    }

    pthread_barrier_wait(((struct ThreadProducerData*)thread_producer_data)->product_insertion_barrier);

    if(tid == 0) {
        verifyProducedProducts(N);
        pthread_barrier_wait(((struct ThreadProducerData*)thread_producer_data)->verification_barrier);
    } else {
        pthread_barrier_wait(((struct ThreadProducerData*)thread_producer_data)->verification_barrier);
    }
    return NULL;
}

void* sellProducts(void* thread_data) {
    int N   = ((struct ThreadProducerData*)thread_data)->number_of_products;
    int tid = ((struct ThreadProducerData*)thread_data)->thread_id;
    struct HTNode *product;
    int j = 0;
    for(int i = 0; i < N; i++) {
        product = createHTNode(tid*N+i);
        HTInsert(consumer_hash_tables[j%(N/3)], 4*N, product);
        listDelete(product_dll, product->productID);
        j++;
    }

    pthread_barrier_wait(((struct ThreadProducerData*)thread_data)->product_insertion_barrier);

    if(tid == 0) {
        verifySoldProducts(N);
        pthread_barrier_wait(((struct ThreadProducerData*)thread_data)->verification_barrier);
    } else {
        pthread_barrier_wait(((struct ThreadProducerData*)thread_data)->verification_barrier);
    }
    return NULL;
}

void* detectBrokenProducts(void* thread_data) {
    int N   = ((struct ThreadProducerData*)thread_data)->number_of_products;
    int tid = ((struct ThreadProducerData*)thread_data)->thread_id;
    int random_product_id;

    for(int i = 0; i < N/3; i++) {       
    
        random_product_id = rand() % (N*N);
        while(HTDelete(consumer_hash_tables[i%(N/3)], 4*N, random_product_id) == 0) {
            random_product_id = rand() % (N*N);
            printf("DETECT WHILE\n");
        }  
        push(stack, random_product_id);
    }

    pthread_barrier_wait(((struct ThreadProducerData*)thread_data)->product_insertion_barrier);

    if(tid == 0) {
        verifyBrokenProducts(N);
        pthread_barrier_wait(((struct ThreadProducerData*)thread_data)->verification_barrier);
    } else {
        pthread_barrier_wait(((struct ThreadProducerData*)thread_data)->verification_barrier);
    }
    return NULL;
}

void* repairBrokenProducts(void* thread_data) {
    int N   = ((struct ThreadProducerData*)thread_data)->number_of_products;
    int tid = ((struct ThreadProducerData*)thread_data)->thread_id;
    int broken_product_id;
    struct DLLNode *repaired_product;

    broken_product_id = pop(stack);
    while(broken_product_id != -2) { //-2 means empty_stack
        repaired_product = createDLLNode(broken_product_id);
        listInsert(product_dll, repaired_product);
        broken_product_id = pop(stack);
        printf("REPAIR WHILE - broken id: %d \n", broken_product_id);
    }

    pthread_barrier_wait(((struct ThreadProducerData*)thread_data)->product_insertion_barrier);
    if(tid == 0) {
        verifyRepairedProducts(N);
    }
    return NULL;
}

void verifyProducedProducts(int N) {
    long long int found_sum = DLLProductIdSum(product_dll);
    long long int expected_sum = ((N*N)*(N*N-1))/2;

    printf("\n-------------------< DLL Verification >-------------------\n");
    printf("  List sum check  (expected: %lld , found: %lld)\n\n", expected_sum, found_sum);

    int found_size = DLLSize(product_dll);
    printf("  List size check (expected: %d , found: %d)\n", N*N, found_size);
    
    if(expected_sum == found_sum && N*N == found_size) printf("\n\t%s  Produced Products Verification Succeded%s\n", KGRN, KWHT);
    else printf("\n\t%s  Produced Products Verification Failed%s\n", KRED, KWHT);

    printf("----------------------------------------------------------\n");
}

void verifySoldProducts(int N) {
    long long int total_sum = 0, expected_sum = ((N*N)*(N*N-1))/2;
    int ht_size_passes = 0, ht_size;

    printf("\n-------------------< HT Verification >--------------------\n");
    for(int i = 0; i < N/3; i++) {
        ht_size = HTSize(consumer_hash_tables[i], 4*N);
        printf("  HT[%d] size check (expected: %d , found: %d)\n", i, 3*N, ht_size);
        if(3*N == ht_size) ht_size_passes++;
    }

    for(int i = 0; i < N/3; i++) {
        total_sum += HTProductIDSum(consumer_hash_tables[i], 4*N);
    }
    printf("\n  HT sum  check (expected: %lld , found: %lld)\n", expected_sum, total_sum);
    if(ht_size_passes == N/3 && expected_sum == total_sum) printf("\n\t%s  Sold Products Verification Succeded%s\n", KGRN, KWHT);
    else printf("\n\t%s  Sold Products Verification Failed%s\n", KRED, KWHT);
    printf("----------------------------------------------------------\n");
}

void verifyBrokenProducts(int N) {
    int ht_size_passes = 0, ht_size = 0, actual_stack_products = 0, expected_stack_products = (N*N)/3;

    printf("\n-------------------< Stack Verification >--------------------\n");
    for(int i = 0; i < N/3; i++) {
        ht_size = HTSize(consumer_hash_tables[i], 4*N);
        printf("  HT[%d] size check (expected: %d , found: %d)\n", i, 2*N, ht_size);
        if(2*N == ht_size) ht_size_passes++;
    }

    actual_stack_products = getStackSize(stack);
    printf("\n  Stack size check (expected: %d , found: %d)\n", expected_stack_products, actual_stack_products);
    
    if(ht_size_passes == N/3 && expected_stack_products == actual_stack_products) 
        printf("\n\t%s  Broken Products Verification Succeded%s\n", KGRN, KWHT);
    else 
        printf("\n\t%s  Broken Products Verification Failed%s\n", KRED, KWHT);
    
    printf("----------------------------------------------------------\n");
}

void verifyRepairedProducts(int N) {
    int found_size = DLLSize(product_dll);
    
    printf("\n-------------------< DLL Verification >--------------------\n");
    printf("  List size check (expected: %d , found: %d)\n", (N*N)/3, found_size);
    
    if((N*N)/3 == found_size)
        printf("\n\t%s  Repaired Products Verification Succeded%s\n", KGRN, KWHT);
    else 
        printf("\n\t%s  Repaired Products Verification Failed%s\n", KRED, KWHT);
    
    printf("----------------------------------------------------------\n");
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
    printf("\n");
    for(int i = 0; i < hash_tables_number; i++) {
        printf("Hash Table %d\n", i);
        printHT(consumer_hash_tables[i], hash_table_size);
        printf("\n");
    }
}