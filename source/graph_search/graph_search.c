#include "graph_search.h"

// Parsing the given input
static void parse_args(int argc, char** argv, char* input_file_name, char* query_file_name, char* output_file_name, int* k, int* E, int* R, int* N, int* l, int* m){
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-d") == 0){
            i++;
            if(i < argc){
                strcpy(input_file_name, argv[i]);
            }
        }else if(strcmp(argv[i], "-q") == 0){
            i++;
            if(i < argc){
                strcpy(query_file_name, argv[i]);
            }        
        }else if(strcmp(argv[i], "-k") == 0){   
            i++;
            if(i < argc){
                (*k) = atoi(argv[i]);
            }        
        }else if(strcmp(argv[i], "-E") == 0){   
            i++;
            if(i < argc){
                (*E) = atoi(argv[i]);
            }     
        }else if(strcmp(argv[i], "-R") == 0){   
            i++;
            if(i < argc){
                (*R) = atoi(argv[i]);
            }  
        }else if(strcmp(argv[i], "-N") == 0){   
            i++;
            if(i < argc){
                (*N) = atoi(argv[i]);
            }   
        }else if(strcmp(argv[i], "-l") == 0){   
            i++;
            if(i < argc){
                (*l) = atoi(argv[i]);
            } 
        }else if(strcmp(argv[i], "-m") == 0){   
            i++;
            if(i < argc){
                (*m) = atoi(argv[i]);
            } 
        }else if(strcmp(argv[i], "-o") == 0){
            i++;
            if(i < argc){
                strcpy(output_file_name, argv[i]);
            }        
        }    
    }
}

// Brute force helping function to find the navigation node
static int brute_force(uint8_t** p_array, double* point, int data_size, int vector_size){
    int index;
    double min_distance = INFINITY;
    for(int j = 0; j < data_size; j++){
        double distance = calculate_distance(p_array[j], point, vector_size, METRIC_DOUBLE);    // For every distance saving the minimum one
        if(distance < min_distance){
            min_distance = distance;
            index = j;
        }
    }
    return index;
}

