#ifndef ASSIGN
#define ASSIGN

/**** Includes ****/

#include "list.h"
#include "common.h"
#include "random.h"
#include "search.h"
#include "metrics.h"
#include "macqueen.h"
#include "hashtable.h"
#include "hypercube.h"

/**** Defines ****/

#define W 20

/**** Functions ****/

// Lloyd's algorithm to assign data to each nearest centroid. Given an array of [num_centroids], dataset [p_array]
// of data with [data_size], [macqueen] to assign the values. [flag] is used to stop the Lloyd's algorithm
// if no new assignment are possible. Pass to [flag] false value and if nessecery Lloyd's will turn in to true
void lloyd(Macqueen macqueen, uint8_t** p_array, int num_centroids, int data_size, int vector_size, bool* flag);

// Reverse search's algorithm to assign data to each nearest centroid. Given [L] [hashtables], dataset [p_array]
// of data with [data_size], [macqueen] to assign the values. [flag] is used to infrom the reverse's algorithm 
// that most balls have not been changed. Pass to [flag] false value and if nessecery reverse will turn in to true
void lsh_reverse_search(Hashtable* hashtables, Random_values* random_values, Macqueen macqueen, uint8_t** p_array, int num_centroids, int L, int  k, int data_size, int vector_size, int* old_size, double R, bool* flag);

// Reverse search's algorithm to assign data to each nearest centroid. Given [hypercube], dataset [p_array]
// of data with [data_size], [macqueen] to assign the values. [flag] is used to infrom the reverse's algorithm 
// that most balls have not been changed. Pass to [flag] false value and if nessecery reverse will turn in to true
void cube_reverse_search(Hypercube hypercube, Random_values cube_random_values, Macqueen macqueen, uint8_t** p_array, int num_centroids, int M, int k, int probes, int data_size, int vector_size, double R, bool* flag);

#endif