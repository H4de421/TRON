#include "Display/DisplayLoop.h"

void *updateDisplayLoop(void *raw_args)
{
    struct BoardContent *args = raw_args;

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000000000 / FPS;

    while (!STOPED)
    {
        // lock buffer mutex
        pthread_mutex_lock(args->bufferMutex);
        // Draw_updates
        if (args->buffer->size != 0)
        {
            printf("%s", args->buffer->content);
            fflush(stdout);
            clearString(args->buffer);
        }
        // close mutex
        pthread_mutex_unlock(args->bufferMutex);
        // sleep until next loop
        nanosleep(&ts, NULL);
    }
    // flush the buffer a last time before exiting
    pthread_mutex_lock(args->bufferMutex);
    if (args->buffer->size != 0)
    {
        printf("%s", args->buffer->content);
        fflush(stdout);
        clearString(args->buffer);
    }
    pthread_mutex_unlock(args->bufferMutex);

    return NULL;
}
