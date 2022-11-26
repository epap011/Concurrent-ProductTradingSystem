//TODO:
//Understand Barrier
//Thread id should be from gettid or for-loop counter?
//Why N=2 sometimes produce 2 items instead of 4 ?

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>

struct ProductList {
    struct Product *head;
    struct Product *tail;
};

struct Product {
    int productID;
    pthread_mutex_t lock;
    struct Product *next;
    struct Product *prev;
};

struct ThreadProducerData {
    pthread_barrier_t *barrier;
    int number_of_products;
    int thread_id;
};

/*Function Declarations*/
void listInsert(struct Product *product);
struct ProductList* createProductList();
struct Product* createProduct();
void *insertAllProductsAtList();
void listDelete();
void printProductList();
void productListSumCheck(int N);
void productListSizeCheck(int N);

/*Shared Variables*/
volatile struct ProductList *product_list;
volatile struct Product     *product;

int main() {
    pthread_t *producer_thread_ids;
    pthread_barrier_t barrier;
    struct ThreadProducerData *thread_producer_data;
    int N;

    printf("Enter N: ");
    scanf("%d", &N);

    producer_thread_ids = (pthread_t*)malloc(sizeof(pthread_t) * N);
    product_list        = createProductList();

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
    printProductList();
}

/*Creates a product list with head and tail pointing to a sentinel node(product)
  If no errors occured while creating the list or the product, the list will be returned*/
struct ProductList* createProductList() {
    struct ProductList *product_list;
    struct Product     *head_sentinel_product, *tail_sentinel_product;

    product_list = (struct ProductList*)malloc(sizeof(struct ProductList));
    assert(product_list != NULL);

    /*creating 2 sentinel nodes*/
    head_sentinel_product   = createProduct(-1);
    product_list->head      = head_sentinel_product;

    tail_sentinel_product   = createProduct(-1);
    product_list->tail      = tail_sentinel_product;

    head_sentinel_product->next = tail_sentinel_product;
    tail_sentinel_product->prev = head_sentinel_product;

    return product_list;
}

/*Creates a product and initializes it with the productID
  If no errors occured while creating the product, the product will be returned*/
struct Product* createProduct(int productID) {
    struct Product *product = (struct Product*)malloc(sizeof(struct Product));
    assert(product != NULL);
    if(pthread_mutex_init(&product->lock, NULL) != 0) {                                    
        perror("Error: pthread_mutex_init failed!");                                                       
        exit(1);                                                                    
    }

    product->productID = productID;
    product->next      = NULL;
    product->prev      = NULL;
    return product;
}

/*Creates a number (N) of products and insert them at the list*/
void* insertAllProductsAtList(void* thread_producer_data) {
    struct Product *product;
    int N   = ((struct ThreadProducerData*)thread_producer_data)->number_of_products;
    int tid = ((struct ThreadProducerData*)thread_producer_data)->thread_id;

    for(int i = 0; i < N; i++) {
        product = createProduct(i*N+tid);
        listInsert(product);
        printf("%d\n", product->productID);
    }

    pthread_barrier_wait(((struct ThreadProducerData*)thread_producer_data)->barrier);
    if(tid == 0) {
        productListSizeCheck(N);
        productListSumCheck(N);
    }
}

/*Insert a product at the product list to the right position*/
void listInsert(struct Product *product) {
    struct Product *prev, *curr;
    int result;

    if(pthread_mutex_lock(&product_list->head->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }
    
    prev = product_list->head;
    curr = prev->next;
    
    if(pthread_mutex_lock(&curr->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    while(curr->next != NULL && curr->productID < product->productID) {
        if(pthread_mutex_unlock(&prev->lock) != 0) {
            perror("Error: pthread_mutex_unlock failed!");
            exit(1);
        }
        prev = curr;
        curr = curr->next;
        if(pthread_mutex_lock(&curr->lock) != 0) {
            perror("Error: pthread_mutex_lock failed!");
            exit(1);
        }
    }

        if(curr->productID == product->productID) result = 0;
        else {
            product->next = curr;
            product->prev = prev;
            prev->next    = product;
            result        = 1;
        }

    if(pthread_mutex_unlock(&prev->lock) != 0) {
        perror("Error: pthread_mutex_unlock failed!");
        exit(1);
    }
    if(pthread_mutex_unlock(&curr->lock) != 0) {
        perror("Error: pthread_mutex_unlock failed!");
        exit(1);
    }
}

/*Deletes a product from the product list with a specific productID*/
void listDelete() {
    
}

void productListSizeCheck(int N) {
    struct Product *curr;
    int counter = 0;

    curr = product_list->head;
    while(curr != NULL) {
        counter++;        
        curr = curr->next;
    }
    printf("List size check (expected: %d , found: %d)\n", N*N, counter-2); /*don't count sentinel nodes*/
}

void productListSumCheck(int N) {
    struct Product *curr;
    int sum = 0;

    curr = product_list->head;
    while(curr != NULL) {
        sum += curr->productID;
        curr = curr->next;
    }
    printf("List size check (expected: %d , found: %d)\n", ((N*N)*(N*N-1))/2, sum+2); /*sentinel nodes sum should be removed (-2 + 2 == 0)*/
}

void printProductList() {
    struct Product *curr;

    printf("\nHead: %d\t", product_list->head->productID);
    printf("Tail: %d\n", product_list->tail->productID);

    curr = product_list->head;
    while(curr != NULL) {
        if(curr->next != NULL) printf("[%d]=", curr->productID);
        else printf("[%d]", curr->productID);
        
        curr = curr->next;
    }
    printf("\n");
}