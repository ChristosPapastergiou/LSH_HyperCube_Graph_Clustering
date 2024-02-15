#include "cube.h"

// Parsing the given input
static void parse_args(int argc, char** argv, char* input_file_name, char* query_file_name, char* output_file_name, int* k, int* M, int* probes, int* N, double* R){
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
        }else if(strcmp(argv[i], "-M") == 0){
            i++;
            if(i < argc){
                (*M) = atoi(argv[i]);
            }        
        }else if(strcmp(argv[i], "-probes") == 0){
            i++;
            if(i < argc){
                (*probes) = atoi(argv[i]);
            }  
        }else if(strcmp(argv[i], "-o") == 0){
            i++;
            if(i < argc){
                strcpy(output_file_name, argv[i]);
            }        
        }else if(strcmp(argv[i], "-N") == 0){
            i++;
            if(i < argc){
                (*N) = atoi(argv[i]);
            }        
        }else if(strcmp(argv[i], "-R") == 0){
            i++;
            if(i < argc){
                (*R) = atof(argv[i]);
            }        
        }      
    }
}

int main(int argc, char** argv){
    srand(time(NULL));
    while(1){
        /**** Data ****/

        char* input_file_name = (char*) mem_allocate(sizeof(char) * BUFFER);
        char* query_file_name = (char*) mem_allocate(sizeof(char) * BUFFER);
        char* output_file_name = (char*) mem_allocate(sizeof(char) * BUFFER);

        double R = 10000;                               // Default values in case k and L
        int k = 14, M = 10, probes = 2, N = 1, w = W;   // is not given as arguments from the user
        
        uint32_t magic_number = 0, num_of_images = 0, num_of_rows = 0, num_of_cols = 0; 

        /**** Input parsing ****/

        parse_args(argc, argv, input_file_name, query_file_name, output_file_name, &k, &M, &probes, &N, &R);

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

        /**** Hypercube ****/

        Hypercube hypercube = create_hypercube(k, probes);

        /**** Values ****/

        Random_values random_values = create_random_values(k, w, vector_size);

        int** coordinates = (int**) mem_allocate(sizeof(int*) * data_size);
        for(int i = 0; i < data_size; i++){
            coordinates[i] = (int*) mem_allocate(sizeof(int) * k);
        }
        uint32_t* h_array = (uint32_t*) mem_allocate(sizeof(int) * k);
        for(int i = 0; i < data_size; i++){
            random_h_values(h_array, p_array[i], get_v_values(random_values), get_t_values(random_values), k, w, vector_size);
            generate_point_coordinates(h_array, coordinates[i], k);
            insert_hypercube(hypercube, p_array[i], coordinates[i], i);
        }

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

        for(int i = 0; i < query_size; i++){
            random_h_values(h_array, q_array[i], get_v_values(random_values), get_t_values(random_values), k, w, vector_size);
            generate_point_coordinates(h_array, coordinates[i], k);
        }

        /**** Nearest neightbour ****/

        struct timeval* exausted_start_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * query_size);
        struct timeval* exausted_end_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * query_size);
        List* exausted = exausted_search(q_array, p_array, vector_size, query_size, data_size, N, exausted_start_time, exausted_end_time);

        /**** Approximate k Nearest neightbours ****/

        struct timeval* kNN_start_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * query_size);
        struct timeval* kNN_end_time = (struct timeval*) mem_allocate(sizeof(struct timeval) * query_size);
        List* kNN = cube_approximate_kNN(hypercube, q_array, coordinates, M, N, vector_size, query_size, kNN_start_time, kNN_end_time);

        /**** Approximate Range Search ****/

        List* range = cube_approximate_range_search(hypercube, q_array, coordinates, M, N, vector_size, query_size, R);

        /**** Comparison with the real dimension ****/

        char compare_question[BUFFER];
        printf(" Was this a compressed file and you want to see the performance based on the true dimension? [Y or y] \n");
        scanf("%s", compare_question);
        if((strcmp("Y", compare_question) == 0) || (strcmp("y", compare_question) == 0)){
            FILE* file = fopen("../../data/cube_compare.csv", "w+");
            if(file == NULL){
                printf("Error opening file\n");
            }
            compare_dimension(file, exausted, kNN);
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

        double total_kNN = 0.0;
        double* kNN_times = (double*) mem_allocate(sizeof(double) * query_size);
        for(int i = 0; i < query_size; i++){
            double start_seconds = (double) kNN_start_time[i].tv_sec + (double) kNN_start_time[i].tv_usec / 1000000.0;
            double end_seconds = (double) kNN_end_time[i].tv_sec + (double) kNN_end_time[i].tv_usec / 1000000.0;
            kNN_times[i] = end_seconds - start_seconds;
            total_kNN += kNN_times[i];
        }

        /**** File output & result writing ****/

        double max_af = 0.0;
        double total_true_distance = 0.0;
        double total_approximate_distance = 0.0;
        for(int i = 0; i < query_size; i++){
            fprintf(output_file, "Query : %d\n", i + 1);
            for(int j = 0; j < N; j++){
                fprintf(output_file, "Nearest neighbor - %d : %d\n ", j+1, get_image(kNN[i], j));
                fprintf(output_file, "distanceCube : ");
                print_list_item(output_file, kNN[i], j);
                fprintf(output_file, " distanceTrue : ");
                print_list_item(output_file, exausted[i], j);
            }
            fprintf(output_file, "tCube : %f\n", kNN_times[i]);
            fprintf(output_file, "tTrue : %f\n", exausted_times[i]);
            fprintf(output_file, "AF    : %f\n", get_distance(kNN[i], 0) / get_distance(exausted[i], 0));
            if(get_distance(kNN[i], 0) / get_distance(exausted[i], 0) > max_af){
                max_af = get_distance(kNN[i], 0) / get_distance(exausted[i], 0);
            }
            total_approximate_distance += get_distance(kNN[i], 0);
            total_true_distance += get_distance(exausted[i], 0);
            fprintf(output_file, "R-near neighbours : \n");
            print_list(output_file, range[i]);
            fprintf(output_file, "\n");
        }
        total_approximate_distance /= query_size;
        total_true_distance /= query_size;
        fprintf(output_file, "tAverageCube : %f\n", total_kNN / query_size);
        fprintf(output_file, "tAverageTrue : %f\n", total_exausted / query_size);
        fprintf(output_file, "MAF : %f\n", max_af);
        fprintf(output_file, "AAF : %f\n", total_approximate_distance / total_true_distance);
        fclose(output_file);

        /**** Free memory ****/

        mem_free(exausted_start_time);
        mem_free(exausted_end_time);
        mem_free(kNN_start_time);
        mem_free(kNN_end_time);

        mem_free(exausted_times);
        mem_free(kNN_times);

        for(int i = 0; i < query_size; i++){
            destroy_list(kNN[i]);
            destroy_list(range[i]);
            destroy_list(exausted[i]);
        }
        mem_free(kNN);
        mem_free(range);
        mem_free(exausted);

        mem_free(h_array);

        for(int i = 0; i < data_size; i++){
            mem_free(coordinates[i]);
        }
        mem_free(coordinates);

        delete_random_values(random_values);
        destroy_hypercube(hypercube);

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