static int gnn(int argc, char* input_file_name, char* query_file_name, char* output_file_name, int k, int E, int R, int N){
    if(E > k){
        printf(" [ E must be less or equal to k ] \n");
        return EXIT_FAILURE;
    }

    uint32_t magic_number = 0, num_of_images = 0, num_of_rows = 0, num_of_cols = 0; 

    /**** Input file ****/
    
    if((*input_file_name) == 0){
        printf(" [ Please provide a dataset file ] \n");
        scanf("%s", input_file_name);
    }
    FILE* input_file = fopen(input_file_name, "rb");
    if(input_file == NULL) {
        printf("Error opening input file\n");
    }

    fread(&magic_number, sizeof(uint32_t), 1, input_file);
    fread(&num_of_images, sizeof(uint32_t), 1, input_file);
    fread(&num_of_rows, sizeof(uint32_t), 1, input_file);
    fread(&num_of_cols, sizeof(uint32_t), 1, input_file);

    magic_number = ntohl(magic_number);
    num_of_images = ntohl(num_of_images);
    num_of_rows = ntohl(num_of_rows);
    num_of_cols = ntohl(num_of_cols);

    printf("magic number is   : %d\n", (int) magic_number);
    printf("number of images  : %d\n", (int) num_of_images);
    printf("number of rows    : %d\n", (int) num_of_rows);
    printf("number of columns : %d\n", (int) num_of_cols);

    /**** Data of dataset ****/

    int data_size = num_of_images;
    int vector_size = num_of_rows * num_of_cols;

    /**** Data array and Data structure creation ****/

    uint8_t** p_array = (uint8_t**) mem_allocate(sizeof(uint8_t*) * data_size);
    for(int i = 0; i < data_size; i++){
        p_array[i] = (uint8_t*) mem_allocate(sizeof(uint8_t) * vector_size);
    }
    for(int i = 0; i < data_size; i++){
        for(int j = 0; j < vector_size; j++){
            fread(&p_array[i][j], sizeof(uint8_t), 1, input_file);
        }
    }
    fclose(input_file);

    /**** Hashtable array ****/

    Hashtable* hashtables = (Hashtable*) mem_allocate(sizeof(Hashtable) * NUM_HASHTABLES);
    for(int i = 0; i < NUM_HASHTABLES; i++){
        hashtables[i] = create_hashtable(data_size);
    }

    /**** Values ****/

    Random_values* random_values = (Random_values*) mem_allocate(sizeof(Random_values) * NUM_HASHTABLES);
    for(int i = 0; i < NUM_HASHTABLES; i++){
        random_values[i] = create_random_values(RANDOM_VALUES, W, vector_size);
    }

    /**** Hashtable bucket & insertion ****/

    int** bucket = (int**) mem_allocate(sizeof(int*) * NUM_HASHTABLES);
    for(int i = 0; i < NUM_HASHTABLES; i++){
        bucket[i] = (int*) mem_allocate(sizeof(int) * data_size);
    }
    uint32_t* h_array = (uint32_t*) mem_allocate(sizeof(uint32_t) * RANDOM_VALUES);
    for(int a = 0; a < NUM_HASHTABLES; a++){
        for(int i = 0; i < data_size; i++){
            random_h_values(h_array, p_array[i], get_v_values(random_values[a]), get_t_values(random_values[a]), RANDOM_VALUES, W, vector_size); 
            uint32_t id = random_id_values(h_array, get_r_values(random_values[a]), RANDOM_VALUES);
            insert_hashtable(hashtables[a], p_array[i], id, i);
            bucket[a][i] = id % get_hashtable_size(hashtables[a]);
        }
    }

    /**** Approximate k Nearest neightbours ****/

    struct timeval* kNN_start_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * data_size);
    struct timeval* kNN_end_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * data_size);
    List* kNN = lsh_approximate_kNN(hashtables, p_array, bucket, NUM_HASHTABLES, k + 1, vector_size, data_size, kNN_start_time, kNN_end_time);

    /**** Graph & Connect edges ****/

    Graph graph = create_graph(p_array, data_size);

    for(int i = 0; i < data_size; i++){
        for(int j = 1; j < get_list_size(kNN[i]); j++){
            add_directed_edge(graph, i, get_image(kNN[i], j));  // Connecting the edges of neighbours
        }
    }

    /**** Query and Output file ****/

    if((*query_file_name) == 0){
        printf(" [ Please provide a query file ] \n");
        scanf("%s", query_file_name);
    }
    FILE* query_file = fopen(query_file_name, "rb");
    if(query_file == NULL) {
        printf("Error opening query file\n");
    }
    if((*output_file_name) == 0){
        printf(" [ Please provide an output file ] \n");
        scanf("%s", output_file_name);
    }
    FILE* output_file = fopen(output_file_name, "w+");
    if(output_file == NULL) {
        printf("Error opening output file\n");
    }
    
    fread(&magic_number, sizeof(uint32_t), 1, query_file);
    fread(&num_of_images, sizeof(uint32_t), 1, query_file);
    fread(&num_of_rows, sizeof(uint32_t), 1, query_file);
    fread(&num_of_cols, sizeof(uint32_t), 1, query_file);

    magic_number = ntohl(magic_number);
    num_of_images = ntohl(num_of_images);
    num_of_rows = ntohl(num_of_rows);
    num_of_cols = ntohl(num_of_cols);

    /**** Data of query ****/

    // int query_size = QUERY_SIZE;
    int query_size = num_of_images;
    
    /**** Query array ****/

    uint8_t** q_array = (uint8_t**) mem_allocate(sizeof(uint8_t*) * query_size);
    for(int i = 0; i < query_size; i++){
        q_array[i] = (uint8_t*) mem_allocate(sizeof(uint8_t) * vector_size);
    }
    for(int i = 0; i < query_size; i++){
        for(int j = 0; j < vector_size; j++){
            fread(&q_array[i][j], sizeof(uint8_t), 1, query_file);
        }
    }
    fclose(query_file);

    /**** Nearest neightbour ****/

    struct timeval* exausted_start_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * query_size);
    struct timeval* exausted_end_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * query_size);
    List* exausted = exausted_search(q_array, p_array, vector_size, query_size, data_size, N, exausted_start_time, exausted_end_time);

    /**** Graph Nearest Neighbours ****/

    struct timeval* gNN_start_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * query_size);
    struct timeval* gNN_end_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * query_size);

    List* gNN = (List*) mem_allocate(sizeof(List) * query_size);
    for(int i = 0; i < query_size; i++){
        gettimeofday(&gNN_start_time[i], NULL);
        gNN[i] = gNN_search(graph, q_array[i], N, R, E, GREEDY_STEPS, vector_size);
        gettimeofday(&gNN_end_time[i], NULL);
    }

    /**** Comparison with the real dimension ****/

    char compare_question[BUFFER];
    printf(" Was this a compressed file and you want to see the performance based on the true dimension? [Y or y] \n");
    scanf("%s", compare_question);
    if((strcmp("Y", compare_question) == 0) || (strcmp("y", compare_question) == 0)){
        FILE* file = fopen("../../data/graph_gnn_compare.csv", "w+");
        if(file == NULL){
            printf("Error opening file\n");
        }
        compare_dimension(file, exausted, gNN);
        fclose(file);
    }

    /**** Calculate times ****/

    double total_exausted = 0.0;
    double* exausted_times = (double*) mem_allocate(sizeof(double) * query_size);
    for(int i = 0; i < query_size; i++){
        double start_seconds = (double) exausted_start_time[i].tv_sec + (double) exausted_start_time[i].tv_usec / 1000000.0;
        double end_seconds = (double) exausted_end_time[i].tv_sec + (double) exausted_end_time[i].tv_usec / 1000000.0;
        exausted_times[i] = end_seconds - start_seconds;
        total_exausted += exausted_times[i]; 
    }

    double total_gNN = 0.0;
    double* gNN_times = (double*) mem_allocate(sizeof(double) * query_size);
    for(int i = 0; i < query_size; i++){
        double start_seconds = (double) gNN_start_time[i].tv_sec + (double) gNN_start_time[i].tv_usec / 1000000.0;
        double end_seconds = (double) gNN_end_time[i].tv_sec + (double) gNN_end_time[i].tv_usec / 1000000.0;
        gNN_times[i] = end_seconds - start_seconds;
        total_gNN += gNN_times[i]; 
    }

    /**** File output & result writing ****/

    double max_af = 0.0;
    double total_true_distance = 0.0;
    double total_approximate_distance = 0.0;
    fprintf(output_file, "GNNS Results\n");
    for(int i = 0; i < query_size; i++){
        fprintf(output_file, "Query : %d\n", i + 1);
        for(int j = 0; j < N; j++){
            fprintf(output_file, "Nearest neighbor - %d : %d\n ", j + 1, get_image(gNN[i], j));
            fprintf(output_file, "distanceApproximate : ");
            print_list_item(output_file, gNN[i], j);
            fprintf(output_file, " distanceTrue : ");
            print_list_item(output_file, exausted[i], j);
        }
        fprintf(output_file, "tApproximate : %f\n", gNN_times[i]);
        fprintf(output_file, "tTrue : %f\n", exausted_times[i]);
        fprintf(output_file, "AF    : %f\n", get_distance(gNN[i], 0) / get_distance(exausted[i], 0));
        if(get_distance(gNN[i], 0) / get_distance(exausted[i], 0) > max_af){
            max_af = get_distance(gNN[i], 0) / get_distance(exausted[i], 0);
        }
        total_approximate_distance += get_distance(gNN[i], 0);
        total_true_distance += get_distance(exausted[i], 0);
        fprintf(output_file, "\n");
    }
    total_approximate_distance /= query_size;
    total_true_distance /= query_size;
    fprintf(output_file, "tAverageApproximate : %f\n", total_gNN / query_size);
    fprintf(output_file, "tAverageTrue : %f\n", total_exausted / query_size);
    fprintf(output_file, "MAF : %f\n", max_af);
    fprintf(output_file, "AAF : %f\n", total_approximate_distance / total_true_distance);
    fclose(output_file);

    /**** Free memory ****/

    mem_free(exausted_start_time);
    mem_free(exausted_end_time);
    mem_free(kNN_start_time);
    mem_free(kNN_end_time);
    mem_free(gNN_start_time);
    mem_free(gNN_end_time);

    mem_free(exausted_times);
    mem_free(gNN_times);

    for(int i = 0; i < query_size; i++){
        destroy_list(exausted[i]);
        destroy_list(gNN[i]);
    }
    for(int i = 0; i < data_size; i++){
        destroy_list(kNN[i]);
    }
    mem_free(exausted);
    mem_free(gNN);
    mem_free(kNN);

    destroy_graph(graph);

    for(int i = 0; i < NUM_HASHTABLES; i++){
        mem_free(bucket[i]);
    }
    mem_free(bucket);
    mem_free(h_array);

    for(int i = 0; i < NUM_HASHTABLES; i++){
        delete_random_values(random_values[i]);
    }
    mem_free(random_values);

    for(int i = 0; i < NUM_HASHTABLES; i++){
        destroy_hashtable(hashtables[i]);
    }
    mem_free(hashtables);

    for(int i = 0; i < query_size; i++){
        mem_free(q_array[i]);
    }
    mem_free(q_array);

    for(int i = 0; i < data_size; i++){
        mem_free(p_array[i]);
    }
    mem_free(p_array);

    mem_free(input_file_name);
    mem_free(query_file_name);
    mem_free(output_file_name);

    return EXIT_SUCCESS;
}

