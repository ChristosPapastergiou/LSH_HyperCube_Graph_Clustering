#ifndef GRAPH
#define GRAPH

/**** Includes ****/

#include "common.h"

/**** Typedefs ****/

typedef struct node* Node;
typedef struct edge* Edge;
typedef struct graph* Graph;

/**** Functions ****/

// Creating and returning a graph of [size] equal to dataset size given dataset array [p_array] 
Graph create_graph(uint8_t** p_array, int size);

// Destroying a given [graph]
void destroy_graph(Graph graph);

// Connecting [source_index] with the neighbour [destination_index] of a given [graph]
void add_directed_edge(Graph graph, int source_index, int destination_index);

// Given an [array] filling it with the neighbours of [vertex], 
// given the [graph] and the number of expantion [E]
void get_edges(Graph graph, int* array, int vertex, int E);

// Saving in a .csv file the neighbour indexes of every data point given a [graph] 
void save_graph(Graph graph);

// Loading a pre-saved [graph] of the graph_file
void load_graph(Graph graph, FILE* graph_file);

// Returning data information with [index] given a [graph]
uint8_t* get_point(Graph graph, int index);

// Returning the size of [graph]
int get_graph_size(Graph graph);

// Returning the number of neighbours of data point with number [index] given a [graph]
int get_neighbours(Graph graph, int index);

#endif