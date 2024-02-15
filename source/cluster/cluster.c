#include "cluster.h"

// Parsing the given input
static void parse_args(int argc, char** argv, char* input_file_name, char* conf_file_name, char* output_file_name, char* method, bool* complete){
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-i") == 0){
            i++;
            if(i < argc){
                strcpy(input_file_name, argv[i]);
            }
        }else if(strcmp(argv[i], "-c") == 0){
            i++;
            if(i < argc){
                strcpy(conf_file_name, argv[i]);
            }        
        }else if(strcmp(argv[i], "-o") == 0){
            i++;
            if(i < argc){
                strcpy(output_file_name, argv[i]);
            }        
        }else if(strcmp(argv[i], "-m") == 0){
            i++;
            if(i < argc){
                strcpy(method, argv[i]);
            }        
        }else if(strcmp(argv[i], "-complete") == 0){
            (*complete) = true;
        }
    }
}

// Initialize values based on the configuration file
static void initialize_values(FILE* conf_file, int* number_of_clusters, int* number_of_vector_hash_tables, int* number_of_vector_hash_functions, int* max_number_M_hypercube, int* number_of_hypercube_dimensions, int* number_of_probes){
    char temp_line[BUFFER];
    while(fgets(temp_line, sizeof(temp_line), conf_file) != NULL){
        char* temp_variable_name = strtok(temp_line, STRTOK_DELIMITER);
        if(temp_variable_name == NULL){
            printf("Error to variable name\n");
            exit(EXIT_FAILURE);
        }

        char* temp_variable_value = strtok(NULL, STRTOK_DELIMITER);
        if(temp_variable_value == NULL){
            printf("Error to variable value\n");
            exit(EXIT_FAILURE);
        }

        if(strcmp(temp_variable_name, "number_of_clusters") == 0){
            (*number_of_clusters) = atoi(temp_variable_value);
        }else if(strcmp(temp_variable_name, "number_of_vector_hash_tables") == 0){
            (*number_of_vector_hash_tables) = atoi(temp_variable_value);
        }else if(strcmp(temp_variable_name, "number_of_vector_hash_functions") == 0){
            (*number_of_vector_hash_functions) = atoi(temp_variable_value);
        }else if(strcmp(temp_variable_name, "max_number_M_hypercube") == 0){
            (*max_number_M_hypercube) = atoi(temp_variable_value);
        }else if(strcmp(temp_variable_name, "number_of_hypercube_dimensions") == 0){
            (*number_of_hypercube_dimensions) = atoi(temp_variable_value);
        }else if(strcmp(temp_variable_name, "number_of_probes") == 0){
            (*number_of_probes) = atoi(temp_variable_value);
        }
    }
} 

// Creating an array of random values so hashtables and reverse search can use it
static Random_values* random_values_creation(uint8_t** p_array, int L, int k, int data_size, int vector_size){
    Random_values* random_values = (Random_values*) mem_allocate(sizeof(Random_values) * L);
    for(int i = 0; i < L; i++){
        random_values[i] = create_random_values(k, W, vector_size);
    }
    return random_values;
}

// Creating L-hashtables and inserting the data point inside it so reverse can use it
static Hashtable* hashtable_creation(Random_values* random_values, uint8_t** p_array, int L, int k, int data_size, int vector_size){
    Hashtable* hashtables = (Hashtable*) mem_allocate(sizeof(Hashtable) * L);
    for(int i = 0; i < L; i++){
        hashtables[i] = create_hashtable(data_size);
    }

    uint32_t* h_array = (uint32_t*) mem_allocate(sizeof(int) * k);
    for(int a = 0; a < L; a++){
        for(int i = 0; i < data_size; i++){
            random_h_values(h_array, p_array[i], get_v_values(random_values[a]), get_t_values(random_values[a]), k, W, vector_size); 
            uint32_t id = random_id_values(h_array, get_r_values(random_values[a]), k);
            insert_hashtable(hashtables[a], p_array[i], id, i);
        }
    }

    mem_free(h_array);

    return hashtables;
}

