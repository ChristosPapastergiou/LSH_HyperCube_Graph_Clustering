#include "../include/metrics.h"

double euclidean_distance(Pointer p1, Pointer p2, int vector_size){
    uint8_t* point1 = (uint8_t*) p1;
    uint8_t* point2 = (uint8_t*) p2;

    int dif;
    double sum = 0;
    for(int i = 0; i < vector_size; i++){
        dif = point1[i] - point2[i];
        sum += dif * dif;
    }
    return sqrt(sum);
}

double euclidean_distance_double(Pointer p1, Pointer p2, int vector_size){
    uint8_t* point1 = (uint8_t*) p1;
    double* point2 = (double*) p2;

    double dif;
    double sum = 0;
    for(int i = 0; i < vector_size; i++){
        dif = point1[i] - point2[i];
        sum += dif * dif;
    }
    return sqrt(sum);
}

double euclidean_distance_2double(Pointer p1, Pointer p2, int vector_size){
    double* point1 = (double*) p1;
    double* point2 = (double*) p2;

    double dif;
    double sum = 0;
    for(int i = 0; i < vector_size; i++){
        dif = point1[i] - point2[i];
        sum += dif * dif;
    }
    return sqrt(sum);
}

double calculate_distance(Pointer p1, Pointer p2, int vector_size, DistanceMetric metric) {
    return metric(p1, p2, vector_size);
}