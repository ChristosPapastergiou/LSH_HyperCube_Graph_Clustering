#ifndef COMPARE
#define COMPARE

/**** Includes ****/

#include "list.h"
#include "common.h"
#include "search.h"
#include "metrics.h"
#include "macqueen.h"

/**** Defines ****/

#define BUFFER 256
#define QUERY_SIZE 5

/**** Functions ****/

// Comparing the distances based on the dimensions. Given 2 array of lists [exausted] and [approximate]
void compare_dimension(FILE* file, List* exausted, List* approximate);

// Comparing the silhouette of a given [macqueen] with the compressed image clusters 
void compare_silhouette(FILE* file, Macqueen macqueen);

#endif