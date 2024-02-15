#include "../include/list.h"

struct node{
    Node next;          // Pointer to the next node
    uint8_t* p;         // The data of a point
    bool checked;       // Data checked or not
    int data_index;     // Data points index in the dataset
    double distance;    // The distance from qeury point
};

struct list{
    int N;      // N-nearest points
    int size;   // Size of the list
    Node head;  // Head of the node
};

// Checking if the image is already inside the list to prevent duplicate results
static bool check_same_image(List list, int data_number){
    Node current = list->head;
    while(current != NULL){
        if(current->data_index == data_number){
            return true;
        }
        current = current->next;
    }
    return false;
}

// Removing last node of the list
static void remove_last_node(List list){
    Node previous;
    Node current = list->head;
    if(list->size > list->N){
        while(current->next != NULL){
            previous = current;
            current = current->next;
        }
        previous->next = current->next;
        mem_free(current);
        list->size--;
    }
}

// Helping function merge sort uses to merge
static Node merge(Node left, Node right){
    if(left == NULL){
        return right;
    }else if(right == NULL){
        return left;
    }

    Node result = NULL;
    if(left->distance <= right->distance){
        result = left;
        result->next = merge(left->next, right);
    }else{
        result = right;
        result->next = merge(left, right->next);
    }
    return result;
}

// Helping function merge sort uses to split the list
static void split(Node head, Node* left, Node* right){
    Node slow = head;
    Node fast = head->next;

    while(fast != NULL){
        fast = fast->next;
        if(fast != NULL){
            slow = slow->next;
            fast = fast->next;
        }
    }

    (*left) = head;
    (*right) = slow->next;
    slow->next = NULL;
}

// Using merge sort algorithm
static void merge_sort(Node* node){
    Node left;
    Node right;
    Node head = (*node);

    if((head == NULL) || (head->next == NULL)){ // If the list is empty or has only one element, it is already sorted
        return;
    }

    split(head, &left, &right);

    merge_sort(&left);
    merge_sort(&right);

    (*node) = merge(left, right);
}

// Initializing the list
static void initialize_list(List list, int N){
    list->N = N;
    list->size = 0;
    list->head = NULL;
}

List create_list(int N){
    List list = (List) mem_allocate(sizeof(struct list));
    initialize_list(list, N);
    return list;
}

void destroy_list(List list){
    Node current = list->head;
    while(current != NULL){
        Node temp = current;
        current = current->next;
        mem_free(temp);
    }
    mem_free(list);
}

void insert_list(List list, uint8_t* p, double distance, int data_number){
    if((data_number != -1) && (check_same_image(list, data_number) == true)){ // Before inserting data check if it is already inside the list
        return;
    }

    Node new = (Node) mem_allocate(sizeof(struct node));

    new->p = p;                     // The new node of the list
    new->next = NULL;               // must contain all the information
    new->checked = false;
    new->distance = distance;       // about the data passed
    new->data_index = data_number; 

    Node current = list->head;
    if(current == NULL){            // First item in the list
        new->next = current;        // And just store in the list
        list->head = new;
        list->size++;
        return;
    }

    while(current->next != NULL){
        current = current->next;
    }
    new->next = current->next;      // Store the new node in the list
    current->next = new;
    list->size++;
}

void insert_sorted_list(List list, uint8_t* p, double distance, int data_number){
    if((data_number != -1) && (check_same_image(list, data_number) == true)){ // Before inserting data check if it is already inside the list
        return;
    }

    Node new = (Node) mem_allocate(sizeof(struct node));

    new->p = p;                     // The new node of the list
    new->next = NULL;               // must contain all the information
    new->checked = false;
    new->distance = distance;       // about the data passed
    new->data_index = data_number; 

    Node current = list->head;

    if((current == NULL) || (distance < current->distance)){    // The smallest distances must be placed at the 
        new->next = current;                                    // begging so the list must remain sorted
        list->head = new;
        list->size++;

        remove_last_node(list); // Check if the new node that was added made the list bigger than N and if yes remove the last node (furthest distance) 
        return;
    }
    while((current->next != NULL) && (current->next->distance <= distance)){
        current = current->next;
    }
    if((current->next == NULL) && (list->size >= list->N)){
        mem_free(new);  // The distance is the biggest of all the distances inside the list but also the 
        return;         // list doesnt have the space to place it to the end so it will be destroyed
    }
    new->next = current->next;
    current->next = new;
    list->size++;

    remove_last_node(list);
}

void insert_front_list(List list, uint8_t* p, double distance, int data_number){
    Node new = (Node) mem_allocate(sizeof(struct node));

    new->p = p;                     // The new node of the list
    new->next = NULL;               // must contain all the information
    new->checked = false;
    new->distance = distance;       // about the data passed
    new->data_index = data_number; 

    new->next = list->head;
    list->head = new;
    list->size++;
}

void list_sort(List list){
    merge_sort(&(list->head));
}

void remove_sorted_item(List list){
    Node first = list->head;
    list->head = first->next;
    list->size--;
    mem_free(first);
}

void print_list(FILE* output_file, List list){
    Node current = list->head;
    while(current != NULL){
        fprintf(output_file, "%d\n", current->data_index);
        current = current->next;
    }
}

void print_list_item(FILE* output_file, List list, int n){
    Node node = list->head;
    for(int i = 0; i < n; i++){
        node = node->next;
    }
    fprintf(output_file, "%f\n", node->distance);
}

void increase_list_capacity(List list){
    list->N += 1;
}

void update_checked(List list, int data_index){
    Node node = list->head;
    while((node->data_index != data_index) && (node->next != NULL)){
        node = node->next;
    }
    if(node->data_index == data_index){
        node->checked = true;
        return;
    }
}

bool check_if_checked(List list, int data_index){
    Node node = list->head;
    if(node == NULL){
        return false;
    }
    while((node->data_index != data_index) && (node->next != NULL)){
        node = node->next;
    }
    if((node->data_index == data_index) && (node->checked == true)){
        return true;
    }
    return false;
}

int get_list_size(List list){
    return list->size;
}

int get_image(List list, int n){
    Node node = list->head;
    for(int i = 0; i < n; i++){
        node = node->next;
    }
    return node->data_index;
}

double get_distance(List list, int n){
    Node node = list->head;
    for(int i = 0; i < n; i++){
        node = node->next;
    }
    return node->distance;
}

double get_last_distance(List list){
    Node node = list->head;
    while(node->next != NULL){
        node = node->next;
    }
    return node->distance;
}