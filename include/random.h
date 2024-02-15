#ifndef RANDOM
#define RANDOM

/**** Includes ****/

#include "common.h"

/**** Defines ****/

#define MAX_INT 4294967291    // The (2^32) - 5y

/**** Typedefs ****/

typedef struct values* Values;
typedef struct random_values* Random_values;

/**** Functions ****/

// Creating and returning [k] random values given [w] and data point size [vector_size] 
Random_values create_random_values(int k, int w, int vector_size);

// Destroying [randoms values]
void delete_random_values(Random_values random_values);

// Generating to [coordinates] {0,1} values given the [k] [h_array]  
void generate_point_coordinates(uint32_t* h_array, int* coordinates, int k);

// Generating [k][h] functions based on the format h(p) = floor((p * v + t)/w)
// Given the data point [p] with size [vector_size], k random vector [v], k random t [t] and [w] 
void random_h_values(uint32_t* h, uint8_t* p, float** v, float* t, int k, int w, int vector_size);

// Generating [k][h] functions based on the format h(p) = floor((p * v + t)/w)
// Given the data point [p] with size [vector_size], k random vector [v], k random t [t] and [w] 
void random_h_values_double(uint32_t* h_array, double* p, float** v, float* t, int k, int w, int vector_size);

// Calculating all ids based on the format id(p) = Σ(ri*hi) mod M 
// Given the [k] [h] functions and the [r]
uint32_t random_id_values(uint32_t* h, int* r, int k);

// Returning the k r-random values given the [random_values]
int* get_r_values(Random_values random_values);

// Returning the k t-random values given the [random_values]
float* get_t_values(Random_values random_values);

// Returning the k d-vectors values given the [random_values]
float** get_v_values(Random_values random_values);

#endif