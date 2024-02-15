#ifndef COMMON
#define COMMON

/**** Includes ****/

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>
#include <arpa/inet.h>

/**** Defines ****/

#define pi (3.1415926535897932384626)

/**** Typedefs ****/

typedef void* Pointer;

/**** Functions ****/

// Reallocate memory for [pointer] given the [size]
Pointer mem_realloc(Pointer pointer, size_t size);

// Allocate & initialize memory for Pointer given the [size]
Pointer mem_allocate(size_t size);

// Free memory allocated for a given [pointer] 
void mem_free(Pointer pointer);

#endif