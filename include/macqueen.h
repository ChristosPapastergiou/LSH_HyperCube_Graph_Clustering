#ifndef MACQUEEN
#define MACQUEEN

/**** Includes ****/

#include "common.h"
#include "random.h"
#include "metrics.h"
#include "hashtable.h"
#include "hypercube.h"

/**** Defines ****/

#define CAPACITY 1000
#define ITERATIONS 10000

/**** Typedefs ****/

typedef struct assign* Assign;
typedef struct macqueen* Macqueen;
typedef struct centroid_data* Centroid_data;

/**** Functions ****/

// Creating and returning macqueen given a dataset [p_array] with [data_size] and data point size [vector_size], 
// an array with the initialized centroids [centroids], the total clusters [num_centroids]
// Give specific [method] to update the centroids
Macqueen lighting_macqueen(Hashtable* hashtables, Random_values* hash_random_values, Hypercube hypercube, Random_values cube_random_values, uint8_t** p_array, double** centroids, int num_centroids, int number_of_vector_hash_tables, int number_of_vector_hash_functions, int max_number_M_hypercube, int number_of_hypercube_dimensions, int number_of_probes, int data_size, int vector_size, char* method);

// Destroying a given [macqueen]
void destroy_macqueen(Macqueen macqueen);

// Updating array of assignments given the [macqueen] and the index of the array that needs to be updated [assignment_index]
// Values that are about to be updated [centroid_index], [second_closest_centroid] and the [distance]
void update_assign(Macqueen macqueen, int assignment_index, int centroid_index, int second_closest_centroid, double distance);

// Increasing or decreasing total data of a centroid given [macqueen] and [centroid_index]
// Increasing data if [flag] = true and decreasing if [flag] = false
void set_centroid_total_data(Macqueen macqueen, int centroid_index, bool flag);

// Returning the assignment to a centroid of a data given the [macqueen] and the data index [assignment_index] 
int get_assignment_to_centroid(Macqueen macqueen, int assignment_index);

// Returning the total amount of data a centroid has given [macqueen] and the centroid's index [centroid_index]
int get_centroid_total_data(Macqueen macqueen, int centroid_index);

// Returning the 2nd closest centroid of a data given [macqueen] and data's index [assignment_index]
int get_second_closest_centroid(Macqueen macqueen, int assignment_index);

// Returning the total amount of clusters given [macqueen]
int get_number_of_clusters(Macqueen macqueen);

// Returning the distance of the assignment to a centroid of a data given the [macqueen] and the data index [assignment_index] 
double get_distance_to_centroid(Macqueen macqueen, int assignment_index);

// Returning the array of the coordinates given the [macqueen] and the index of the wanted centroid [centroid_index]
double* get_centroid_coordinates(Macqueen macqueen, int centroid_index);

#endif