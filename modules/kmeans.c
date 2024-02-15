#include "../include/kmeans.h"

struct partial_p{
    double* p;          // P(r) array : Storing partial sums of each data point square distance to the closest centroid
    int* data_index;    // Data index
};

struct distance_to_centroid{
    double min_distance;        // Min distance of a data to it's nearest centroid
    int nearest_centroid_index; // The index of the nearest centroid
};

struct kmeans{
    int data_size;
    int vector_size;
    int num_centroids;
    Partial_p partial_p;                        
    Distance_to_centroid* distance_to_centroid; 
};

// A function to return r position (new centroid position in p_array) using binary search
static int get_r_position(double* p, double x, int p_size){
    int mid;
    int left = 0;
    int right = p_size - 1;
    
    while(left < right){
        mid = left + ((right - left) / 2);
        if(p[mid] < x){
            left = mid + 1;
        }else if(p[mid] >= x){
            right = mid;
        }
    }

    return left;
}

// Initialization of the struct distance_to_centroids
static void initialize_distance_to_centroids(Kmeans kmeans){
    for(int i = 0; i < kmeans->data_size; i++){
        kmeans->distance_to_centroid[i]->min_distance = INFINITY;
        kmeans->distance_to_centroid[i]->nearest_centroid_index = -1;
    }
}

// Initialization of Kmeans
static void initialize_kmeans(Kmeans kmeans, int data_size, int vector_size){
    kmeans->num_centroids = 0;
    kmeans->data_size = data_size;
    kmeans->vector_size = vector_size;
}

// Creating kmeans and allocating needed memory 
static Kmeans create_kmeans(int k, int data_size, int vector_size){
    Kmeans kmeans = (Kmeans) mem_allocate(sizeof(struct kmeans));
    kmeans->partial_p = (Partial_p) mem_allocate(sizeof(struct partial_p));
    kmeans->distance_to_centroid = (Distance_to_centroid*) mem_allocate(sizeof(Distance_to_centroid) * data_size);
    for(int i = 0; i < data_size; i++){
        kmeans->distance_to_centroid[i] = (Distance_to_centroid) mem_allocate(sizeof(struct distance_to_centroid));
    }
    initialize_kmeans(kmeans, data_size, vector_size);
    initialize_distance_to_centroids(kmeans);

    return kmeans;
}

static void destroy_kmeans(Kmeans kmeans){
    for(int i = 0; i < kmeans->data_size; i++){
        mem_free(kmeans->distance_to_centroid[i]);
    }
    mem_free(kmeans->distance_to_centroid);
    mem_free(kmeans->partial_p);
    mem_free(kmeans);
}

void k_means_plus_plus(uint8_t** p_array, uint8_t** initialized_centroids, int k, int data_size, int vector_size){
    Kmeans kmeans = create_kmeans(k, data_size, vector_size);  
    
    int* is_centroid = (int*) mem_allocate(sizeof(int) * data_size);    // Array too know if a data point is a centroid

    int first_centroid_index = rand() % data_size;      // Random image from the dataset as the first centroid
    is_centroid[first_centroid_index] = 1;

    memcpy(initialized_centroids[0], p_array[first_centroid_index], sizeof(uint8_t) * vector_size);    // Add to centroids array the random image from above
    kmeans->num_centroids++;     // Centroids increased
    
    for(int a = 1; a < k; a++){
        kmeans->partial_p->p = (double*) mem_allocate(sizeof(double) * (kmeans->data_size - kmeans->num_centroids));
        kmeans->partial_p->data_index = (int*) mem_allocate(sizeof(int) * (kmeans->data_size - kmeans->num_centroids));

        double max_minimum_distance = 0;
        for(int i = 0; i < data_size; i++){
            if(is_centroid[i] == 0){                                                                        // For every non centroid finding the 
                int nearest_centroid_index = 0;                                                             // closest centroid distance
                double min_distance = calculate_distance(p_array[i], initialized_centroids[0], vector_size, METRIC);
                for(int j = 1; j < kmeans->num_centroids; j++){
                    double distance = calculate_distance(p_array[i], initialized_centroids[j], vector_size, METRIC);
                    if(distance < min_distance){
                        min_distance = distance;
                        nearest_centroid_index = j;
                    }
                }
                kmeans->distance_to_centroid[i]->min_distance = min_distance;                       // Updating the values for the minimum distance
                kmeans->distance_to_centroid[i]->nearest_centroid_index = nearest_centroid_index;   // and the index of the closest centroid
                if(min_distance > max_minimum_distance){
                    max_minimum_distance = min_distance;
                }
            }
        }
        for(int i = 0; i < data_size; i++){
            if(is_centroid[i] == 0){
                kmeans->distance_to_centroid[i]->min_distance /= max_minimum_distance;  // For every distance of a data thats not a centroid dividing with
            }                                                                           // the maximum of the minimun distances to normalize it
        }

        int r_index = 0;
        for(int i = 0; i < data_size; i++){
            if(is_centroid[i] == 0){                                                                        // For every non centroid filling the 
                if(r_index == 0){                                                                           // P(r) array. P(r) = P(r-1) + D(r)^2
                    kmeans->partial_p->data_index[r_index] = i;
                    kmeans->partial_p->p[r_index] = pow(kmeans->distance_to_centroid[i]->min_distance, 2);
                }else{
                    kmeans->partial_p->data_index[r_index] = i;
                    kmeans->partial_p->p[r_index] = kmeans->partial_p->p[r_index - 1] + pow(kmeans->distance_to_centroid[i]->min_distance, 2);
                }
                r_index++;
            }
        }
    
        int p_size = data_size - kmeans->num_centroids;                  // Size of p isnt the full dataset set
        double x = ((double) rand() / RAND_MAX) * kmeans->partial_p->p[p_size - 1]; // Values must be in the range of p array values
        int r_pos = get_r_position(kmeans->partial_p->p, x, p_size);
        int data_index = kmeans->partial_p->data_index[r_pos];
        
        int new_centroid_index = kmeans->num_centroids;
        memcpy(initialized_centroids[new_centroid_index], p_array[data_index], sizeof(uint8_t) * vector_size);
        is_centroid[data_index] = 1;
        kmeans->num_centroids++;

        mem_free(kmeans->partial_p->p);
        mem_free(kmeans->partial_p->data_index);
    }
    
    mem_free(is_centroid);
    destroy_kmeans(kmeans);
}