/**********************************/
/* @author Papageorgiou Efthymios */
/* @version v.1                   */
/**********************************/

#ifndef CONCURRENT_STACK_H
#define CONCURRENT_STACK_H
#include <pthread.h>

struct Stack {
    struct StackNode *top;
    int stack_size;
    pthread_mutex_t lock;
};

struct StackNode {
    int productID;
    struct StackNode *next;
};

struct Stack* createStack();
struct StackNode* createStackNode(int productID);
int push(struct Stack *stack, int productID);
int pop(struct Stack *stack);
int isFull(struct Stack *stack);
int isEmpty(struct Stack *stack);
void printStack(struct Stack *stack);
int getStackSize(struct Stack *stack);
int computeStackSize(struct Stack *stack);

int tryPush(struct Stack *stack, struct StackNode *stack_node);
struct StackNode* tryPop(struct Stack *stack);

#endif /*CONCURRENT_STACK_H*/