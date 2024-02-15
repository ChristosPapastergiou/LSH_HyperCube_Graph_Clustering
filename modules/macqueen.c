#include "../include/assign.h"
#include "../include/random.h"
#include "../include/macqueen.h"
#include "../include/hashtable.h"
#include "../include/hypercube.h"

struct assign{
    int assigned_to_centroid;       // The centroid that a data will be assigned to
    int second_closest_centroid;    // Data's second closest centroid index
    double distance_to_centroid;    // The distance from the closest centroid
};

struct centroid_data{
    int total_data;         // Total amount of data related to a centroid
    int index_counter;      // Counter using to add data indexes in the data_indexes array
    int* data_indexes;      // Array to store data indexes related to each centroid
    double* coordinates;    // The coordinates of the centroid
};

struct macqueen{
    int data_size;
    int num_centroids;
    Assign* assignment_array;       // Array of size data_size to save the assignments
    Centroid_data* centroid_data;   // Centroid array to store usefull information about every centroid
};

// Initialization of the assignment array
static void initialize_assign(Macqueen macqueen){
    for(int i = 0; i < macqueen->data_size; i++){    
        macqueen->assignment_array[i]->assigned_to_centroid = -1;
        macqueen->assignment_array[i]->second_closest_centroid = -1;
        macqueen->assignment_array[i]->distance_to_centroid = -1;
    }
}

// Initialization of the centroids data
static void initialize_centroid_data(Macqueen macqueen, double** centroids, int vector_size){
    for(int i = 0; i < macqueen->num_centroids; i++){
        macqueen->centroid_data[i]->coordinates = (double*) mem_allocate(sizeof(double) * vector_size);
        memcpy(macqueen->centroid_data[i]->coordinates, centroids[i], sizeof(double) * vector_size);  // Storing the centroid's data
        macqueen->centroid_data[i]->total_data = 0;
        macqueen->centroid_data[i]->index_counter = 0;
    }
}

// Initialization of the macqueen
static void initialize_macqueen(Macqueen macqueen, int data_size, int num_centroids){
    macqueen->data_size = data_size;
    macqueen->num_centroids = num_centroids;
}

// Helping function to create the macqueen to store both assignment array and centroid data array
static Macqueen create_macqueen(double** centroids, int num_centroids, int data_size, int vector_size){
    Macqueen macqueen = (Macqueen) mem_allocate(sizeof(struct macqueen));
    macqueen->centroid_data = (Centroid_data*) mem_allocate(sizeof(Centroid_data) * num_centroids);
    macqueen->assignment_array = (Assign*) mem_allocate(sizeof(Assign) * data_size);
    for(int i = 0; i < data_size; i++){
        macqueen->assignment_array[i] = (Assign) mem_allocate(sizeof(struct assign));
    }
    for(int i = 0; i < num_centroids; i++){
        macqueen->centroid_data[i] = (Centroid_data) mem_allocate(sizeof(struct centroid_data));  
    }
    
    initialize_macqueen(macqueen, data_size, num_centroids);
    initialize_centroid_data(macqueen, centroids, vector_size);
    initialize_assign(macqueen);

    return macqueen;
}

// Helping function to update the centroids after new assignments
static void update_centroids(Macqueen macqueen, uint8_t** p_array, int vector_size){
    int num_centroids = macqueen->num_centroids;
    int data_size = macqueen->data_size;

    for(int i = 0; i < num_centroids; i++){
        if(macqueen->centroid_data[i]->total_data == 0){
            continue;
        }
        for(int j = 0; j < vector_size; j++){
            macqueen->centroid_data[i]->coordinates[j] = 0; // If possible to zero all coordinates to get the new centroid
        }
    }
    for(int i = 0; i < data_size; i++){
        int centroid_index = macqueen->assignment_array[i]->assigned_to_centroid;
        if(centroid_index == -1){
            continue;
        }
        for(int j = 0; j < vector_size; j++){
            macqueen->centroid_data[centroid_index]->coordinates[j] += (double) p_array[i][j];  // Getting all needed values
        }
    }
    for(int i = 0; i < num_centroids; i++){
        if(macqueen->centroid_data[i]->total_data == 0){
            continue;
        }
        for(int j = 0; j < vector_size; j++){
            macqueen->centroid_data[i]->coordinates[j] /= macqueen->centroid_data[i]->total_data;    // Calculating mean value
        }
    }
}

