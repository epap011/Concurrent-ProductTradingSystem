/**********************************/
/* @author Papageorgiou Efthymios */
/* @version v.1                   */
/**********************************/

#include "concurrent_ht.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void swap(struct HTNode** hash_table, int index_1, int index_2);
int hash_function(int key, int table_size, int collision_number);
int hash_1(int key, int table_size);
int hash_2(int key, int table_size);

struct HTNode** createHT(int size) {
    struct HTNode **hash_table = (struct HTNode**)malloc(sizeof(struct HTNode*) * size);
    assert(hash_table != NULL);

    for(int i = 0; i < size; i++) {
        hash_table[i] = (struct HTNode*)malloc(sizeof(struct HTNode)); /*Dummy ht nodes*/
        hash_table[i]->productID = -1;
        if(pthread_mutex_init(&hash_table[i]->lock, NULL) != 0) {                                    
            perror("Error: pthread_mutex_init failed!");                                                       
            exit(1);                                                                    
        }
    }

    return hash_table;
}

struct HTNode* createHTNode(int productID) {
    struct HTNode* ht_node = (struct HTNode*)malloc(sizeof(struct HTNode));
    assert(ht_node != NULL);
    ht_node->productID = productID;
    
    return ht_node;
}

int HTInsert(struct HTNode** hash_table, int hash_table_size, struct HTNode* ht_node) {
    int prev = -1, curr, collision_counter, visit_counter, result;
    int key = ht_node->productID, tmp;

    curr = hash_function(key, hash_table_size, 0);    
    if(pthread_mutex_lock(&hash_table[curr]->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    collision_counter = 1;
    visit_counter     = 0;
    while(hash_table[curr]->productID != -1 && visit_counter < hash_table_size) {
        if(hash_table[curr]->productID == key) break;
        
        /*Ordered Hashing*/
        if(hash_table[curr]->productID > key) {
            tmp = hash_table[curr]->productID;
            hash_table[curr]->productID = key;
            key = tmp;

            if(pthread_mutex_unlock(&hash_table[prev]->lock) != 0) {
                perror("Error: pthread_mutex_lock failed!");
                exit(1);
            }
            if(pthread_mutex_unlock(&hash_table[curr]->lock) != 0) {
                perror("Error: pthread_mutex_lock failed!");
                exit(1);
            }

            curr = hash_function(key, hash_table_size, 0);    
            
            if(pthread_mutex_lock(&hash_table[curr]->lock) != 0) {
                perror("Error: pthread_mutex_lock failed!");
                exit(1);
            }
            collision_counter = 1;
            visit_counter     = 0;
            continue;
        }
        
        prev = curr;
        curr = hash_function(key, hash_table_size, collision_counter++);
        if(pthread_mutex_lock(&hash_table[curr]->lock) != 0) {
            perror("Error: pthread_mutex_lock failed!");
            exit(1);
        }

        if(pthread_mutex_unlock(&hash_table[prev]->lock) != 0) {
            perror("Error: pthread_mutex_lock failed!");
            exit(1);
        }
        visit_counter++;
    }

    if(visit_counter == hash_table_size || hash_table[curr]->productID == key) result = 0;
    else {
        hash_table[curr]->productID = key;
        result = 1;
    }

    if(pthread_mutex_unlock(&hash_table[curr]->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    if(prev != -1 && pthread_mutex_unlock(&hash_table[prev]->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    return result;
}

int HTDelete(struct HTNode** hash_table, int hash_table_size, int productID) {
    int prev = -1, curr, collision_counter, visit_counter, result;

    curr = hash_function(productID, hash_table_size, 0);    
    if(pthread_mutex_lock(&hash_table[curr]->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    collision_counter = 1;
    visit_counter     = 0;
    while(hash_table[curr]->productID != productID && visit_counter < hash_table_size) {
        prev = curr;
        curr = hash_function(productID, hash_table_size, collision_counter++);
        if(pthread_mutex_lock(&hash_table[curr]->lock) != 0) {
            perror("Error: pthread_mutex_lock failed!");
            exit(1);
        }

        if(pthread_mutex_unlock(&hash_table[prev]->lock) != 0) {
            perror("Error: pthread_mutex_lock failed!");
            exit(1);
        }
        visit_counter++;
    }

    if(visit_counter == hash_table_size) result = 0;
    else {
        hash_table[curr]->productID = -1;
        result = 1;
    }

    if(pthread_mutex_unlock(&hash_table[curr]->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    if(prev != -1 && pthread_mutex_unlock(&hash_table[prev]->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    return result;
}

int HTSearch(struct HTNode** hash_table, int hash_table_size, int productID) {
    int prev = -1, curr, collision_counter, visit_counter, result;

    curr = hash_function(productID, hash_table_size, 0);    
    if(pthread_mutex_lock(&hash_table[curr]->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    collision_counter = 1;
    visit_counter     = 0;
    while(hash_table[curr]->productID != productID && visit_counter < hash_table_size) {
        prev = curr;
        curr = hash_function(productID, hash_table_size, collision_counter++);
        if(pthread_mutex_lock(&hash_table[curr]->lock) != 0) {
            perror("Error: pthread_mutex_lock failed!");
            exit(1);
        }

        if(pthread_mutex_unlock(&hash_table[prev]->lock) != 0) {
            perror("Error: pthread_mutex_lock failed!");
            exit(1);
        }
        visit_counter++;
    }

    if(visit_counter == hash_table_size) result = 0;
    else result = 1;

    if(pthread_mutex_unlock(&hash_table[curr]->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    if(prev != -1 && pthread_mutex_unlock(&hash_table[prev]->lock) != 0) {
        perror("Error: pthread_mutex_lock failed!");
        exit(1);
    }

    return result;
}

int hash_function(int key, int table_size, int collision_number) {
    return (hash_1(key, table_size)+collision_number+hash_2(key, table_size))%table_size;
}

int hash_1(int key, int table_size) {
    return key%table_size;
}

int hash_2(int key, int table_size) {
    return table_size - (key%table_size);
}

int HTSize(struct HTNode** hash_table, int size) {
    int counter = 0;
    for(int i = 0; i < size; i++) {
        if(hash_table[i]->productID != -1) counter++;
    }
    return counter;
}

long int HTProductIDSum(struct HTNode** hash_table, int size) {
    long int sum = 0;
    for(int i = 0; i < size; i++) {
        if(hash_table[i]->productID != -1) sum+=hash_table[i]->productID;
    }
    return sum;
}

void printHT(struct HTNode** hash_table, int size) {
    for(int i = 0; i < size; i++) {
        printf("[%d] = %d\n", i, hash_table[i]->productID);
    }
}