static int mrng(int argc, char* input_file_name, char* query_file_name, char* output_file_name, int N, int l){
    if(l < N){
        printf(" [ l must be greater or equal to N ] \n");
        return EXIT_FAILURE;
    }

    uint32_t magic_number = 0, num_of_images = 0, num_of_rows = 0, num_of_cols = 0; 

    /**** Input file ****/
    
    if((*input_file_name) == 0){
        printf(" [ Please provide a dataset file ] \n");
        scanf("%s", input_file_name);
    }
    FILE* input_file = fopen(input_file_name, "rb");
    if(input_file == NULL) {
        printf("Error opening input file\n");
    }

    fread(&magic_number, sizeof(uint32_t), 1, input_file);
    fread(&num_of_images, sizeof(uint32_t), 1, input_file);
    fread(&num_of_rows, sizeof(uint32_t), 1, input_file);
    fread(&num_of_cols, sizeof(uint32_t), 1, input_file);

    magic_number = ntohl(magic_number);
    num_of_images = ntohl(num_of_images);
    num_of_rows = ntohl(num_of_rows);
    num_of_cols = ntohl(num_of_cols);

    printf("magic number is   : %d\n", (int) magic_number);
    printf("number of images  : %d\n", (int) num_of_images);
    printf("number of rows    : %d\n", (int) num_of_rows);
    printf("number of columns : %d\n", (int) num_of_cols);

    /**** Data of dataset ****/

    int data_size = num_of_images;
    int vector_size = num_of_rows * num_of_cols;

    /***** Data array and Data structure creation ****/

    uint8_t** p_array = (uint8_t**) mem_allocate(sizeof(uint8_t*) * data_size);
    for(int i = 0; i < data_size; i++){
        p_array[i] = (uint8_t*) mem_allocate(sizeof(uint8_t) * vector_size);
    }
    for(int i = 0; i < data_size; i++){
        for(int j = 0; j < vector_size; j++){
            fread(&p_array[i][j], sizeof(uint8_t), 1, input_file);
        }
    }
    fclose(input_file);

    /**** Graph ****/

    Graph graph = create_graph(p_array, data_size);

    FILE* graph_file = fopen("graph.csv", "r");
    if(graph_file == NULL){
        /**** MRNG construction ****/

        List* L = (List*) mem_allocate(sizeof(List) * data_size);   // The array of Lp lists to store the neighbours 
        for(int i = 0; i < data_size; i++){                         // to use it for edge connection 
            L[i] = create_list(data_size);
        }

        for(int i = 0; i < data_size; i++){
            Vector R = create_vector(data_size);
            
            for(int j = 0; j < data_size; j++){
                if(i == j){
                    continue;
                }
                double distance = calculate_distance(p_array[i], p_array[j], vector_size, METRIC);  // Adding to the vector Rp all the
                insert_vector(R, p_array[j], distance, j, j);                                       // data points except a some point
            }
            sort_vector(R); // Sorting Rp based on the distance

            int j = 1;                  // Counter to check a data point's distance with the next data point's distance
            int equal_distance = 1;     // Counter to store how many data point may have the same distance 
            while((get_vector_size(R) > j) && (get_image_distance_vector(R, j) == get_image_distance_vector(R, j + 1))){    // Checking if there are more than one
                equal_distance++;                                                                                           // data with the same distance (minimum)
                j++;
            }
            for(int j = 0; j <= equal_distance; j++){    
                int item = get_image_index_vector(R, j);
                double distance = get_image_distance_vector(R, j);
                if(distance == 0){
                    continue;
                }

                insert_sorted_list(L[i], p_array[item], distance, item);    // Inserting the data with the minimum distance in the Lp list
            }
            for(int j = 0; j <= equal_distance; j++){
                remove_vector(R, j);                        // Remove from Rp list the data that has been inserted in the Lp list
            }

            for(int j = 0; j < get_vector_size(R); j++){    // Every time the Rp size will not be the same
                bool condition = true;
                int r = get_image_index_vector(R, j);
                if(r < 0){
                    continue;
                }
                double pr_distance = calculate_distance(p_array[i], p_array[r], vector_size, METRIC);
                for(int k = 0; k < get_list_size(L[i]); k++){
                    int t = get_image(L[i], k);
                    double rt_distance = calculate_distance(p_array[r], p_array[t], vector_size, METRIC);
                    double tp_distance = calculate_distance(p_array[t], p_array[i], vector_size, METRIC);
                    if((pr_distance > rt_distance) && (pr_distance > tp_distance)){     // Checking if the pr distance is the highest of the triangle
                        condition = false;                                              // and in this case it must not be inserted in the Lp list 
                        break;
                    }
                }
                if(condition){
                    insert_sorted_list(L[i], p_array[r], pr_distance, r);
                }
            }
            
            destroy_vector(R);
        }

        /**** Connect edges of graph ****/

        for(int i = 0; i < data_size; i++){
            for(int j = 0; j < get_list_size(L[i]); j++){
                add_directed_edge(graph, i, get_image(L[i], j));    // Connecting the edges of neighbours
            }
        }
        save_graph(graph);  // To save time on the next run 

        for(int i = 0; i < data_size; i++){
            destroy_list(L[i]);
        }
        mem_free(L);
    }else{
        load_graph(graph, graph_file);  // If there is a file of a ready graph just use it (assumming that the dataset size is the same as the saved one)
    }

    /**** Navigating Node ****/

    double* centroid = (double*) mem_allocate(sizeof(double) * vector_size);
    for(int i = 0; i < data_size; i++){
        for(int j = 0; j < vector_size; j++){
            centroid[j] += (double) p_array[i][j];
        }
    }
    for(int i = 0; i < vector_size; i++){
        centroid[i] /= data_size;
    }
    int navigating_node_index = brute_force(p_array, centroid, data_size, vector_size); // Finding the navigating node

    /**** Query and Output file ****/

    if((*query_file_name) == 0){
        printf(" [ Please provide a query file ] \n");
        scanf("%s", query_file_name);
    }
    FILE* query_file = fopen(query_file_name, "rb");
    if(query_file == NULL){
        printf("Error opening query file\n");
    }
    if((*output_file_name) == 0){
        printf(" [ Please provide an output file ] \n");
        scanf("%s", output_file_name);
    }
    FILE* output_file = fopen(output_file_name, "w+");
    if(output_file == NULL){
        printf("Error opening output file\n");
    }
    
    fread(&magic_number, sizeof(uint32_t), 1, query_file);
    fread(&num_of_images, sizeof(uint32_t), 1, query_file);
    fread(&num_of_rows, sizeof(uint32_t), 1, query_file);
    fread(&num_of_cols, sizeof(uint32_t), 1, query_file);

    magic_number = ntohl(magic_number);
    num_of_images = ntohl(num_of_images);
    num_of_rows = ntohl(num_of_rows);
    num_of_cols = ntohl(num_of_cols);

    /**** Data of query ****/

    // int query_size = QUERY_SIZE;
    int query_size = num_of_images;
    
    /**** Query array ****/

    uint8_t** q_array = (uint8_t**) mem_allocate(sizeof(uint8_t*) * query_size);
    for(int i = 0; i < query_size; i++){
        q_array[i] = (uint8_t*) mem_allocate(sizeof(uint8_t) * vector_size);
    }
    for(int i = 0; i < query_size; i++){
        for(int j = 0; j < vector_size; j++){
            fread(&q_array[i][j], sizeof(uint8_t), 1, query_file);
        }
    }
    fclose(query_file);

    /**** Nearest neightbour ****/

    struct timeval* exausted_start_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * query_size);
    struct timeval* exausted_end_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * query_size);
    List* exausted = exausted_search(q_array, p_array, vector_size, query_size, data_size, N, exausted_start_time, exausted_end_time);

    /**** Graph Nearest Neighbours ****/

    struct timeval* mrng_start_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * query_size);
    struct timeval* mrng_end_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * query_size);

    List* mrng = (List*) mem_allocate(sizeof(List) * query_size);
    for(int i = 0; i < query_size; i++){
        gettimeofday(&mrng_start_time[i], NULL);
        mrng[i] = gg_search(graph, q_array[i], navigating_node_index, N, l, vector_size);
        gettimeofday(&mrng_end_time[i], NULL);
    }

    /**** Comparison with the real dimension ****/

    char compare_question[BUFFER];
    printf(" Was this a compressed file and you want to see the performance based on the true dimension? [Y or y] \n");
    scanf("%s", compare_question);
    if((strcmp("Y", compare_question) == 0) || (strcmp("y", compare_question) == 0)){
        FILE* file = fopen("../../data/graph_gg_compare.csv", "w+");
        if(file == NULL){
            printf("Error opening file\n");
        }
        compare_dimension(file, exausted, mrng);
        fclose(file);
    }

    /**** Calculate times ****/

    double total_exausted = 0.0;
    double* exausted_times = (double*) mem_allocate(sizeof(double) * query_size);
    for(int i = 0; i < query_size; i++){
        double start_seconds = (double) exausted_start_time[i].tv_sec + (double) exausted_start_time[i].tv_usec / 1000000.0;
        double end_seconds = (double) exausted_end_time[i].tv_sec + (double) exausted_end_time[i].tv_usec / 1000000.0;
        exausted_times[i] = end_seconds - start_seconds;
        total_exausted += exausted_times[i];
    }

    double total_mrng = 0.0;
    double* mrng_times = (double*) mem_allocate(sizeof(double) * query_size);
    for(int i = 0; i < query_size; i++){
        double start_seconds = (double) mrng_start_time[i].tv_sec + (double) mrng_start_time[i].tv_usec / 1000000.0;
        double end_seconds = (double) mrng_end_time[i].tv_sec + (double) mrng_end_time[i].tv_usec / 1000000.0;
        mrng_times[i] = end_seconds - start_seconds;
        total_mrng += mrng_times[i];
    }

    /**** File output & result writing ****/

    double max_af = 0.0;
    double total_true_distance = 0.0;
    double total_approximate_distance = 0.0;
    fprintf(output_file, "MRNG Results\n");
    for(int i = 0; i < query_size; i++){
        fprintf(output_file, "Query : %d\n", i + 1);
        for(int j = 0; j < N; j++){
            fprintf(output_file, "Nearest neighbor - %d : %d\n ", j + 1, get_image(mrng[i], j));
            fprintf(output_file, "distanceApproximate : ");
            print_list_item(output_file, mrng[i], j);
            fprintf(output_file, " distanceTrue : ");
            print_list_item(output_file, exausted[i], j);
        }
        fprintf(output_file, "tApproximate : %f\n", mrng_times[i]);
        fprintf(output_file, "tTrue : %f\n", exausted_times[i]);
        fprintf(output_file, "AF    : %f\n", get_distance(mrng[i], 0) / get_distance(exausted[i], 0));
        if(get_distance(mrng[i], 0) / get_distance(exausted[i], 0) > max_af){
            max_af = get_distance(mrng[i], 0) / get_distance(exausted[i], 0);
        }
        total_approximate_distance += get_distance(mrng[i], 0);
        total_true_distance += get_distance(exausted[i], 0);
        fprintf(output_file, "\n");
    }
    total_approximate_distance /= query_size;
    total_true_distance /= query_size;
    fprintf(output_file, "tAverageApproximate : %f\n", total_mrng / query_size);
    fprintf(output_file, "tAverageTrue : %f\n", total_exausted / query_size);
    fprintf(output_file, "MAF : %f\n", max_af);
    fprintf(output_file, "AAF : %f\n", total_approximate_distance / total_true_distance);
    fclose(output_file);

    /**** Free memory ****/

    mem_free(exausted_start_time);
    mem_free(exausted_end_time);
    mem_free(mrng_start_time);
    mem_free(mrng_end_time);

    mem_free(exausted_times);
    mem_free(mrng_times);

    for(int i = 0; i < query_size; i++){
        destroy_list(exausted[i]);
        destroy_list(mrng[i]);
    }
    mem_free(exausted);
    mem_free(mrng);

    for(int i = 0; i < query_size; i++){
        mem_free(q_array[i]);
    }
    mem_free(q_array);
    mem_free(centroid);

    destroy_graph(graph);

    for(int i = 0; i < data_size; i++){
        mem_free(p_array[i]);
    }
    mem_free(p_array);

    mem_free(input_file_name);
    mem_free(query_file_name);
    mem_free(output_file_name);

    return EXIT_SUCCESS;
}