// Helping function to know if centroids changed or not
static int check_centroid_difference(Macqueen macqueen, double** previous_centroids, int num_centroids, int vector_size){
    int difference = 0;
    for(int i = 0; i < num_centroids; i++){
        for(int j = 0; j < vector_size; j++){
            if(macqueen->centroid_data[i]->coordinates[j] != previous_centroids[i][j]){     // If there is a change then no need to 
                difference++;                                                               // continue
                break;
            }
        }
        if(difference != 0){                                                                // But before leaving the loop copy paste the new centroids
            for(int j = 0; j < num_centroids; j++){                                         // to the array so it is updated with the new centroids 
                memcpy(previous_centroids[j], macqueen->centroid_data[j]->coordinates, sizeof(double) * vector_size);
            }
            break;
        }
    }
    return difference;
}

// Function to return radious needed
static double get_radious(Macqueen macqueen, int num_centroids, int vector_size){
    double min_distance = INFINITY;
    for(int i = 0; i < num_centroids; i++){
        for(int j = (i + 1); j < num_centroids; j++){
            double distance = calculate_distance(get_centroid_coordinates(macqueen, i), get_centroid_coordinates(macqueen, j), vector_size, METRIC_2DOUBLE);
            if(distance < min_distance){
                min_distance = distance;
            }
        } 
    }
    return (min_distance / 2);
}

// Usefull functions to update the second closest centroid index
static void update_second(Macqueen macqueen, int assignment_index, int second_closest_centroid){
    macqueen->assignment_array[assignment_index]->second_closest_centroid = second_closest_centroid;
}

void destroy_macqueen(Macqueen macqueen){
    for(int i = 0; i < macqueen->num_centroids; i++){
        mem_free(macqueen->centroid_data[i]->coordinates);
        mem_free(macqueen->centroid_data[i]);
    }
    for(int i = 0; i < macqueen->data_size; i++){
        mem_free(macqueen->assignment_array[i]);
    }
    mem_free(macqueen->assignment_array);
    mem_free(macqueen->centroid_data);
    mem_free(macqueen);
}

