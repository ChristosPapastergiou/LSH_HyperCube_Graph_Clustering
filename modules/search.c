#include "../include/search.h"

/** LSH **/

List* lsh_approximate_kNN(Hashtable* hashtables, uint8_t** q_array, int** bucket, int L, int N, int vector_size, int query_size, struct timeval* start_time, struct timeval* end_time){
    List* list_array = (List*) mem_allocate(sizeof(List) * query_size);
    for(int i = 0; i < query_size; i++){
        list_array[i] = create_list(N);
        for(int j = 0; j < N; j++){
            insert_list(list_array[i], NULL, INFINITY, -1);
        }
    }    

    double distance;
    for(int i = 0; i < query_size; i++){   
        gettimeofday(&start_time[i], NULL);
        for(int a = 0; a < L; a++){

            int* data_numbers = (int*) mem_allocate(sizeof(int) * (get_bucket_size(hashtables[a], bucket[a][i])));
            uint8_t** p_array = (uint8_t**) mem_allocate(sizeof(uint8_t*) * (get_bucket_size(hashtables[a], bucket[a][i])));

            get_hashtable_data(hashtables[a], p_array, data_numbers, bucket[a][i]);
            for(int j = 0; j < get_bucket_size(hashtables[a], bucket[a][i]); j++){
                distance = calculate_distance(q_array[i], p_array[j], vector_size, METRIC);
                if(distance < get_last_distance(list_array[i])){
                    insert_sorted_list(list_array[i], p_array[j], distance, data_numbers[j]);
                }
            }

            mem_free(p_array);
            mem_free(data_numbers);

        }
        gettimeofday(&end_time[i], NULL);
    }
    return list_array;
}

List* lsh_approximate_range_search(Hashtable* hashtables, uint8_t** q_array, int** bucket, int L, int N, int vector_size, int query_size, double R){
    List* list_array = (List*) mem_allocate(sizeof(List) * query_size);
    for(int i = 0; i < query_size; i++){
        list_array[i] = create_list(R_NEIGHBOURS);
    }    

    double distance;
    for(int i = 0; i < query_size; i++){    
        int stop = 0;
        for(int a = 0; a < L; a++){
            int* data_numbers = (int*) mem_allocate(sizeof(int) * (get_bucket_size(hashtables[a], bucket[a][i])));
            uint8_t** p_array = (uint8_t**) mem_allocate(sizeof(uint8_t*) * (get_bucket_size(hashtables[a], bucket[a][i])));

            get_hashtable_data(hashtables[a], p_array, data_numbers, bucket[a][i]);
            for(int j = 0; j < get_bucket_size(hashtables[a], bucket[a][i]); j++){
                distance = calculate_distance(q_array[i], p_array[j], vector_size, METRIC);
                if(distance < R){
                    insert_list(list_array[i], p_array[j], distance, data_numbers[j]);
                    stop++;
                }
                if(stop == R_NEIGHBOURS){
                    break;
                }
            }
            mem_free(p_array);
            mem_free(data_numbers);

            if(stop == R_NEIGHBOURS){
                break;
            }
        }
    }
    return list_array;
}

/** Cube **/

List* cube_approximate_kNN(Hypercube hypercube, uint8_t** q_array, int** coordinates, int M, int N, int vector_size, int query_size, struct timeval* start_time, struct timeval* end_time){
    List* list_array = (List*) mem_allocate(sizeof(List) * query_size);
    for(int i = 0; i < query_size; i++){
        list_array[i] = create_list(N);
        for(int j = 0; j < N; j++){
            insert_list(list_array[i], NULL, INFINITY, -1);
        }
    }
    
    double distance;
    for(int i = 0; i < query_size; i++){
        gettimeofday(&start_time[i], NULL);

        int vertex_index = get_vertex_index(hypercube, coordinates[i]);
        int total_points = get_total_points(hypercube, vertex_index);

        int points_to_check = M;
        if(total_points < M){
            points_to_check = total_points;
        }
        
        int* image_numbers = (int*) mem_allocate(sizeof(int) * points_to_check);
        uint8_t** p_array = (uint8_t**) mem_allocate(sizeof(uint8_t*) * points_to_check);

        get_hypercube_data(hypercube, p_array, image_numbers, vertex_index, points_to_check);
        for(int j = 0; j < points_to_check; j++){
            distance = calculate_distance(q_array[i], p_array[j], vector_size, METRIC);
            if(distance < get_last_distance(list_array[i])){
                insert_sorted_list(list_array[i], p_array[j], distance, image_numbers[j]);
            }
        }

        mem_free(p_array);
        mem_free(image_numbers);

        gettimeofday(&end_time[i], NULL);
    }
    return list_array;
}

