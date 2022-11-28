#include "concurrent_stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_STACK_SIZE 1000

struct Stack* createStack() {
    struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
    assert(stack != NULL);
    stack->top          =  createStackNode(-1);
    stack->stack_size   = 0;

    if(pthread_mutex_init(&stack->lock, NULL) != 0) {                                    
        perror("Error: pthread_mutex_init failed!");                                                       
        exit(1);                                                                    
    }

    stack->top->next = NULL;

    return stack;
}

struct StackNode* createStackNode(int productID) {
    struct StackNode* stack_node = (struct StackNode*)malloc(sizeof(struct StackNode));
    assert(stack_node != NULL);
    stack_node->productID = productID;
    return stack_node;
}

int isFull(struct Stack *stack) {
    if(stack->stack_size == MAX_STACK_SIZE) return 1;
    return 0;
}

int isEmpty(struct Stack *stack) {
    if(stack->stack_size == 0) return 1;
    return 0;
}

int push(struct Stack *stack, int productID) {
    struct StackNode *stack_node = createStackNode(productID);
    assert(stack_node != NULL);

    stack_node->productID = productID;
    while(1) {
        if(tryPush(stack, stack_node)) return 1;
        //else Backoff(MIN, MAX);
    }
    return 0;
}

int tryPush(struct Stack *stack, struct StackNode *stack_node) {
    struct StackNode *old_top = stack->top;
    stack_node->next = old_top;

    if(pthread_mutex_trylock(&stack->lock) == 0) {
        if(old_top == stack->top) {
            stack_node->next = stack->top;
            stack->top       = stack_node;
            if(pthread_mutex_unlock(&stack->lock) != 0) {
                perror("Error: pthread_mutex_unlock failed!");
                exit(1);
            }
            stack->stack_size++;
        }
        return 1;
    }

    return 0;
}

int getStackSize(struct Stack *stack) {
    return stack->stack_size;
}

void printStack(struct Stack *stack) {
    struct StackNode *curr = stack->top;

    printf("\nStack\n");
    while(curr != NULL) {
        if(curr->productID != -1)
            printf("|%d|\n", curr->productID);
        curr = curr->next;
    }
    printf("\n");
}