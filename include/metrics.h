#ifndef METRICS
#define METRICS

/**** Includes ****/

#include "common.h"

/**** Defines ****/

#define METRIC euclidean_distance                   // Euclidean Distance for two uint8_t
#define METRIC_DOUBLE euclidean_distance_double     // Euclidean Distance for one uint8_t and one double
#define METRIC_2DOUBLE euclidean_distance_2double   // Euclidean Distance for two doubles

/**** Typedefs ****/

typedef double (*DistanceMetric)(Pointer , Pointer, int);

/**** Functions ****/

// Calculate euclidean distance of two data points [p1] [p2] with [vector_size] 
double euclidean_distance(Pointer p1, Pointer p2, int vector_size);

// Calculate euclidean distance of two data points [p1] [p2] with [vector_size] 
double euclidean_distance_double(Pointer p1, Pointer p2, int vector_size);

// Calculate euclidean distance of two data points [p1] [p2] with [vector_size] 
double euclidean_distance_2double(Pointer p1, Pointer p2, int vector_size);

// Calculate distance of two data points [p1] [p2] with [vector_size] using [metric]
double calculate_distance(Pointer p1, Pointer p2, int vector_size, DistanceMetric metric);

#endif