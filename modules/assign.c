#include "../include/list.h"
#include "../include/assign.h"
#include "../include/random.h"
#include "../include/search.h"
#include "../include/hashtable.h"
#include "../include/hypercube.h"

void lsh_reverse_search(Hashtable* hashtables, Random_values* random_values, Macqueen macqueen, uint8_t** p_array, int num_centroids, int L, int  k, int data_size, int vector_size, int* old_size, double R, bool* flag){
    int** bucket = (int**) mem_allocate(sizeof(int*) * L);
    for(int i = 0; i < L; i++){
        bucket[i] = (int*) mem_allocate(sizeof(int) * num_centroids);
    }

    uint32_t* h_array = (uint32_t*) mem_allocate(sizeof(uint32_t) * k);
    for(int a = 0; a < L; a++){
        for(int i = 0; i < num_centroids; i++){
            random_h_values_double(h_array, get_centroid_coordinates(macqueen, i), get_v_values(random_values[a]), get_t_values(random_values[a]), k, W, vector_size);
            uint32_t id = random_id_values(h_array, get_r_values(random_values[a]), k);
            bucket[a][i] = id % get_hashtable_size(hashtables[a]);
        }
    }

    int changes = 0;
    
    List* range = cluster_lsh_approximate_range_search(hashtables, macqueen, bucket, L, vector_size, num_centroids, R);

    for(int i = 0; i < num_centroids; i++){
        if(old_size[i] != get_list_size(range[i])){ // Check if the previous size of list has not been changed
            changes++;                              // because this means that there are no new points added with the new radious R
        }
    }
    if(changes == 0){
        (*flag) = true;

        for(int i = 0; i < num_centroids; i++){
            destroy_list(range[i]);
        }
        mem_free(range);
        mem_free(h_array);
        for(int i = 0; i < L; i++){
            mem_free(bucket[i]);
        }
        mem_free(bucket);

        return;
    }
    for(int i = 0; i < num_centroids; i++){
        old_size[i] = get_list_size(range[i]);
    }

    int** marked_to = (int**) mem_allocate(sizeof(int*) * data_size);
    for(int i = 0; i < data_size; i++){
        marked_to[i] = (int*) mem_allocate(sizeof(int) * num_centroids);
    }
    for(int i = 0; i < num_centroids; i++){
        int size = get_list_size(range[i]);
        for(int j = 0; j < size; j++){
            int data_index = get_image(range[i], j);
            if(get_assignment_to_centroid(macqueen, data_index) != -1){ // If a point had been assigned previously no need to change it again
                continue;
            }
            marked_to[data_index][i] = 1;   // So mark all the point that needs to be assigned
        }
    }

    for(int i = 0; i < num_centroids; i++){
        int size = get_list_size(range[i]);
        for(int j = 0; j < size; j++){
            int data_index = get_image(range[i], j);
            if(marked_to[data_index][i] == 1){                                                                  // If an index is marked its okey to 
                if(get_assignment_to_centroid(macqueen, data_index) == -1){                                     // continue to the assignment
                    update_assign(macqueen, data_index, i, -1, get_distance(range[i], j));                      // Also if a point has not been assigned
                    set_centroid_total_data(macqueen, i, true);                                                 // it will be. If it was already assigned
                }else if(get_distance(range[i], j) < get_distance_to_centroid(macqueen, data_index)){           // there is a confict so check the
                    set_centroid_total_data(macqueen, get_assignment_to_centroid(macqueen, data_index), false); // respective distances to centroids and
                    update_assign(macqueen, data_index, i, -1, get_distance(range[i], j));                      // assign it to the closest one 
                    set_centroid_total_data(macqueen, i, true);
                }
            }
        }
    }

    /**** Frees ****/
    
    for(int i = 0; i < data_size; i++){
        mem_free(marked_to[i]);
    }
    mem_free(marked_to);

    for(int i = 0; i < num_centroids; i++){
        destroy_list(range[i]);
    }
    mem_free(range);

    mem_free(h_array);
    for(int i = 0; i < L; i++){
        mem_free(bucket[i]);
    }
    mem_free(bucket);
}