int main(int argc, char** argv){
    srand(time(NULL));
    while(1){
        /**** Data ****/

        char* input_file_name = (char*) mem_allocate(sizeof(char) * BUFFER);
        char* query_file_name = (char*) mem_allocate(sizeof(char) * BUFFER);
        char* output_file_name = (char*) mem_allocate(sizeof(char) * BUFFER);

        int k = 50, E = 30, R = 1, N = 1, l = 20, m = 1;    // Default values in case k and L is not given as arguments from the user

        /**** Input parsing ****/

        parse_args(argc, argv, input_file_name, query_file_name, output_file_name, &k, &E, &R, &N, &l, &m);

        /**** GNNS or MRNG selection ****/

        if(m == 1){
            if(gnn(argc, input_file_name, query_file_name, output_file_name, k, E, R, N) == EXIT_FAILURE){
                break;
            }
        }else if(m == 2){
            if(mrng(argc, input_file_name, query_file_name, output_file_name, N, l) == EXIT_FAILURE){
                break;
            }
        }else{
            printf(" [ -m ] must be 1 or 2 \n");
            break;
        }

        char reset_question[BUFFER];
        printf(" Press [ Y or y ] to continue, press [ any key ] to stop\n");
        scanf("%s", reset_question);
        if((strcmp("Y", reset_question) == 0) || (strcmp("y", reset_question) == 0)){
            continue;
        }else{
            break;
        }
    }

    exit(EXIT_SUCCESS);
}