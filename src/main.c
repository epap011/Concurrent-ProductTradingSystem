/**********************************/
/* @author Papageorgiou Efthymios */
/* @version v.1                   */
/**********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

#include "concurrent_dll.h"
#include "concurrent_ht.h"
#include "concurrent_stack.h"

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KWHT  "\x1B[37m"
#define KCYN  "\x1B[36m"
#define KMAG  "\x1B[35m"

/*---< Shared Variables >---*/
struct DoubleLinkedList *product_dll;            /* a double linked list   */
struct HTNode           ***consumer_hash_tables; /* a table of hash tables */
struct Stack            *stack;                  /* a stack                */

int DEBUG_MODE = 0;
pthread_mutex_t debug_lock;

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
void destroyAllHashTableMutexes(int hash_tables, int hash_table_size);

typedef struct ThreadData {
    pthread_barrier_t *product_insertion_barrier;
    pthread_barrier_t *verification_barrier;
    int number_of_products;
    int thread_id;
}ThreadData;

int main(int argc, char *argv[]) {
    pthread_t          *thread_ids;
    pthread_barrier_t   product_insertion_barrier, verification_barrier;
    ThreadData *thread_data;
    int N;

    if(argc == 2) {
        N = atoi(argv[1]);
        if(N % 3 != 0) {
            printf("%sN must be multiple of 3%s\n", KRED, KWHT);
            exit(1);
        }
        if(N <= 0) {
            printf("%sN must be a positive number%s\n", KRED, KWHT);
            exit(1);
        }
    }
    
    if(argc == 3) {
        N = atoi(argv[1]);
        if(N % 3 != 0) {
            printf("%sN must be multiple of 3%s\n", KRED, KWHT);
            exit(1);
        }
        if(N <= 0) {
            printf("%sN must be a positive number%s\n", KRED, KWHT);
            exit(1);
        }
        if(strcmp(argv[2], "--debug_mode") == 0) {
            DEBUG_MODE = 1;
            if(pthread_mutex_init(&debug_lock, NULL) != 0) {                                    
                perror("Error: pthread_mutex_init failed!");                                                       
                exit(1);                                                                    
            }
        }
        else {
            printf("%sAs a second argument you should provide \"--debug_mode\"%s\n", KRED, KWHT);
            exit(1);
        } 
    }

    srand(time(NULL));

    /*DLL CREATION*/
    product_dll = createDLL();

    /*HASH TABLE CREATION*/
    consumer_hash_tables = init_consumer_hash_tables(N/3, 4*N);

    /*STACK CREATION*/
    stack = createStack();

    thread_ids = (pthread_t*)malloc(sizeof(pthread_t) * N);
    pthread_barrier_init(&product_insertion_barrier, NULL, N);
    pthread_barrier_init(&verification_barrier, NULL, N);
    
    for(int i = 0; i < N; i++) {
        thread_data = (struct ThreadData *)malloc(sizeof(struct ThreadData));
        thread_data->product_insertion_barrier = &product_insertion_barrier;
        thread_data->verification_barrier      = &verification_barrier;
        thread_data->number_of_products        = N;
        thread_data->thread_id                 = i;
        pthread_create(&thread_ids[i], NULL, playDifferentRoles, (void *)thread_data);
    } 

    for(int i = 0; i < N; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    pthread_barrier_destroy(&product_insertion_barrier);
    pthread_barrier_destroy(&verification_barrier);

    destroyAllHashTableMutexes(N/3, 4*N);
}

void* playDifferentRoles(void* thread_data) {
    int N = ((struct ThreadData*)thread_data)->number_of_products;
    
    produceProducts(thread_data);

    if(DEBUG_MODE && pthread_mutex_trylock(&debug_lock) == 0) {
        printf("%s--------------< D E B U G    M E S S A G E >--------------%s\n", KMAG, KWHT);
        printf("[DLL after product production]\n");
        printDLL(product_dll);
        printf("%s----------------------------------------------------------%s\n", KMAG, KWHT);
        pthread_mutex_unlock(&debug_lock);
    }

    sellProducts(thread_data);

    if(DEBUG_MODE && pthread_mutex_trylock(&debug_lock) == 0) {
        printf("%s--------------< D E B U G    M E S S A G E >--------------%s\n", KMAG, KWHT);
        printf("*DLL after product sale*\n");
        printDLL(product_dll);

        printf("*Hash Tables after product sale*\n");
        printAllHashTables(N/3, 4*N);
        printf("%s----------------------------------------------------------%s\n", KMAG, KWHT);
        pthread_mutex_unlock(&debug_lock);
    }

    detectBrokenProducts(thread_data);

    if(DEBUG_MODE && pthread_mutex_trylock(&debug_lock) == 0) {
        printf("%s--------------< D E B U G    M E S S A G E >--------------%s\n", KMAG, KWHT);
        printf("*Hash Tables after product sale*\n");
        printAllHashTables(N/3, 4*N);

        printf("*Stack after broken product detection*\n");
        printStack(stack);
        printf("%s----------------------------------------------------------%s\n", KMAG, KWHT);
        pthread_mutex_unlock(&debug_lock);
    }

    repairBrokenProducts(thread_data);

    if(DEBUG_MODE && pthread_mutex_trylock(&debug_lock) == 0) {
        printf("%s--------------< D E B U G    M E S S A G E >--------------%s\n", KMAG, KWHT);
        printf("\t*Stack after repair of products*\n");
        printStack(stack);

        printf("\t*DLL after repair of products*\n");
        printDLL(product_dll);
        printf("%s----------------------------------------------------------%s\n", KMAG, KWHT);
        pthread_mutex_unlock(&debug_lock);
    }

    return NULL;
}

void* produceProducts(void* thread_producer_data) {
    int N   = ((struct ThreadData*)thread_producer_data)->number_of_products;
    int tid = ((struct ThreadData*)thread_producer_data)->thread_id;
    struct DLLNode *product;

    for(int i = 0; i < N; i++) {
        product = createDLLNode(i*N+tid);
        listInsert(product_dll, product);
    }

    pthread_barrier_wait(((struct ThreadData*)thread_producer_data)->product_insertion_barrier);

    if(tid == 0) {
        verifyProducedProducts(N);
        pthread_barrier_wait(((struct ThreadData*)thread_producer_data)->verification_barrier);
    } else {
        pthread_barrier_wait(((struct ThreadData*)thread_producer_data)->verification_barrier);
    }
    return NULL;
}

void* sellProducts(void* thread_data) {
    int N   = ((struct ThreadData*)thread_data)->number_of_products;
    int tid = ((struct ThreadData*)thread_data)->thread_id;
    int j = 0;

    for(int i = 0; i < N; i++) {
        HTInsert(consumer_hash_tables[j%(N/3)], 4*N, tid*N+i);
        listDelete(product_dll, tid*N+i);
        j++;
    }

    pthread_barrier_wait(((struct ThreadData*)thread_data)->product_insertion_barrier);

    if(tid == 0) {
        verifySoldProducts(N);
        pthread_barrier_wait(((struct ThreadData*)thread_data)->verification_barrier);
    } else {
        pthread_barrier_wait(((struct ThreadData*)thread_data)->verification_barrier);
    }
    return NULL;
}

void* detectBrokenProducts(void* thread_data) {
    int N   = ((struct ThreadData*)thread_data)->number_of_products;
    int tid = ((struct ThreadData*)thread_data)->thread_id;
    int random_product_id;
    int *detected_products;

    detected_products = (int*) malloc(sizeof(int) * (N*N));
    for(int i = 0; i < N*N; i++) {
        detected_products[i] = -1;
    }

    for(int i = 0; i < N/3; i++) {       
    
        random_product_id = rand() % (N*N);
        while(HTDelete(consumer_hash_tables[i%(N/3)], 4*N, random_product_id) == 0) {
            detected_products[random_product_id] = random_product_id;
            /*In order to achieve better performance, we ensure that the produced random key is not in a hash table
            via the detected_table which stores acts as a logger for already deleted keys.Thus, the overhead
            of HTDelete for nonsense keys is reduced.It is time consuming for other threads too, because of locks!*/
            do {
                random_product_id = rand() % (N*N);
            } while(detected_products[random_product_id] != -1);
            
        }
        push(stack, random_product_id);
    }

    pthread_barrier_wait(((struct ThreadData*)thread_data)->product_insertion_barrier);

    if(tid == 0) {
        verifyBrokenProducts(N);
        pthread_barrier_wait(((struct ThreadData*)thread_data)->verification_barrier);
    } else {
        pthread_barrier_wait(((struct ThreadData*)thread_data)->verification_barrier);
    }
    return NULL;
}

void* repairBrokenProducts(void* thread_data) {
    int N   = ((struct ThreadData*)thread_data)->number_of_products;
    int tid = ((struct ThreadData*)thread_data)->thread_id;
    int broken_product_id;
    struct DLLNode *repaired_product;

    broken_product_id = pop(stack);
    while(broken_product_id != -2) { //-2 means empty_stack
        repaired_product = createDLLNode(broken_product_id);
        listInsert(product_dll, repaired_product);
        broken_product_id = pop(stack);
    }

    pthread_barrier_wait(((struct ThreadData*)thread_data)->product_insertion_barrier);
    if(tid == 0) {
        verifyRepairedProducts(N);
    }
    return NULL;
}

void verifyProducedProducts(int N) {
    long long int found_sum = DLLProductIdSum(product_dll);
    long long int expected_sum = ((N*N)*(N*N-1))/2;

    printf("\n%s------------------------< DLL Verification >-----------------------%s\n", KCYN, KWHT);
    printf("  List sum check  (expected: %lld , found: %lld)\n\n", expected_sum, found_sum);

    int found_size = DLLSize(product_dll);
    printf("  List size check (expected: %d , found: %d)\n", N*N, found_size);
    
    if(expected_sum == found_sum && N*N == found_size) printf("\n\t%s  Produced Products Verification Succeded%s\n", KGRN, KWHT);
    else printf("\n\t%s  Produced Products Verification Failed%s\n", KRED, KWHT);

    printf("%s-------------------------------------------------------------------%s\n\n", KCYN, KWHT);
}

void verifySoldProducts(int N) {
    long int total_sum = 0, expected_sum = ((N*N)*(N*N-1))/2;
    int ht_size_passes = 0, ht_size;

    printf("\n%s-------------------< DLL -> HT Verification >----------------------%s\n", KCYN, KWHT);
    for(int i = 0; i < N/3; i++) {
        ht_size = HTSize(consumer_hash_tables[i], 4*N);
        printf("  HT[%d] size check (expected: %d , found: %d)\n", i, 3*N, ht_size);
        if(3*N == ht_size) ht_size_passes++;
    }

    for(int i = 0; i < N/3; i++) {
        total_sum += HTProductIDSum(consumer_hash_tables[i], 4*N);
    }
    printf("\n  HT sum  check (expected: %ld , found: %ld)\n", expected_sum, total_sum);
    if(ht_size_passes == N/3 && expected_sum == total_sum) printf("\n\t%s  Sold Products Verification Succeded%s\n", KGRN, KWHT);
    else printf("\n\t%s  Sold Products Verification Failed%s\n", KRED, KWHT);
    printf("%s-------------------------------------------------------------------%s\n\n", KCYN, KWHT);
}

void verifyBrokenProducts(int N) {
    int ht_size_passes = 0, ht_size = 0, actual_stack_products = 0, expected_stack_products = (N*N)/3;

    printf("\n%s-------------------< HT -> Stack Verification >--------------------%s\n", KCYN, KWHT);
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
    
    printf("%s-------------------------------------------------------------------%s\n\n", KCYN, KWHT);
}

void verifyRepairedProducts(int N) {
    int found_size = DLLSize(product_dll);
    
    printf("\n%s-------------------< STACK -> DLL Verification >-------------------%s\n", KCYN, KWHT);
    printf("  List size check (expected: %d , found: %d)\n", (N*N)/3, found_size);
    
    if((N*N)/3 == found_size)
        printf("\n\t%s  Repaired Products Verification Succeded%s\n", KGRN, KWHT);
    else 
        printf("\n\t%s  Repaired Products Verification Failed%s\n", KRED, KWHT);
    
    printf("%s-------------------------------------------------------------------%s\n\n", KCYN, KWHT);
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

void destroyAllHashTableMutexes(int hash_tables, int hash_table_size) {
    for(int i = 0; i < hash_tables; i++) {
        destroyAllMutexesAtHashTable(consumer_hash_tables[i], hash_table_size);
    }
}