// Creating hypercube and inserting the data point inside it so reverse can use it
static Hypercube hypercube_creation(Random_values random_values, uint8_t** p_array, int k, int probes, int data_size, int vector_size){
    Hypercube hypercube = create_hypercube(k, probes);

    int* coordinates = (int*) mem_allocate(sizeof(int) * k);
    uint32_t* h_array = (uint32_t*) mem_allocate(sizeof(int) * k);
    for(int i = 0; i < data_size; i++){
        random_h_values(h_array, p_array[i], get_v_values(random_values), get_t_values(random_values), k, W, vector_size);
        generate_point_coordinates(h_array, coordinates, k);
        insert_hypercube(hypercube, p_array[i], coordinates, i);
    }

    mem_free(h_array);
    mem_free(coordinates);

    return hypercube;
}

// Function to calculate the Silhouette Score for a single data point
static double calculate_silhouette_score(FILE* output_file, Macqueen macqueen, uint8_t** p_array, double** centroid_sil, int* indexes, int num_centroids, int data_size, int vector_size, int data_index){
    int assigned_centroid = get_assignment_to_centroid(macqueen, data_index);
    int total_centroid_data = get_centroid_total_data(macqueen, assigned_centroid);

    int second_closest_centroid = get_second_closest_centroid(macqueen, data_index);
    int second_centroid_total_data = get_centroid_total_data(macqueen, second_closest_centroid);
    
    double a = 0.0;
    double b = 0.0;
    double s = 0.0;

    for(int i = 0; i < data_size; i++){
        if((get_assignment_to_centroid(macqueen, i) != assigned_centroid) || (data_index == i)){
            continue;
        }
        a += calculate_distance(p_array[data_index], p_array[i], vector_size, METRIC);  // Calculate a(i)
    }
    a /= (total_centroid_data - 1); // Average distance within the same cluster

    for(int i = 0; i < data_size; i++){
        if(get_assignment_to_centroid(macqueen, i) != second_closest_centroid){
            continue;
        }
        b += calculate_distance(p_array[data_index], p_array[i], vector_size, METRIC);  // Calculate b(i)
    }
    b /= (second_centroid_total_data);  // Average distance within the next closest cluster

    if(a < b){
        s = 1.0 - (a / b);
    }else if(a > b){
        s = (b / a) - 1.0;
    }

    centroid_sil[assigned_centroid][indexes[assigned_centroid]] = s;
    indexes[assigned_centroid]++;

    return s;
}

// Function to calculate the overall Silhouette Coefficient
static void overall_silhouette_coefficient(FILE* output_file, Macqueen macqueen, uint8_t** p_array, int num_centroids, int data_size, int vector_size){

    double total_silhouette_score = 0.0;

    double** centroid_sil = (double**) mem_allocate(sizeof(double*) * num_centroids);
    int* indexes = (int*) mem_allocate(sizeof(int) * num_centroids);
    for(int i = 0; i < num_centroids; i++){
        int data = get_centroid_total_data(macqueen, i);
        centroid_sil[i] = (double*) mem_allocate(sizeof(double) * data);
        indexes[i] = 0;
    }

    for(int i = 0; i < data_size; i++){
        double s = calculate_silhouette_score(output_file, macqueen, p_array, centroid_sil, indexes, num_centroids, data_size, vector_size, i);
        total_silhouette_score += s;
    }

    fprintf(output_file, "Silhouette : [ ");
    for(int i = 0; i < num_centroids; i++){
        int data = get_centroid_total_data(macqueen, i);
        double total_cent_silou = 0;
        for(int j = 0; j < data; j++){
            total_cent_silou += centroid_sil[i][j];
        }
        total_cent_silou /= data; 
        fprintf(output_file, "%f, ", total_cent_silou);
    }
    fprintf(output_file, "%f ]\n", (total_silhouette_score / data_size));    // Average Silhouette Score

    for(int i = 0; i < num_centroids; i++){
        mem_free(centroid_sil[i]);
    }
    mem_free(centroid_sil);
    mem_free(indexes);
}

