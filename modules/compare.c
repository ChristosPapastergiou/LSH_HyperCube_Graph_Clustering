#include "../include/compare.h"

static double calculate(Macqueen macqueen, uint8_t** p_array, double** centroid_sil, int* indexes, int num_centroids, int data_size, int vector_size, int data_index){
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

void compare_dimension(FILE* file, List* exausted, List* approximate){
    char* original_input_file_name = (char*) mem_allocate(sizeof(char) * BUFFER);
    char* original_query_file_name = (char*) mem_allocate(sizeof(char) * BUFFER);

    uint32_t magic_number = 0, num_of_images = 0, num_of_rows = 0, num_of_cols = 0; 

    /**** Input file ****/

    printf(" [ Please provide a dataset file ] \n");
    scanf("%s", original_input_file_name);

    FILE* input_file = fopen(original_input_file_name, "rb");
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

    /**** Query file ****/

    printf(" [ Please provide a query file ] \n");
    scanf("%s", original_query_file_name);

    FILE* query_file = fopen(original_query_file_name, "rb");
    if(query_file == NULL){
        printf("Error opening query file\n");
    }

    fread(&magic_number, sizeof(uint32_t), 1, query_file);
    fread(&num_of_images, sizeof(uint32_t), 1, query_file);
    fread(&num_of_rows, sizeof(uint32_t), 1, query_file);
    fread(&num_of_cols, sizeof(uint32_t), 1, query_file);

    magic_number = ntohl(magic_number);
    num_of_images = ntohl(num_of_images);
    num_of_rows = ntohl(num_of_rows);
    num_of_cols = ntohl(num_of_cols);

    int query_size = QUERY_SIZE;
    // int query_size = num_of_images;
    
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
    List* exausted_true = exausted_search(q_array, p_array, vector_size, query_size, data_size, 1, exausted_start_time, exausted_end_time);

    /**** Calculation of the distance in the original dimension ****/

    double* true_compressed_distances = (double*) mem_allocate(sizeof(double) * query_size);
    for(int i = 0; i < query_size; i++){
        true_compressed_distances[i] = calculate_distance(q_array[i], p_array[get_image(exausted[i], 0)], vector_size, METRIC);
    }

    double* graph_compressed_distances = (double*) mem_allocate(sizeof(double) * query_size);
    for(int i = 0; i < query_size; i++){
        graph_compressed_distances[i] = calculate_distance(q_array[i], p_array[get_image(approximate[i], 0)], vector_size, METRIC);
    }

    /**** Output file ****/

    double total_true_distance = 0.0;
    double total_graph_distance = 0.0;
    double total_approximate_distance = 0.0;
    for(int i = 0; i < query_size; i++){
        total_true_distance += get_distance(exausted_true[i], 0);
        total_graph_distance += graph_compressed_distances[i];
        total_approximate_distance += true_compressed_distances[i];
    }
    total_true_distance /= query_size;
    total_graph_distance /= query_size;
    total_approximate_distance /= query_size;

    fprintf(file, "AAF (Exausted) : %f\n", total_approximate_distance / total_true_distance);
    fprintf(file, "AAF (Approximate): %f\n", total_graph_distance / total_true_distance);

    /**** Free memory ****/

    mem_free(graph_compressed_distances);
    mem_free(true_compressed_distances);

    mem_free(exausted_start_time);
    mem_free(exausted_end_time);

    for(int i = 0; i < query_size; i++){
        destroy_list(exausted_true[i]);
    }
    mem_free(exausted_true);

    for(int i = 0; i < query_size; i++){
        mem_free(q_array[i]);
    }
    mem_free(q_array);

    for(int i = 0; i < data_size; i++){
        mem_free(p_array[i]);
    }
    mem_free(p_array);

    mem_free(original_query_file_name);
    mem_free(original_input_file_name);
}

void compare_silhouette(FILE* file, Macqueen macqueen){
    char* original_input_file_name = (char*) mem_allocate(sizeof(char) * BUFFER);

    uint32_t magic_number = 0, num_of_images = 0, num_of_rows = 0, num_of_cols = 0; 

    /**** Input file ****/

    printf(" [ Please provide a dataset file ] \n");
    scanf("%s", original_input_file_name);

    FILE* input_file = fopen(original_input_file_name, "rb");
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

    /**** Calculate Silhouette ****/

    double total_silhouette_score = 0.0;

    double** centroid_sil = (double**) mem_allocate(sizeof(double*) * get_number_of_clusters(macqueen));
    int* indexes = (int*) mem_allocate(sizeof(int) * get_number_of_clusters(macqueen));
    for(int i = 0; i < get_number_of_clusters(macqueen); i++){
        int data = get_centroid_total_data(macqueen, i);
        centroid_sil[i] = (double*) mem_allocate(sizeof(double) * data);
        indexes[i] = 0;
    }

    for(int i = 0; i < data_size; i++){
        double s = calculate(macqueen, p_array, centroid_sil, indexes, get_number_of_clusters(macqueen), data_size, vector_size, i);
        total_silhouette_score += s;
    }

    /**** Output file ****/

    fprintf(file, "Silhouette : [ ");
    for(int i = 0; i < get_number_of_clusters(macqueen); i++){
        int data = get_centroid_total_data(macqueen, i);
        double total_cent_silou = 0;
        for(int j = 0; j < data; j++){
            total_cent_silou += centroid_sil[i][j];
        }
        total_cent_silou /= data; 
        fprintf(file, "%f, ", total_cent_silou);
    }
    fprintf(file, "%f ]\n", (total_silhouette_score / data_size));

    /**** Free memory ****/

    for(int i = 0; i < get_number_of_clusters(macqueen); i++){
        mem_free(centroid_sil[i]);
    }
    mem_free(centroid_sil);
    mem_free(indexes);

    for(int i = 0; i < data_size; i++){
        mem_free(p_array[i]);
    }
    mem_free(p_array);

    mem_free(original_input_file_name);
}