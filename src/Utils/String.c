#include "Utils/String.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct string *createString(int capacity, pthread_mutex_t *dispaly_mutex)
{
    struct string *res = malloc(sizeof(struct string));
    res->size = 0;
    res->capacity = capacity;
    res->content = calloc(capacity + 1, sizeof(char));
    res->dispaly_mutex = dispaly_mutex;
    return res;
}

void destroyString(struct string *str)
{
    free(str->content);
    free(str);
}

void clearString(struct string *str)
{
    *(str->content) = '\0';
    str->size = 0;
}

void doubleCapacity(struct string *str)
{
    str->content =
        realloc(str->content, (str->capacity * 2 + 1) * sizeof(char));
    str->capacity *= 2;
}

void addToString(struct string *str, char *content, int size)
{
    pthread_mutex_lock(str->dispaly_mutex);
    if (str->size + size >= str->capacity)
    {
        doubleCapacity(str);
        pthread_mutex_unlock(str->dispaly_mutex);
        addToString(str, content, size);
    }
    else
    {
        strcat(str->content, content);
        str->size += size;
        str->content[str->size] = '\0';
    }
    pthread_mutex_unlock(str->dispaly_mutex);
}

char *my_itoa(int nb)
{
    if (nb == 0)
    {
        return "0";
    }
    char tmp[2048];
    char *res = calloc(2048, 1);
    int i = 0;
    while (nb > 0)
    {
        int value = nb % 10;
        tmp[i] = '0' + value;
        i++;
        nb /= 10;
    }
    for (int j = i - 1; j >= 0; j--)
    {
        res[(i - 1) - j] = tmp[j];
    }
    return res;
}
