#pragma once

#include <stdlib.h>
#include <pthread.h>

typedef struct string {
    char *content;
    int size;
    int capacity;
    pthread_mutex_t *dispaly_mutex;
} String ;

struct string *createString(int capacity, pthread_mutex_t *dispaly_mutex);
void destroyString(struct string *str);

void clearString(struct string *str);

void doubleCapacity(struct string *str);

void addToString(struct string*str, char *content, int size);

char *my_itoa(int nb);

