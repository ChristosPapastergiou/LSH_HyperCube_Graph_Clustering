#include "../include/common.h"

Pointer mem_realloc(Pointer pointer, size_t size){
    Pointer array = realloc(pointer, size);
    if(array == NULL){
        printf("Re-Allocation failure\n");
        exit(EXIT_FAILURE);
    }
    return array;
}

Pointer mem_allocate(size_t size){
    Pointer array = calloc(1, size);
    if(array == NULL){          
        printf("Allocation failure\n");
        exit(EXIT_FAILURE);
    }
    return array;
}

void mem_free(Pointer pointer){
    free(pointer);
}