#ifndef VECTOR
#define VECTOR

/**** Includes ****/

#include "common.h"

/**** Typedefs ****/

typedef struct vector* Vector;
typedef struct vector_node* Vector_node;

/**** Functions ****/

// Creating and returning a vector with [size]
Vector create_vector(int size);

// Destroying given [vector]
void destroy_vector(Vector vector);

// Inserting a data point [point] with [distance] and number 
// in dataset [data_index] in the [position] in the given [vector]
void insert_vector(Vector vector, uint8_t* point, double distance, int position, int data_index);

// Initializing to zero the data of a data point given a [vector] and the [position] 
void remove_vector(Vector vector, int position);

// Sorting a given [vector]
void sort_vector(Vector vector);

// Returning the size of a given [vector]
int get_vector_size(Vector vector);

// Returning the index of data point given a [vector] and [index] in the vector
int get_image_index_vector(Vector vector, int index);

// Returning the distance of data point given a [vector] and [index] in the vector
double get_image_distance_vector(Vector vector, int index);

#endif