List* cube_approximate_range_search(Hypercube hypercube, uint8_t** q_array, int** coordinates, int M, int N, int vector_size, int query_size, double R){
    List* list_array = (List*) mem_allocate(sizeof(List) * query_size);
    for(int i = 0; i < query_size; i++){
        list_array[i] = create_list(R_NEIGHBOURS);
    }

    double distance;
    for(int i = 0; i < query_size; i++){
        int vertex_index = get_vertex_index(hypercube, coordinates[i]);
        int total_points = get_total_points(hypercube, vertex_index);

        int points_to_check = M;
        if(total_points < M){
            points_to_check = total_points;
        }
        
        int stop = 0;
        int* image_numbers = (int*) mem_allocate(sizeof(int) * points_to_check);
        uint8_t** p_array = (uint8_t**) mem_allocate(sizeof(uint8_t*) * points_to_check);

        get_hypercube_data(hypercube, p_array, image_numbers, vertex_index, points_to_check);
        for(int j = 0; j < points_to_check; j++){
            distance = calculate_distance(q_array[i], p_array[j], vector_size, METRIC);
            if(distance < R){
                insert_list(list_array[i], p_array[j], distance, image_numbers[j]);
                stop++;
            }
            if(stop == R_NEIGHBOURS){
                break;
            }
        }

        mem_free(p_array);
        mem_free(image_numbers);
    }        
    return list_array;
}

/** Cluster **/

List* cluster_lsh_approximate_range_search(Hashtable* hashtables, Macqueen macqueen, int** bucket, int L, int vector_size, int num_centroids, double R){
    List* list_array = (List*) mem_allocate(sizeof(List) * num_centroids);
    for(int i = 0; i < num_centroids; i++){
        list_array[i] = create_list(R_CLUSTERS);
    }    

    double distance;
    for(int i = 0; i < num_centroids; i++){    
        int stop = 0;
        for(int a = 0; a < L; a++){
            
            int* data_numbers = (int*) mem_allocate(sizeof(int) * (get_bucket_size(hashtables[a], bucket[a][i])));
            uint8_t** p_array = (uint8_t**) mem_allocate(sizeof(uint8_t*) * (get_bucket_size(hashtables[a], bucket[a][i])));

            get_hashtable_data(hashtables[a], p_array, data_numbers, bucket[a][i]);
            for(int j = 0; j < get_bucket_size(hashtables[a], bucket[a][i]); j++){
                distance = calculate_distance(p_array[j], get_centroid_coordinates(macqueen, i), vector_size, METRIC_DOUBLE);
                if(distance < R){
                    insert_list(list_array[i], p_array[j], distance, data_numbers[j]);
                    stop++;
                }
                if(stop == R_CLUSTERS){
                    break;
                }
            }

            mem_free(p_array);
            mem_free(data_numbers);

            if(stop == R_CLUSTERS){
                break;
            }
        }
    }
    return list_array;
}

List* cluster_cube_approximate_range_search(Hypercube hypercube, Macqueen macqueen, int** coordinates, int M, int vector_size, int num_centroids, double R){
    List* list_array = (List*) mem_allocate(sizeof(List) * num_centroids);
    for(int i = 0; i < num_centroids; i++){
        list_array[i] = create_list(R_CLUSTERS);
    }

    double distance;
    for(int i = 0; i < num_centroids; i++){
        int vertex_index = get_vertex_index(hypercube, coordinates[i]);
        int total_points = get_total_points(hypercube, vertex_index);

        int points_to_check = M;
        if(total_points < M){
            points_to_check = total_points;
        }
        
        int stop = 0;
        int* image_numbers = (int*) mem_allocate(sizeof(int) * points_to_check);
        uint8_t** p_array = (uint8_t**) mem_allocate(sizeof(uint8_t*) * points_to_check);

        get_hypercube_data(hypercube, p_array, image_numbers, vertex_index, points_to_check);
        for(int j = 0; j < points_to_check; j++){
            distance = calculate_distance(p_array[j], get_centroid_coordinates(macqueen, i), vector_size, METRIC_DOUBLE);
            if(distance < R){
                insert_list(list_array[i], p_array[j], distance, image_numbers[j]);
                stop++;
            }
            if(stop == R_CLUSTERS){
                break;
            }
        }

        mem_free(p_array);
        mem_free(image_numbers);
    }        
    return list_array;
}

