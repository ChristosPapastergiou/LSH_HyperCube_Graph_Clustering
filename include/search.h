#ifndef SEARCH
#define SEARCH

/**** Includes ****/

#include "list.h"
#include "graph.h"
#include "common.h"
#include "random.h"
#include "metrics.h"
#include "macqueen.h"
#include "hashtable.h"
#include "hypercube.h"

/**** Defines ****/

#define R_NEIGHBOURS 20
#define R_CLUSTERS 30000

/**** Functions ****/

/** LSH **/

// Approximate kNN search algorithm. Returning an array of lists, with size [query_size] and every list has the [N] data points that are approximate closest to the query data
// Given the [L] [hashtables], query array [q_array] with data point size [vector_size] and [start_time] [end_time] to save the overall time kNN did 
List* lsh_approximate_kNN(Hashtable* hashtables, uint8_t** q_array, int** bucket, int L, int N, int vector_size, int query_size, struct timeval* start_time, struct timeval* end_time);

// Approximate range search algorithm. Returning an array of lists, with size [query_size] and every list has the [R_NEIGHBOURS] data points that are approximate closest to the query data
// Given the [L] [hashtables], query array [q_array] with data point size [vector_size] and the radious [R] to search
List* lsh_approximate_range_search(Hashtable* hashtables, uint8_t** q_array, int** bucket, int L, int N, int vector_size, int query_size, double R);

/** Cube **/

// Approximate kNN search algorithm. Returning an array of lists, with size [query_size] and every list has the [N] data points that are approximate closest to the query data
// Given a [hypercube] query array [q_array] with data point size [vector_size] and [start_time] [end_time] to save the overall time kNN did 
List* cube_approximate_kNN(Hypercube hypercube, uint8_t** q_array, int** coordinates, int M, int N, int vector_size, int query_size, struct timeval* start_time, struct timeval* end_time);

// Approximate range search algorithm. Returning an array of lists, with size [query_size] and every list has the [R_NEIGHBOURS] data points that are approximate closest to the query data
//  Given the [L] [hashtables], query array [q_array] with data point size [vector_size] and the radious [R] to search
List* cube_approximate_range_search(Hypercube hypercube, uint8_t** q_array, int** coordinates, int M, int N, int vector_size, int query_size, double R);

/** Cluster **/

// Approximate range search algorithm. Returning an array of lists, with size [num_centroids] and every list has the [R_NEIGHBOURS] data points that are approximate closest to the centroid
// Given the [L] [hashtables], query array [q_array] with data point size [vector_size] and the radious [R] to search
List* cluster_lsh_approximate_range_search(Hashtable* hashtables, Macqueen macqueen, int** bucket, int L, int vector_size, int num_centroids, double R);

// Approximate range search algorithm. Returning an array of lists, with size [num_centroids] and every list has the [R_NEIGHBOURS] data points that are approximate closest to the centroid
// Given the [hypercube], query array [q_array] with data point size [vector_size] and the radious [R] to search
List* cluster_cube_approximate_range_search(Hypercube hypercube, Macqueen macqueen, int** coordinates, int M, int vector_size, int num_centroids, double R);

/** Graph **/

// Graph Nearest Neighbour algorithm. Creating and return a list of [N] nearest neighbours given a [graph],
// a query [point] with size [vector_size], [R] random restarts, number of expantions [E], number of [greedy_steps]
List gNN_search(Graph graph, uint8_t* point, int N, int R, int E, int greedy_steps, int vector_size);

// Generic search on Graph algorithm. Creating and return a list of [N] nearest neighbours given a [graph],
// a query [point] with size [vector_size], navigating/start vertex of the dataset [start_node], candidate pool size [l]
List gg_search(Graph graph, uint8_t* point, int start_node, int N, int l, int vector_size);

/** General **/

// Exausted search algorithm. Returning an array of lists, with size [query_size] and every list has the [N] data that are real closest to the query data.
// Given query array [q_array] and the dataset array [p_array] of size [data_size] and each data point with size [vector_size]
List* exausted_search(uint8_t** q_array, uint8_t** p_array, int vector_size, int query_size, int data_size, int N, struct timeval* start_time, struct timeval* end_time);

#endif