int main(int argc, char** argv){
    srand(time(NULL));

    /**** Data ****/

    char* method = (char*) mem_allocate(sizeof(char) * BUFFER);
    char* conf_file_name = (char*) mem_allocate(sizeof(char) * BUFFER);
    char* input_file_name = (char*) mem_allocate(sizeof(char) * BUFFER);
    char* output_file_name = (char*) mem_allocate(sizeof(char) * BUFFER);
    
    bool complete = false;
    uint32_t magic_number = 0, num_of_images = 0, num_of_rows = 0, num_of_cols = 0; 
    int number_of_clusters = 0, number_of_vector_hash_tables = 0, number_of_vector_hash_functions = 0, max_number_M_hypercube = 0, number_of_hypercube_dimensions = 0, number_of_probes = 0;

    /**** Input parsing ****/

    parse_args(argc, argv, input_file_name, conf_file_name, output_file_name, method, &complete);

    if((strcmp(method, "Classic") != 0) && (strcmp(method, "LSH") != 0) && (strcmp(method, "Hypercube") != 0)){
        printf(" Valid methods : [ Classic ] [ LSH ] [ Hypercube ] \n");
    }

    /**** Input file and Data structure creation ****/
    
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

    if((*conf_file_name) == 0){
        printf(" [ Please provide a configuration file ] \n");
        scanf("%s", conf_file_name);
    }
    FILE* conf_file = fopen(conf_file_name, "rb");
    if(conf_file_name == NULL) {
        printf("Error opening configuration file\n");
    }
    initialize_values(conf_file, &number_of_clusters, &number_of_vector_hash_tables, &number_of_vector_hash_functions, &max_number_M_hypercube, &number_of_hypercube_dimensions, &number_of_probes);
    fclose(conf_file);

    printf("number_of_clusters              : %d\n", number_of_clusters);
    printf("number_of_vector_hash_tables    : %d\n", number_of_vector_hash_tables);
    printf("number_of_vector_hash_functions : %d\n", number_of_vector_hash_functions);
    printf("max_number_M_hypercube          : %d\n", max_number_M_hypercube);
    printf("number_of_hypercube_dimensions  : %d\n", number_of_hypercube_dimensions);
    printf("number_of_probes                : %d\n", number_of_probes);

    /**** Data ****/

    int data_size = num_of_images;
    int vector_size = num_of_rows * num_of_cols;

    /***** Data array ****/

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

    /**** Hashtable or Hypercube creation if needed ****/

    Hashtable* hashtables = NULL;
    Random_values* hash_random_values = NULL;
    if(strcmp(method, "LSH") == 0){
        hash_random_values = random_values_creation(p_array, number_of_vector_hash_tables, number_of_vector_hash_functions, data_size, vector_size);
        hashtables = hashtable_creation(hash_random_values, p_array, number_of_vector_hash_tables, number_of_vector_hash_functions, data_size, vector_size);
    }
    
    Hypercube hypercube = NULL;
    Random_values cube_random_values = NULL;
    if(strcmp(method, "Hypercube") == 0){
        cube_random_values = create_random_values(number_of_hypercube_dimensions, W, vector_size);
        hypercube = hypercube_creation(cube_random_values, p_array, number_of_hypercube_dimensions, number_of_probes, data_size, vector_size);
    }

    /**** Centroids array to be initialized ****/

    uint8_t** initialized_centroids = (uint8_t**) mem_allocate(sizeof(uint8_t*) * number_of_clusters);
    for(int i = 0; i < number_of_clusters; i++){
        initialized_centroids[i] = (uint8_t*) mem_allocate(sizeof(uint8_t) * vector_size);
    }

    /**** Initialization (K-means++) ****/
    
    struct timeval clustering_start_time;
    struct timeval clustering_end_time;

    gettimeofday(&clustering_start_time, NULL);

    k_means_plus_plus(p_array, initialized_centroids, number_of_clusters, data_size, vector_size);

    /**** Centroids array to use for assignment and update ****/

    double** centroids = (double**) mem_allocate(sizeof(double*) * number_of_clusters);
    for(int i = 0; i < number_of_clusters; i++){
        centroids[i] = (double*) mem_allocate(sizeof(double) * vector_size);
        for(int j = 0; j < vector_size; j++){
            centroids[i][j] = (double) initialized_centroids[i][j];     // Macqueen must take doubles so convert the initialized centroids from kmeans
        }
    }

    /**** Centroids selection (MacQueen) ****/

    Macqueen macqueen = lighting_macqueen(hashtables, hash_random_values, hypercube, cube_random_values, p_array, centroids, number_of_clusters, number_of_vector_hash_tables, number_of_vector_hash_functions, max_number_M_hypercube, number_of_hypercube_dimensions, number_of_probes, data_size, vector_size, method);

    gettimeofday(&clustering_end_time, NULL);

    /**** Comparison with the real dimension ****/

    char compare_question[BUFFER];
    printf(" Was this a compressed file and you want to see the performance based on the true dimension? [Y or y] \n");
    scanf("%s", compare_question);
    if((strcmp("Y", compare_question) == 0) || (strcmp("y", compare_question) == 0)){
        FILE* file = fopen("../../data/cluster_compare.csv", "w+");
        if(file == NULL){
            printf("Error opening file\n");
        }
        compare_silhouette(file, macqueen);
        fclose(file);
    }

    /**** Calculate times ****/

    double start_seconds = (double) clustering_start_time.tv_sec + (double) clustering_start_time.tv_usec / 1000000.0;
    double end_seconds = (double) clustering_end_time.tv_sec + (double) clustering_end_time.tv_usec / 1000000.0;
    double clustering_times = end_seconds - start_seconds;
    
    /**** File output ****/

    if((*output_file_name) == 0){
        printf(" [ Please provide an output file ] \n");
        scanf("%s", output_file_name);
    }
    FILE* output_file = fopen(output_file_name, "w+");
    if(output_file == NULL) {
        printf("Error opening output file\n");
    }

    for(int i = 0; i < number_of_clusters; i++){
        double* centroid_coordinates = get_centroid_coordinates(macqueen, i);
        fprintf(output_file, "Cluster-%d { size : %d, centroid : ", i + 1, get_centroid_total_data(macqueen, i));
        for(int j = 0; j < vector_size; j++){
            fprintf(output_file,"%f ", centroid_coordinates[j]);
        }
        fprintf(output_file, "}\n");
    }   
    fprintf(output_file, "clustering_time : %f\n", clustering_times);
    overall_silhouette_coefficient(output_file, macqueen, p_array, number_of_clusters, data_size, vector_size);

    if(complete){
        for(int i = 0; i < number_of_clusters; i++){
            double* centroid_coordinates = get_centroid_coordinates(macqueen, i);
            fprintf(output_file, "Cluster-%d {", i + 1);
            for(int j = 0; j < vector_size; j++){
                fprintf(output_file," %f", centroid_coordinates[j]);
            }
            for(int j = 0; j < data_size; j++){
                if(i == get_assignment_to_centroid(macqueen, j)){
                    fprintf(output_file, ", %d", j + 1);
                }
            }
            fprintf(output_file, " }\n");
        }
    }

    fclose(output_file);
        
    /**** Free memory ****/

    destroy_macqueen(macqueen);

    for(int i = 0; i < number_of_clusters; i++){
        mem_free(centroids[i]);
    }
    mem_free(centroids);

    for(int i = 0; i < number_of_clusters; i++){
        mem_free(initialized_centroids[i]);
    }
    mem_free(initialized_centroids);

    if(hashtables != NULL){
        for(int i = 0; i < number_of_vector_hash_tables; i++){
            destroy_hashtable(hashtables[i]);
        }
        for(int i = 0; i < number_of_vector_hash_tables; i++){
            delete_random_values(hash_random_values[i]);
        }
        mem_free(hash_random_values);
        mem_free(hashtables);
    }else if(hypercube != NULL){
        destroy_hypercube(hypercube);
        delete_random_values(cube_random_values);
    }

    for(int i = 0; i < data_size; i++){
        mem_free(p_array[i]);
    }
    mem_free(p_array);

    mem_free(method);
    mem_free(conf_file_name);
    mem_free(input_file_name);
    mem_free(output_file_name);

    exit(EXIT_SUCCESS);
}