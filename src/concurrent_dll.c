/**********************************/
/* @author Papageorgiou Efthymios */
/* @version v.1                   */
/**********************************/

#include "concurrent_dll.h"
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/*Creates a product list with head and tail pointing to a sentinel node(product)
If no errors occured while creating the list or the product, the list will be returned*/
struct DoubleLinkedList* createDLL() {
    struct DoubleLinkedList *dll;
    struct DLLNode *head_sentinel_node, *tail_sentinel_node;

    dll = (struct DoubleLinkedList*)malloc(sizeof(struct DoubleLinkedList));
    assert(dll != NULL);

    /*creating 2 sentinel nodes*/
    head_sentinel_node = createDLLNode(-1);
    dll->head          = head_sentinel_node;

    tail_sentinel_node = createDLLNode(-1);
    dll->tail          = tail_sentinel_node;

    head_sentinel_node->next = tail_sentinel_node;
    tail_sentinel_node->prev = head_sentinel_node;

    return dll;
}

/*Creates a product and initializes it with the productID
If no errors occured while creating the product, the product will be returned*/
struct DLLNode* createDLLNode(int productID) {
    struct DLLNode *node = (struct DLLNode*)malloc(sizeof(struct DLLNode));
    assert(node != NULL);

    if(pthread_mutex_init(&node->lock, NULL) != 0) {                                    
        perror("Error: pthread_mutex_init failed!");                                                       
        exit(1);                                                                    
    }

    node->productID = productID;
    node->next      = NULL;
    node->prev      = NULL;

    return node;
}

/*Insert a product at the product list to the right position*/
int listInsert(struct DoubleLinkedList *dll, struct DLLNode *node) {
    struct DLLNode *prev, *curr;
    int result;

    if(pthread_mutex_lock(&dll->head->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }
    
    prev = dll->head;
    curr = prev->next;
    
    if(pthread_mutex_lock(&curr->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    while(curr->next != NULL && curr->productID < node->productID) {
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

    if(curr->productID == node->productID) result = 0;
    else {
        node->next = curr;
        node->prev = prev;
        prev->next = node;
        curr->prev = node;
        result     = 1;
    }

    if(pthread_mutex_unlock(&prev->lock) != 0) {
        perror("Error: pthread_mutex_unlock failed!");
        exit(1);
    }
    if(pthread_mutex_unlock(&curr->lock) != 0) {
        perror("Error: pthread_mutex_unlock failed!");
        exit(1);
    }
    return result;
}

int listDelete(struct DoubleLinkedList *dll, int productID) {
    struct DLLNode *prev, *curr;
    int result;

    if(pthread_mutex_lock(&dll->head->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }
    
    prev = dll->head;
    curr = prev->next;
    
    if(pthread_mutex_lock(&curr->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    while(curr->next != NULL && curr->productID < productID) {
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

    if(curr->productID == productID) {
        prev->next       = curr->next;
        curr->next->prev = prev;
        result           = 1;
    }
    else {
        result = 0;
    }

    if(pthread_mutex_unlock(&prev->lock) != 0) {
        perror("Error: pthread_mutex_unlock failed!");
        exit(1);
    }
    if(pthread_mutex_unlock(&curr->lock) != 0) {
        perror("Error: pthread_mutex_unlock failed!");
        exit(1);
    }
    return result;
}

int listSearch(struct DoubleLinkedList *dll, int productID) {
    struct DLLNode *prev, *curr;
    int result;

    if(pthread_mutex_lock(&dll->head->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }
    
    prev = dll->head;
    curr = prev->next;
    
    if(pthread_mutex_lock(&curr->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    while(curr->next != NULL && curr->productID < productID) {
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

    if(curr->productID == productID) result = 1;
    else result = 0;

    if(pthread_mutex_unlock(&prev->lock) != 0) {
        perror("Error: pthread_mutex_unlock failed!");
        exit(1);
    }
    if(pthread_mutex_unlock(&curr->lock) != 0) {
        perror("Error: pthread_mutex_unlock failed!");
        exit(1);
    }
    return result;
}

int DLLSize(struct DoubleLinkedList *dll) {
    struct DLLNode *curr;
    int counter = 0;

    curr = dll->head;
    while(curr != NULL) {
        counter++;        
        curr = curr->next;
    }
    return counter-2;
}

long long int DLLProductIdSum(struct DoubleLinkedList *dll) {
    struct DLLNode *curr;
    long long int sum = 0;

    curr = dll->head;
    while(curr != NULL) {
        sum += curr->productID;
        curr = curr->next;
    }
    return sum+2;
}

void printDLL(struct DoubleLinkedList *dll) {
    struct DLLNode *curr;

    printf("\nDLL: ");
    curr = dll->head;
    while(curr != NULL) {
        if(curr->next != NULL) printf("[%d]=", curr->productID);
        else printf("[%d]", curr->productID);
        
        curr = curr->next;
    }
    printf("\n");
}