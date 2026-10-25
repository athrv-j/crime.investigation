#include <stdlib.h>
#include "../headers/stack.h"

Stack* createStack(int capacity) {
    Stack *s = (Stack*)malloc(sizeof(Stack));
    s->items = (void**)malloc(capacity * sizeof(void*));
    s->top = -1;
    s->capacity = capacity;
    return s;
}

void push(Stack *s, void *item) {
    if (s->top < s->capacity - 1) {
        s->items[++s->top] = item;
    }
}

void* pop(Stack *s) {
    if (s->top >= 0) {
        return s->items[s->top--];
    }
    return NULL;
}

int isEmpty(Stack *s) {
    return s->top == -1;
}

void freeStack(Stack *s) {
    free(s->items);
    free(s);
}