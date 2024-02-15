#ifndef KMEANS
#define KMEANS

/**** Includes ****/

#include "common.h"
#include "metrics.h"

/**** Typedefs ****/

typedef struct kmeans* Kmeans;
typedef struct partial_p* Partial_p;
typedef struct distance_to_centroid* Distance_to_centroid;

/**** Functions ****/

// Kmeans++ algorithm to initialize [k] centroids. Initializing an array [initialized_centroid]
// with the centroids given a dataset [p_array] with [data_size] and size of data points [vector_size]
void k_means_plus_plus(uint8_t** p_array, uint8_t** initialized_centroids, int k, int data_size, int vector_size);

#endif