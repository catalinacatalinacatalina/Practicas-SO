#ifndef GLOBALS_H
#define GLOBALS_H
#include <semaphore.h>

typedef struct {
    int servings;
    int cook_waiting;
    int sav_waiting;
}shared_t;



#endif