/** Graph **/

List gNN_search(Graph graph, uint8_t* point, int N, int R, int E, int greedy_steps, int vector_size){
    List S = create_list(N);

    int* expanded_data_indexes = (int*) mem_allocate(sizeof(int) * E);
    for(int i = 0; i < R; i++){
        int y = rand() % get_graph_size(graph);                                                     // Selecting a random data point as y
        double y_distance = calculate_distance(point, get_point(graph, y), vector_size, METRIC);    // and calculating it's distance
        for(int j = 0; j < greedy_steps; j++){
            int closest_index = -1;
            double min_distance = INFINITY;
            get_edges(graph, expanded_data_indexes, y, E);
            for(int k = 0; k < E; k++){ 
                if(check_if_checked(S, expanded_data_indexes[k]) == true){
                    continue;
                }

                double distance = calculate_distance(point, get_point(graph, expanded_data_indexes[k]), vector_size, METRIC);
                if(distance < min_distance){
                    min_distance = distance;
                    closest_index = expanded_data_indexes[k];
                }
                if(get_list_size(S) == N){
                    increase_list_capacity(S);
                }
                
                insert_sorted_list(S, get_point(graph, expanded_data_indexes[k]), distance, expanded_data_indexes[k]);
            }
            if(y_distance < min_distance){
                break;
            }
            
            y = closest_index;          // The closest found data point
            y_distance = min_distance;  // will be the new y

            update_checked(S, y);       // After selecting the new y update it
        }
    }

    mem_free(expanded_data_indexes);

    return S;
}

List gg_search(Graph graph, uint8_t* point, int start_node, int N, int l, int vector_size){
    List R = create_list(N);

    double distance = calculate_distance(point, get_point(graph, start_node), vector_size, METRIC);
    insert_sorted_list(R, get_point(graph, start_node), distance, start_node);

    int i = 0;
    while(i < l){
        int p = -1;
        for(int j = 0; j < get_list_size(R); j++){
            if(check_if_checked(R, get_image(R, j)) == false){
                p = get_image(R, j);
                break;
            }
        }
        if(p == -1){
            break;
        }
        update_checked(R, p);

        int* neighbour_indexes = (int*) mem_allocate(sizeof(int) * get_neighbours(graph, p));
        get_edges(graph, neighbour_indexes, p, get_neighbours(graph, p));
        for(int j = 0; j < get_neighbours(graph, p); j++){
            double distance = calculate_distance(point, get_point(graph, neighbour_indexes[j]), vector_size, METRIC);
            insert_sorted_list(R, get_point(graph, neighbour_indexes[j]), distance, neighbour_indexes[j]);
            i++;
        }
        mem_free(neighbour_indexes);
    }
    
    return R;
}

/** General **/

List* exausted_search(uint8_t** q_array, uint8_t** p_array, int vector_size, int query_size, int data_size, int N, struct timeval* start_time, struct timeval* end_time){
    List* list_array = (List*) mem_allocate(sizeof(List) * query_size);
    for(int i = 0; i < query_size; i++){
        list_array[i] = create_list(N);
        for(int j = 0; j < N; j++){
            insert_list(list_array[i], NULL, INFINITY, -1);
        }
    } 

    int* data_numbers = (int*) mem_allocate(sizeof(int) * data_size);
    for(int i = 0; i < data_size; i++){
        data_numbers[i] = i;
    }

    double distance = 0;
    for(int i = 0; i < query_size; i++){
        gettimeofday(&start_time[i], NULL);
        for(int j = 0; j < data_size; j++){
            distance = calculate_distance(q_array[i], p_array[j], vector_size, METRIC);
            if(distance < get_last_distance(list_array[i])){
                insert_sorted_list(list_array[i], p_array[i], distance, data_numbers[j]);
            }
        }
        gettimeofday(&end_time[i], NULL);
    }
    
    mem_free(data_numbers);
    
    return list_array;
}