#include "../include/random.h"

struct values{
    int* r_values;      // Array to store k-r random values
    float* t_values;    // Array to store k-t random values
    float** v_values;   // Array to store k-v random vectors
};

struct random_values{
    int size;           // k values
    Values values;      // All values
};

// Box-muller transform function to create two standard normal variables
static void box_muller(float* u1, float* u2){
    float r = sqrtf(-2.0f * logf(*u1));
    float theta = 2.0f * pi * (*u2);
    *u1 = r * cosf(theta);
    *u2 = r * sinf(theta);
}

// Generating r values
static void random_r_values(Random_values random_values, int k){
    for(int j = 0; j < k; j++){
        random_values->values->r_values[j] = (int) rand();
    }
}

// Generating t values
static void random_t_values(Random_values random_values, int k, int w){
    for(int i = 0; i < k; i++){
        random_values->values->t_values[i] = ((float) rand() / RAND_MAX) * w;
    }
}

// Generating v values
static void random_v_values(Random_values random_values, int k, int vector_size){
    for(int i = 0; i < k; i++){
        for(int j = 0; j < vector_size; j += 2){
            float u1 = (float) rand() / RAND_MAX;    // Random numbers  
            float u2 = (float) rand() / RAND_MAX;    // between 0 and 1

            box_muller(&u1, &u2);

            random_values->values->v_values[i][j] = u1;
            if(i + 1 < vector_size){
                random_values->values->v_values[i][j + 1] = u2;
            }

            random_values->values->v_values[i][j] *= random_values->values->v_values[i][j];         // Square the numbers so there are no negative
            random_values->values->v_values[i][j + 1] *= random_values->values->v_values[i][j + 1]; // values in the vector array
        }
    }
}

// Generating random values t, r, v
static void pick_random_values(Random_values random_values, int k, int w, int vector_size){
    random_r_values(random_values, k);
    random_t_values(random_values, k, w);
    random_v_values(random_values, k, vector_size);
}

// Initialization of the random values
static void initialize_random_values(Random_values random_values, int k){
    random_values->size = k;
}

// Creating and initializing the arrays of the r, t and v
static Values create_values(Random_values random_values, int vector_size){
    random_values->values = (Values) mem_allocate(sizeof(struct values));
    random_values->values->r_values = (int*) mem_allocate(sizeof(int) * random_values->size);
    random_values->values->t_values = (float*) mem_allocate(sizeof(float) * random_values->size);
    random_values->values->v_values = (float**) mem_allocate(sizeof(float*) * random_values->size);
    for(int j = 0; j < random_values->size; j++){
        random_values->values->v_values[j] = (float*) mem_allocate(sizeof(float) * vector_size);
    }
    return random_values->values;
}

Random_values create_random_values(int k, int w, int vector_size){
    Random_values random_values = (Random_values) mem_allocate(sizeof(struct random_values));
    initialize_random_values(random_values, k);
    random_values->values = create_values(random_values, vector_size);
    pick_random_values(random_values, k, w, vector_size);
    return random_values;
}

void delete_random_values(Random_values random_values){
    for(int i = 0; i < random_values->size; i++){
        mem_free(random_values->values->v_values[i]);
    }
    mem_free(random_values->values->r_values);
    mem_free(random_values->values->t_values);
    mem_free(random_values->values->v_values);
    mem_free(random_values->values);
    mem_free(random_values);
}

void generate_point_coordinates(uint32_t* h_array, int* coordinates, int k){
    for(int i = 0; i < k; i++){
        coordinates[i] = h_array[i] % 2;    // Coordinates are {0, 1} values
    }
}

void random_h_values(uint32_t* h_array, uint8_t* p, float** v, float* t, int k, int w, int vector_size){
    float dot_product = 0.0;
    for(int i = 0; i < k; i++){
        for(int j = 0; j < vector_size; j++){
            dot_product += v[i][j] * p[j];
        }
        h_array[i] = (int) floorf((dot_product + t[i]) / w);    // Storing all k-hash functions
    }
}

void random_h_values_double(uint32_t* h_array, double* p, float** v, float* t, int k, int w, int vector_size){
    float dot_product = 0.0;
    for(int i = 0; i < k; i++){
        for(int j = 0; j < vector_size; j++){
            dot_product += v[i][j] * p[j];
        }
        h_array[i] = (int) floorf((dot_product + t[i]) / w);    // Storing all k-hash functions
    }
}

uint32_t random_id_values(uint32_t* h, int* r, int k){
    uint32_t id = 0, partial_result = 0;
    for(int i = 0; i < k; i++){
        partial_result = ((r[i] % MAX_INT) * (h[i] % MAX_INT)) % MAX_INT;
        id = (id + partial_result) % MAX_INT;
    }
    return id;
}

int* get_r_values(Random_values random_values){
    return random_values->values->r_values;
}

float* get_t_values(Random_values random_values){
    return random_values->values->t_values;
}

float** get_v_values(Random_values random_values){
    return random_values->values->v_values;
}