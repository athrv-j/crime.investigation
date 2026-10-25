#ifndef STACK_H
#define STACK_H

typedef struct {
    void** items;
    int top;
    int capacity;
} Stack;

Stack* createStack(int capacity);
void push(Stack*s,void* item);
void* pop(Stack*s);
int isEmpty(Stack*s);
void freeStack(Stack*s);

#endif