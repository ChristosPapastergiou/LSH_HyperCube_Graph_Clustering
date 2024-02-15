#ifndef HYPERCUBE
#define HYPERCUBE

/**** Includes ****/

#include "common.h"

/**** Typedefs ****/

typedef struct vertex* Vertex;
typedef struct hypercube* Hypercube;

/**** Functions ****/

// Createing and returning hypercube given number of dimensions [d] and number of [probes]
Hypercube create_hypercube(int d, int probes);

// Destroying given [hypercube]
void destroy_hypercube(Hypercube hypercube);

// Inserting a data point [p] in the [hypercube], given d-coordinates array [coordinates] and the [image_number]
void insert_hypercube(Hypercube hypercube, uint8_t* p, int* coordinates, int image_number);

// Passing in a wanted array of dataset [p_array] and the number of [points_to_check] 
// all the necessary data given the [hypercube] the [vertex_index] [bucket]
void get_hypercube_data(Hypercube hypercube, uint8_t** p_array, int* data_numbers, int vertex_index, int points_to_check);

// Returning the index of a vertex given a [hypercube] and vetrex's [coordinates]
int get_vertex_index(Hypercube hypercube, int* coordinates);

// Returning the total amount of data points given a [hypercube] and vetrex's [vertex_index]
int get_total_points(Hypercube hypercube, int vertex_index);

#endif