Macqueen lighting_macqueen(Hashtable* hashtables, Random_values* hash_random_values, Hypercube hypercube, Random_values cube_random_values, uint8_t** p_array, double** centroids, int num_centroids, int number_of_vector_hash_tables, int number_of_vector_hash_functions, int max_number_M_hypercube, int number_of_hypercube_dimensions, int number_of_probes, int data_size, int vector_size, char* method){
    Macqueen macqueen = create_macqueen(centroids, num_centroids, data_size, vector_size); 

    double R = 0.0;
    int* old_size = NULL;
    double** previous_centroids = NULL;
    if((hashtables != NULL) || (hypercube != NULL)){                                 // If there is needed the hashtable and the random values will be created
        R = get_radious(macqueen, num_centroids, vector_size);      // Taking the min/2 as the radious from the initialized centroids from kmeans++
        old_size = (int*) mem_allocate(sizeof(int) * num_centroids);
        previous_centroids = (double**) mem_allocate(sizeof(double*) * num_centroids);
    }

    if(previous_centroids != NULL){
        for(int i = 0; i < num_centroids; i++){
            previous_centroids[i] = (double*) mem_allocate(sizeof(double) * vector_size);
            memcpy(previous_centroids[i], macqueen->centroid_data[i]->coordinates, sizeof(double) * vector_size);   // Copy the initialized centroid array to another
        }                                                                                                           // so there will be an
    }

    int max_iterations = 0;
    while(1){
        bool flag = false;
        if(strcmp(method, "Classic") == 0){
            lloyd(macqueen, p_array, num_centroids, data_size, vector_size, &flag);
            if(!flag){
                return macqueen;
            }
            continue;
        }else if(strcmp(method, "LSH") == 0){             
            lsh_reverse_search(hashtables, hash_random_values, macqueen, p_array, num_centroids, number_of_vector_hash_tables, number_of_vector_hash_functions, data_size, vector_size, old_size, R, &flag);
        }else if(strcmp(method, "Hypercube") == 0){
            cube_reverse_search(hypercube, cube_random_values, macqueen, p_array, num_centroids, max_number_M_hypercube, number_of_hypercube_dimensions, number_of_probes, data_size,  vector_size, R, &flag);
        }

        /**** Centers updating ****/
        
        update_centroids(macqueen, p_array, vector_size);

        if(max_iterations > 0){
            if(check_centroid_difference(macqueen, previous_centroids, num_centroids, vector_size) == 0){    // If no changes to centroids no need to continue
                break;
            }
        }

        if(flag){   // Flag from reverse to know if no balls get no new point so stop it
            break;
        }else{
            R *= 2;
        }
        
        max_iterations++;
        if(max_iterations == ITERATIONS){   // Upper bound of iterations
            break;
        }
    }

    for(int i = 0; i < data_size; i++){
        if(get_assignment_to_centroid(macqueen, i) == -1){                                                                  // Ending of algorithm doesnt mean that
            int centroid_index = -1;                                                                                        // all data have been assigned
            double min_distance = INFINITY;                                                                                 // so if something isnt assigned
            for(int j = 0; j < num_centroids; j++){                                                                         // go and find the nearest centroid 
                double distance = calculate_distance(p_array[i], get_centroid_coordinates(macqueen, j), vector_size, METRIC_DOUBLE);// to it and assign it
                if(distance < min_distance){
                    min_distance = distance;
                    centroid_index = j; 
                }
            }
            update_assign(macqueen, i, centroid_index, -1, min_distance);
            set_centroid_total_data(macqueen, centroid_index, true);
        }
    }

    update_centroids(macqueen, p_array, vector_size);   // If something has been assigned above then need to update the new centroids

    for(int i = 0; i < data_size; i++){
        int second_nearest_centroid = -1;
        double second_distance_to_centroid = INFINITY;          // Finding and updating the second nearest neighbour of every data point
        for(int j = 0; j < num_centroids; j++){
            if(j == get_assignment_to_centroid(macqueen, i)){
                continue;
            }
            double distance = calculate_distance(p_array[i], get_centroid_coordinates(macqueen, j), vector_size, METRIC_DOUBLE);
            if(distance < second_distance_to_centroid){
                second_distance_to_centroid = distance;
                second_nearest_centroid = j;
            }
        }
        update_second(macqueen, i, second_nearest_centroid);
    }

    if((hashtables != NULL) || (hypercube != NULL)){
        mem_free(old_size);
        for(int i = 0; i < num_centroids; i++){
            mem_free(previous_centroids[i]);
        }
        mem_free(previous_centroids);
    }

    return macqueen;
}

void update_assign(Macqueen macqueen, int assignment_index, int centroid_index, int second_closest_centroid, double distance){
    macqueen->assignment_array[assignment_index]->second_closest_centroid = second_closest_centroid;
    macqueen->assignment_array[assignment_index]->assigned_to_centroid = centroid_index;
    macqueen->assignment_array[assignment_index]->distance_to_centroid = distance;
}

double get_distance_to_centroid(Macqueen macqueen, int assignment_index){
    return macqueen->assignment_array[assignment_index]->distance_to_centroid;
}

int get_assignment_to_centroid(Macqueen macqueen, int assignment_index){
    return macqueen->assignment_array[assignment_index]->assigned_to_centroid;
}

int get_second_closest_centroid(Macqueen macqueen, int assignment_index){
    return macqueen->assignment_array[assignment_index]->second_closest_centroid;
}

int get_centroid_total_data(Macqueen macqueen, int centroid_index){
    return macqueen->centroid_data[centroid_index]->total_data;
}

int get_number_of_clusters(Macqueen macqueen){
    return macqueen->num_centroids;
}

void set_centroid_total_data(Macqueen macqueen, int centroid_index, bool flag){
    if(flag){
        macqueen->centroid_data[centroid_index]->total_data++;
    }else{
        macqueen->centroid_data[centroid_index]->total_data--;
    }
}

double* get_centroid_coordinates(Macqueen macqueen, int centroid_index){
    return macqueen->centroid_data[centroid_index]->coordinates;
}