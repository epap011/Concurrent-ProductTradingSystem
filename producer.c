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

/*Function Declarations*/
void listInsert(struct Product *product);
struct ProductList* createProductList();
struct Product* createProduct();
void *insertAllProductsAtList();
void listDelete();
void printProductList();

/*Shared Variables*/
volatile struct ProductList *product_list;
volatile struct Product     *product;

int main() {
    pthread_t* producer_thread_ids;
    int N;

    printf("Enter N: ");
    scanf("%d", &N);

    producer_thread_ids = (pthread_t*)malloc(sizeof(pthread_t) * N);
    product_list        = createProductList();

    for(int i = 0; i < N; i++) {
        pthread_create(&producer_thread_ids[i], NULL, insertAllProductsAtList, (void *)&N);
    }
    for(int i = 0; i < N; i++) {
        pthread_join(producer_thread_ids[i], NULL);
    }

    printProductList();

}

/*Creates a product list with head and tail pointing to a sentinel node(product)
  If no errors occured while creating the list or the product, the list will be returned*/
struct ProductList* createProductList() {
    struct ProductList *product_list;
    struct Product     *sentinel_product;

    product_list = (struct ProductList*)malloc(sizeof(struct ProductList));
    assert(product_list != NULL);

    sentinel_product   = createProduct(-1);
    product_list->head = sentinel_product;
    product_list->tail = sentinel_product;

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
void* insertAllProductsAtList(void* number_of_products) {
    struct Product *product;
    int N = *(int*)number_of_products;

    for(int i = 0; i < N; i++) {
        product = createProduct(i*N+gettid());
        listInsert(product);
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

    if(product_list->head->next == NULL) { /*first product case*/
        product_list->head->next = product;
        product_list->tail = product;
        product->next = NULL;
        product->prev = product_list->head;
        if(pthread_mutex_unlock(&product_list->head->lock) != 0) {
            perror("Error: pthread_mutex_unlock failed!");
            exit(1);
        }
        return;
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

void printProductList() {
    struct Product *curr;

    curr = product_list->head;
    while(curr != NULL) {
        if(curr->next != NULL) printf("[%d]=", curr->productID);
        else printf("[%d]", curr->productID);
        
        curr = curr->next;
    }
    printf("\n");
}