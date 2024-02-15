#include "../include/graph.h"

struct node{
    Edge edges;             // Edge to another neighbour
    uint8_t* p;             // Data point
    int num_neighbours;     // Total neighbours of a data point
};

struct edge{
    Edge next;              // Pointer to the next edge
    int neighbour_index;    // Index of a data's neighbour
};

struct graph{
    int size;               // The total size of graph (dataset's size)
    Node* nodes;            // Aray of nodes (vertices) 
};

// Helping function to create an edge and initialize it
static Edge create_edge(int destination_index){
    Edge edge = (Edge) mem_allocate(sizeof(struct edge));
    edge->neighbour_index = destination_index;
    edge->next = NULL;
    return edge; 
}

// Initializing graph
static void initialize_graph(Graph graph, int size){
    graph->size = size;
}

// Creating and initializing the nodes of the graph
static Node* create_graph_nodes(Graph graph, uint8_t** p_array){
    graph->nodes = (Node*) mem_allocate(sizeof(Node) * graph->size);
    for(int i = 0; i < graph->size; i++){
        graph->nodes[i] = (Node) mem_allocate(sizeof(struct node));
        graph->nodes[i]->num_neighbours = 0;
        graph->nodes[i]->p = p_array[i];
        graph->nodes[i]->edges = NULL;
    }
    return graph->nodes;
}

Graph create_graph(uint8_t** p_array, int size){
    Graph graph = (Graph) mem_allocate(sizeof(struct graph));
    initialize_graph(graph, size);
    graph->nodes = create_graph_nodes(graph, p_array);
    return graph;
}

void destroy_graph(Graph graph){
    for(int i = 0; i < graph->size; i++){ // The graph is using adjacency lists
        Edge current = graph->nodes[i]->edges;      // so to free memory need to follow list's method
        while(current != NULL){
            Edge temp = current;
            current = current->next;
            mem_free(temp);
        }
        mem_free(graph->nodes[i]);
    }
    mem_free(graph->nodes);
    mem_free(graph);
}

void add_directed_edge(Graph graph, int source_index, int destination_index){
    Edge new_edge = create_edge(destination_index);

    Edge current = graph->nodes[source_index]->edges;
    if(current == NULL){
        graph->nodes[source_index]->edges = new_edge;
        graph->nodes[source_index]->num_neighbours++;
        return;
    }
    while(current->next != NULL){
        current = current->next;
    }

    current->next = new_edge; 
    graph->nodes[source_index]->num_neighbours++;
}

void get_edges(Graph graph, int* array, int vertex, int E){
    Edge current = graph->nodes[vertex]->edges;
    for(int i = 0; i < E; i++){
        array[i] = current->neighbour_index;
        current = current->next;
    }
}

void save_graph(Graph graph){
    FILE* graph_file = fopen("graph.csv", "w+");
    if(graph_file == NULL) {
        printf("Error opening output file\n"); 
    }

    for(int i = 0; i < graph->size; i++){
        Edge current = graph->nodes[i]->edges;
        while(current != NULL){
            fprintf(graph_file, "%d ", current->neighbour_index);
            current = current->next;
        }
        fprintf(graph_file, "\n");
    }
    fclose(graph_file);
}

void load_graph(Graph graph, FILE* graph_file){
    int i = 0;
    char temp_line[10000];
    while(fgets(temp_line, sizeof(temp_line), graph_file) != NULL){
        char* vertex = strtok(temp_line, " ");
        while(vertex != NULL) {
            add_directed_edge(graph, i, atoi(vertex));
            vertex = strtok(NULL, " ");
        }
        i++;
    }
    fclose(graph_file);
}

uint8_t* get_point(Graph graph, int index){
    return graph->nodes[index]->p;
}

int get_graph_size(Graph graph){
    return graph->size;
}

int get_neighbours(Graph graph, int index){
    return graph->nodes[index]->num_neighbours;
}