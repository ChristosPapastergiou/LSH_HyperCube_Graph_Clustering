#include "../include/hashtable.h"

struct node{
    Node next;      // Pointer to the next node of the bucket
    uint8_t* p;     // Numbers that repressent an image
    uint32_t id;    // ID of an entry in the hashtable 
    int image_num;  // Image's number in the data set
};

struct hashtable{
    int buckets;        // Total size of the hashtable
    int* bucket_size;   // Total size of every bucket
    Node* hashbuckets;  // Hashbuckets of the hashtable and every hashbucket has nodes (linked list)
};

// Initialization of hashtable
static void initialize_hashtable(Hashtable hashtable, int buckets){
    hashtable->buckets = buckets;
    for(int i = 0; i < hashtable->buckets; i++){
        hashtable->bucket_size[i] = 0;
        hashtable->hashbuckets[i] = NULL;
    }
}

Hashtable create_hashtable(int size){
    int buckets = (int) size / 32;
    Hashtable hashtable = (Hashtable) mem_allocate(sizeof(struct hashtable));
    hashtable->hashbuckets = (Node*) mem_allocate(sizeof(Node) * buckets);
    hashtable->bucket_size = (int*) mem_allocate(sizeof(int) * buckets);
    initialize_hashtable(hashtable, buckets);
    
    return hashtable;
}

void destroy_hashtable(Hashtable hashtable){
    for(int i = 0; i < hashtable->buckets; i++){
        Node current = hashtable->hashbuckets[i];   // Every bucket is a "list" so 
        while(current != NULL){                     // need to deallocate every node
            Node temp = current;                    // of every bucket in the hashtable
            current = current->next;
            mem_free(temp);
        }
    }
    mem_free(hashtable->bucket_size);
    mem_free(hashtable->hashbuckets);
    mem_free(hashtable);
}

void insert_hashtable(Hashtable hashtable, uint8_t* p, uint32_t id, int data_number){
    Node new = (Node) mem_allocate(sizeof(struct node));

    new->p = p;                         // The new node of the bucket
    new->id = id;                       // must contain all the information
    new->next = NULL;                   // about the data passed
    new->image_num = data_number;

    int index = id % hashtable->buckets;

    new->next = hashtable->hashbuckets[index];  // Node after node in the bucket
    hashtable->hashbuckets[index] = new;
    hashtable->bucket_size[index]++;
}

int get_hashtable_size(Hashtable hashtable){
    return hashtable->buckets;
}

int get_bucket_size(Hashtable hashtable, int bucket){
    return hashtable->bucket_size[bucket];
}

void get_hashtable_data(Hashtable hashtable, uint8_t** p_array, int* data_numbers, int bucket){    
    int i = 0;
    for(Node node = hashtable->hashbuckets[bucket]; node != NULL; node = node->next){   
        p_array[i] = node->p;   
        data_numbers[i] = node->image_num;
        i++;
    }
}