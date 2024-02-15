#ifndef HASHTABLE
#define HASHTABLE

/**** Includes ****/

#include "common.h"

/**** Typedefs ****/

typedef struct node* Node;
typedef struct hashtable* Hashtable;

/**** Functions ****/

// Createing and returning a hashtable with size [size] / 32
Hashtable create_hashtable(int size);

// Destroying given [hashtable]
void destroy_hashtable(Hashtable hashtable);

// Inserting a data point [p] in the [hashtable], given [id] of data and [data_number] in dataset
void insert_hashtable(Hashtable hashtable, uint8_t* p, uint32_t id, int data_number);

// Passing in a wanted array of dataset [p_array] and [data_numbers] 
// all the necessary data given the [hashtable] and the [bucket]
void get_hashtable_data(Hashtable hashtable, uint8_t** p_array, int* data_numbers, int bucket);

// Returning the size of a given [hashtable]
int get_hashtable_size(Hashtable hashtable);

// Returning the size of [bucket] given a [hashtable] 
int get_bucket_size(Hashtable hashtable, int bucket);

#endif