#include "../include/hypercube.h"

struct vertex{
    int points;
    int neighbours;
    int* image_num;
    uint8_t** p_array;
    int* neighbours_array;
    int* vertex_coordinates;
};

struct hypercube{
    int vertices;
    int dimensions;
    Vertex* vertex_array;
};

// Function to connect all the vertexes with their neighbours
static void connect_neighbours(Hypercube hypercube, int num_neighbours){
    int neighbour_index;
    for(int i = 0; i < hypercube->vertices; i++){
        int hamming_distance = 1;
        for(int j = 0; j < hypercube->vertices; j++){
            if(hypercube->vertex_array[i]->neighbours == num_neighbours){ // If we reach maximum allowed number of neighbours we continue to the next vertex
                break;
            }
            int bit_difference = 0;
            if(i != j){
                for(int k = 0; k < hypercube->dimensions; k++){
                    if(hypercube->vertex_array[i]->vertex_coordinates[k] != hypercube->vertex_array[j]->vertex_coordinates[k]){
                        bit_difference++;
                    }
                }
                if(bit_difference == hamming_distance){
                    neighbour_index = hypercube->vertex_array[i]->neighbours;
                    hypercube->vertex_array[i]->neighbours_array[neighbour_index] = j;
                    hypercube->vertex_array[i]->neighbours++;
                }
            }
            if((j == (hypercube->vertices - 1)) && (hypercube->vertex_array[i]->neighbours < num_neighbours)){    // If we reach the end of the loop and need more neigbours 
                hamming_distance++;                                                                             // Increment hamming distance by one
                j = -1;                                                                                         // Repeat the loop for the new hamming distance
            } 
        }
    }
}

// Decoding the binary to decimal
static int binary_to_decimal(int* point_coordinates, int size){
    int value = 0;
    int power = 0;
    for(int i = (size - 1); i >= 0; i--){
        if(point_coordinates[i] != 0){
            value += (int) pow(2, power);
        }
        power++;
    }
    return value;
}

// Initializating every vertex
static void initialize_vertices(Hypercube hypercube, int num_neighbours){
    for(int i = 0; i < hypercube->vertices; i++){
        for(int j = 0; j < hypercube->dimensions; j++){
            hypercube->vertex_array[i]->vertex_coordinates[j] = (i >> j) & 1; 
        }
        hypercube->vertex_array[i]->neighbours = 0;
        hypercube->vertex_array[i]->points = 0;
    }
    connect_neighbours(hypercube, num_neighbours);
}

// Creation of every vertex
static void create_vertices(Hypercube hypercube, int probes){
    uint32_t num_neighbours = probes - 1;
    for(int i = 0; i < hypercube->vertices; i++){
        hypercube->vertex_array[i]->image_num = (int*) mem_allocate(sizeof(int));
        hypercube->vertex_array[i]->p_array = (uint8_t**) mem_allocate(sizeof(uint8_t*));
        hypercube->vertex_array[i]->neighbours_array = (int*) mem_allocate(sizeof(int) * num_neighbours);
        hypercube->vertex_array[i]->vertex_coordinates = (int*) mem_allocate(sizeof(int) * hypercube->dimensions);
    }
    initialize_vertices(hypercube, num_neighbours);
}

// Initialization of the hypercube
static void initialize_hypercube(Hypercube hypercube, int d, int vertices, int probes){
    hypercube->dimensions = d;
    hypercube->vertices = vertices;
    create_vertices(hypercube, probes);
}

Hypercube create_hypercube(int d, int probes){
    int vertices = (int) pow(2, d);
    Hypercube hypercube = (Hypercube) mem_allocate(sizeof(struct hypercube));
    hypercube->vertex_array = (Vertex*) mem_allocate(sizeof(Vertex) * vertices);
    for(int i = 0; i < vertices; i++){
        hypercube->vertex_array[i] = (Vertex) mem_allocate(sizeof(struct vertex));
    }
    initialize_hypercube(hypercube, d, vertices, probes);
    return hypercube;
}

void destroy_hypercube(Hypercube hypercube){
    for(int i = 0; i < hypercube->vertices; i++){
        mem_free(hypercube->vertex_array[i]->p_array);
        mem_free(hypercube->vertex_array[i]->image_num);
        mem_free(hypercube->vertex_array[i]->neighbours_array);
        mem_free(hypercube->vertex_array[i]->vertex_coordinates);
        mem_free(hypercube->vertex_array[i]);
    }
    mem_free(hypercube->vertex_array);
    mem_free(hypercube);
}

void insert_hypercube(Hypercube hypercube, uint8_t* p, int* coordinates, int image_number){
    int vertex = binary_to_decimal(coordinates, hypercube->dimensions);   // Vertex index uses the helping function to be more efficient
    int points = hypercube->vertex_array[vertex]->points;                 // And updating values for this vertex
    hypercube->vertex_array[vertex]->image_num = (int*) mem_realloc(hypercube->vertex_array[vertex]->image_num, sizeof(int) * (points + 1));
    hypercube->vertex_array[vertex]->p_array = (uint8_t**) mem_realloc(hypercube->vertex_array[vertex]->p_array, sizeof(uint8_t*) * (points + 1));
    hypercube->vertex_array[vertex]->image_num[points] = image_number;
    hypercube->vertex_array[vertex]->p_array[points] = p;
    hypercube->vertex_array[vertex]->points++;
}

void print_hypercube(Hypercube hypercube){
    int total = 0;
    for(int i = 0; i < hypercube->vertices; i++){
        total += hypercube->vertex_array[i]->points;
    }
    printf("%d\n", total);
}

void get_hypercube_data(Hypercube hypercube, uint8_t** p_array, int* data_numbers, int vertex_index, int points_to_check){
    int last_index = -1;
    int checked_points = 0;
    int vertex_points = hypercube->vertex_array[vertex_index]->points;
    for(int i = 0; i < vertex_points; i++){                                         // For the specific vertex filling the 
        p_array[i] = hypercube->vertex_array[vertex_index]->p_array[i];             // array. If the points to check are more than
        data_numbers[i] = hypercube->vertex_array[vertex_index]->image_num[i];     // the vertex points then need to go and check it's neighbours
        checked_points++;                                                           
        last_index = i;
        if(checked_points == points_to_check){
            return;
        }
    }

    int neighbours = hypercube->vertex_array[vertex_index]->neighbours;                             // Taking all the neighbours
    for(int i = 0; i < neighbours; i++){                                                            // and for each neighbour filling the array
        int neightbour_index = hypercube->vertex_array[vertex_index]->neighbours_array[i];          // until all the points need to check are checked
        int neightbour_points = hypercube->vertex_array[neightbour_index]->points;
        for(int j = 0; j < neightbour_points; j++){
            last_index++;
            p_array[last_index] = hypercube->vertex_array[neightbour_index]->p_array[j];
            data_numbers[last_index] = hypercube->vertex_array[neightbour_index]->image_num[j];
            checked_points++;
            if(checked_points == points_to_check){
                return;
            }
        }
    }
}

int get_vertex_index(Hypercube hypercube, int* coordinates){
    return binary_to_decimal(coordinates, hypercube->dimensions);   // Vertex index uses the helping function to be more efficient
}

int get_total_points(Hypercube hypercube, int vertex_index){
    int total = 0;
    int neightbour_index;
    total += hypercube->vertex_array[vertex_index]->points;
    for(int i = 0; i < hypercube->vertex_array[vertex_index]->neighbours; i++){
        neightbour_index = hypercube->vertex_array[vertex_index]->neighbours_array[i];
        total += hypercube->vertex_array[neightbour_index]->points;
    }
    return total;
}