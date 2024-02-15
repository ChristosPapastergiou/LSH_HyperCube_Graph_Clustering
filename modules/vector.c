#include "../include/vector.h"

struct vector_node{
	int data_index;     // Point index on dataset
    uint8_t* point;     // Point info
    double distance;    // The distance of the point
};

struct vector{
    int true_size;      // True size of the allocated array
    int virtual_size;   // How many data array has 
    Vector_node* nodes; // Array to store data point
};

// Helping function merge sort uses to merge
static void merge(Vector_node* array, int left, int mid, int right){
    int i, j, k;
    int sub1 = mid - left + 1;
    int sub2 = right - mid;

    Vector_node* temp_left_array = (Vector_node*) mem_allocate(sizeof(Vector_node) * sub1);   // Temp arrays to store values from the real array
    Vector_node* temp_right_array = (Vector_node*) mem_allocate(sizeof(Vector_node) * sub2);  // of data points so it can be modified

    for(i = 0; i < sub1; i++){
        temp_left_array[i] = array[left + i];       // Copying to temp arrays
    }                                               // the real array
    for (j = 0; j < sub2; j++){
        temp_right_array[j] = array[mid + 1 + j];
    }

    i = 0;      // First subarray index
    j = 0;      // Second subarray index
    k = left;   // Merged subarray index
    while((i < sub1) && (j < sub2)){
        if(temp_left_array[i]->distance <= temp_right_array[j]->distance){      // After subarrays are done the real 
            array[k] = temp_left_array[i];                                      // array must be modified based on the 
            i++;                                                                // subarrays. This is the merge step
        }else{
            array[k] = temp_right_array[j];
            j++;
        }
        k++;
    }
    while(i < sub1){
        array[k] = temp_left_array[i];
        i++;
        k++;
    }
    while(j < sub2){
        array[k] = temp_right_array[j];
        j++;
        k++;
    }

    free(temp_left_array);
    free(temp_right_array);
}

// Using merge sort to sort the vector
static void merge_sort(Vector_node* array, int left, int right){
    if(left < right){
        int mid = left + (right - left) / 2;

        merge_sort(array, left, mid);
        merge_sort(array, mid + 1, right);

        merge(array, left, mid, right);
    }
}

// Initializating vector values
static void initialize_vector(Vector vector, int size){
    vector->true_size = size;
    vector->virtual_size = 0;
}

// Creating and initializing the array of nodes, every node is a data point
static Vector_node* create_vector_nodes(Vector vector){
    vector->nodes = (Vector_node*) mem_allocate(sizeof(Vector_node) * vector->true_size);
    for(int i = 0; i < vector->true_size; i++){
        vector->nodes[i] = (Vector_node) mem_allocate(sizeof(struct vector_node));
    }
    return vector->nodes;
}

Vector create_vector(int size){
    Vector vector = (Vector) mem_allocate(sizeof(struct vector));
    initialize_vector(vector, size);
    vector->nodes = create_vector_nodes(vector);
    return vector;
}

void destroy_vector(Vector vector){
    for(int i = 0; i < vector->true_size; i++){ // True size is the size that has been allocated
        mem_free(vector->nodes[i]);
    }
    mem_free(vector->nodes);
    mem_free(vector);
}

void insert_vector(Vector vector, uint8_t* point, double distance, int position, int data_index){
    vector->virtual_size++;
    vector->nodes[position]->point = point;
    vector->nodes[position]->distance = distance;
    vector->nodes[position]->data_index = data_index;
}

void remove_vector(Vector vector, int position){
    vector->nodes[position]->point = NULL;
    vector->nodes[position]->distance = -1;
    vector->nodes[position]->data_index = -1;
}

void sort_vector(Vector vector){
    merge_sort(vector->nodes, 0, vector->virtual_size - 1);
}

int get_vector_size(Vector vector){
    return vector->virtual_size;
}

int get_image_index_vector(Vector vector, int index){
    return vector->nodes[index]->data_index;
}

double get_image_distance_vector(Vector vector, int index){
    return vector->nodes[index]->distance;
}