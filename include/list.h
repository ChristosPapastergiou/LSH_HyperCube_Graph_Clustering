#ifndef LIST
#define LIST

/**** Includes ****/

#include "common.h"

/**** Typedefs ****/

typedef struct node* Node;
typedef struct list* List;

/**** Functions ****/

// Creating and returning a linked list. [N] is used in case no need
// to go any further. If there is a need to add more the call increase_list_capacity
List create_list(int N);

// Destroying a given [list]
void destroy_list(List list);

// Inserting a data point [p] in the [list], given the [data_number]
void insert_list(List list, uint8_t* p, double distance, int data_number);

// Inserting a data point [p] in the [list] sorted based on [distance], given the [data_number]
void insert_sorted_list(List list, uint8_t* p, double distance, int data_number);

// Inserting a data point [p] in the begging of the [list], given the [data_number]
void insert_front_list(List list, uint8_t* p, double distance, int data_number);

// Sorting a given [list]
void list_sort(List list);

// Removing the first data point of a given [list]
void remove_sorted_item(List list);

// Printing the [list] to the [output_file] 
void print_list(FILE* output_file, List list);

// Printing the [n] data point given a [list] to the [output_file]
void print_list_item(FILE* output_file, List list, int n);

// Increasing the capacity of N-nearest neighbours of a given [list]
void increase_list_capacity(List list);

// Marking as checked the data point with [data_index] of a given [list]
void update_checked(List list, int data_index);

// Checking if the data point with [data_index] is marked as checked given [list]
bool check_if_checked(List list, int data_index);

// Returning the size of a given [list]
int get_list_size(List list);

// Returning the data point index of the [n] item of given a [list]
int get_image(List list, int n);

// Returning the data point distance of the [n] item of given a [list]
double get_distance(List list, int n);

// Returning the distance the last data of given a [list]
double get_last_distance(List list);

#endif