void cube_reverse_search(Hypercube hypercube,Random_values cube_random_values,Macqueen macqueen, uint8_t** p_array, int num_centroids, int M, int k, int probes, int data_size, int vector_size, double R, bool* flag){
    int** coordinates = (int**) mem_allocate(sizeof(int*) * num_centroids);
    for(int i = 0; i < num_centroids; i++){
        coordinates[i] = (int*) mem_allocate(sizeof(int) * k);
    }
    uint32_t* h_array = (uint32_t*) mem_allocate(sizeof(uint32_t) * k);
    for(int i = 0; i < num_centroids; i++){
        random_h_values_double(h_array, get_centroid_coordinates(macqueen, i), get_v_values(cube_random_values), get_t_values(cube_random_values), k, W, vector_size);
        generate_point_coordinates(h_array, coordinates[i], k);
    }

    int changes = 0;

    int* old_size = (int*) mem_allocate(sizeof(int) * num_centroids);
    for(int i = 0; i < num_centroids; i++){
        old_size[i] = get_centroid_total_data(macqueen, i);
    }

    List* range = cluster_cube_approximate_range_search(hypercube, macqueen, coordinates, M,  vector_size, num_centroids, R);

    for(int i = 0; i < num_centroids; i++){
        if(old_size[i] == get_list_size(range[i])){ // Check if the previous size of list has not been changed
            changes++;                              // because this means that there are no new points added with the new radious R
        }
    }
    if(changes == 0){
        (*flag) = true;

        for(int i = 0; i < num_centroids; i++){
        destroy_list(range[i]);
        }
        mem_free(range);

        mem_free(h_array);
        mem_free(old_size);

        for(int i = 0; i < num_centroids; i++){
            mem_free(coordinates[i]);
        }
        mem_free(coordinates);

        return;
    }

    int** marked_to = (int**) mem_allocate(sizeof(int*) * data_size);
    for(int i = 0; i < data_size; i++){
        marked_to[i] = (int*) mem_allocate(sizeof(int) * num_centroids);
    }
    for(int i = 0; i < num_centroids; i++){
        int size = get_list_size(range[i]);
        for(int j = 0; j < size; j++){
            int data_index = get_image(range[i], j);
            if(get_assignment_to_centroid(macqueen, data_index) != -1){ // If a point had been assigned previously no need to change it again
                continue;
            }
            marked_to[data_index][i] = 1;   // So mark all the point that needs to be assigned
        }
    }

    for(int i = 0; i < num_centroids; i++){
        int size = get_list_size(range[i]);
        for(int j = 0; j < size; j++){
            int data_index = get_image(range[i], j);
            if(marked_to[data_index][i] == 1){                                                                  // If an index is marked its okey to 
                if(get_assignment_to_centroid(macqueen, data_index) == -1){                                     // continue to the assignment
                    update_assign(macqueen, data_index, i, -1, get_distance(range[i], j));                      // Also if a point has not been assigned
                    set_centroid_total_data(macqueen, i, true);                                                 // it will be. If it was already assigned
                }else if(get_distance(range[i], j) < get_distance_to_centroid(macqueen, data_index)){           // there is a confict so check the
                    set_centroid_total_data(macqueen, get_assignment_to_centroid(macqueen, data_index), false); // respective distances to centroids and
                    update_assign(macqueen, data_index, i, -1, get_distance(range[i], j));                      // assign it to the closest one 
                    set_centroid_total_data(macqueen, i, true);
                }
            }
        }
    }

    /**** Frees ****/

    for(int i = 0; i < data_size; i++){
        mem_free(marked_to[i]);
    }
    mem_free(marked_to);

    for(int i = 0; i < num_centroids; i++){
        destroy_list(range[i]);
    }
    mem_free(range);

    mem_free(h_array);
    mem_free(old_size);

    for(int i = 0; i < num_centroids; i++){
        mem_free(coordinates[i]);
    }
    mem_free(coordinates);
}

void lloyd(Macqueen macqueen, uint8_t** p_array, int num_centroids, int data_size, int vector_size, bool* flag){
    for(int i = 0; i < data_size; i++){
        int closest_centroid_index = -1;
        int second_closest_centroid_index = -1;

        double min_distance = INFINITY;
        double second_min_distance = INFINITY;

        int previous = get_assignment_to_centroid(macqueen, i);
        for(int j = 0; j < num_centroids; j++){                                                                             
            double distance = calculate_distance(p_array[i], get_centroid_coordinates(macqueen, j), vector_size, METRIC_DOUBLE);
            if(distance < min_distance){
                second_min_distance = min_distance;                     // For every data we want to find it's minimum distance from a centroid 
                min_distance = distance;                                // but also the 2nd nearest distance too to use it on the silhouette
                second_closest_centroid_index = closest_centroid_index; 
                closest_centroid_index = j;
            }else if(distance < second_min_distance){
                second_min_distance = distance;
                second_closest_centroid_index = j;
            }
        }

        if(previous != closest_centroid_index){
            update_assign(macqueen, i, closest_centroid_index, second_closest_centroid_index, min_distance);    // Update assignment array
            (*flag) = true;

            int current = get_assignment_to_centroid(macqueen, i);
            if(current != previous){
                int current_total_data = get_centroid_total_data(macqueen, current);
                double* current_coordinates = get_centroid_coordinates(macqueen, current);

                if(current_total_data == 0){                            // Nothing is inside
                    set_centroid_total_data(macqueen, current, true);   // Just put it in and increase the total counter
                    for(int j = 0; j < vector_size; j++){
                        double point = (double) p_array[i][j];
                        current_coordinates[j] += point;
                        current_coordinates[j] /= 2;
                    }
                    continue;
                }

                if(previous != -1){    
                    int previous_total_data = get_centroid_total_data(macqueen, previous);
                    double* previous_coordinates = get_centroid_coordinates(macqueen, previous);
                    for(int j = 0; j < vector_size; j++){
                        current_coordinates[j] *= current_total_data;       // Previously the vector was divided with the previously total data so we need to 
                        previous_coordinates[j] *= previous_total_data;     // "normalize" it first with the previous total data so we have the "real" numbers
                                                                                                        
                        previous_coordinates[j] -= p_array[i][j];           // Having the "normal" values now remove the data and divide it by the new total data
                        previous_coordinates[j] /= (get_centroid_total_data(macqueen, previous) - 1);   // New total data is 1 less
                    
                        current_coordinates[j] += p_array[i][j];            // Having the "normal" values now add the data and divide it by the new total data
                        current_coordinates[j] /= (get_centroid_total_data(macqueen, current) + 1);     // New total data is 1 more
                    }
                    set_centroid_total_data(macqueen, current, true);       // Finally increasing the total
                    set_centroid_total_data(macqueen, previous, false);     // data as a new data was added or being removed
                }else{                                                      // In this case need only the current...previous doesnt exist
                    for(int j = 0; j < vector_size; j++){
                        current_coordinates[j] *= current_total_data;

                        current_coordinates[j] += p_array[i][j];
                        current_coordinates[j] /= (get_centroid_total_data(macqueen, current) + 1);
                    }
                    set_centroid_total_data(macqueen, current, true);
                }
            }
        }
    }
}