#include "prime.h"
#define HT_INITIAL_BASE_SIZE 53

typedef struct {
    char* key;
    char* value;
}ht_ele;


typedef struct {
    int capacity;
    int count;
    int base_capacity;
    ht_ele** elements;
}hash_table;

// to represent a deleted element
static ht_ele HT_DELETED_ELEM = {NULL,NULL};

// to provide a chunk of memory for hash table element
static ht_ele* ht_new_element(const char* k,const char* v){
    ht_ele* i = (ht_ele*)malloc(sizeof(ht_ele));
    i->key = strdup(k);
    i->value = strdup(v);
    return i;
} 

// to create a hash table with given base size
static hash_table* ht_new_sized(const int base_capacity){
    hash_table* ht = (hash_table*)malloc(sizeof(hash_table));
    ht->base_capacity = base_capacity;

    ht->capacity = next_prime(ht->base_capacity);
    ht->count=0;
    ht->elements = (ht_ele**)calloc((size_t)ht->capacity,sizeof(ht_ele*));
    return ht;
}

// to create a new hash table with initial base size
hash_table* ht_new(){
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

// to delete the chunk of memory provided to hash table element
static void ht_del_element(ht_ele* i){
    free(i->key);
    free(i->value);
    free(i);
}

// to delete the chunk of memory provided to hash table
void ht_del_hash_table(hash_table* ht){
    for(int i = 0 ;i < ht->capacity ; i++){
        ht_ele* element = ht->elements[i];
        if(element!=NULL){
            ht_del_element(element);
        }
    }
    free(ht->elements);
    free(ht);
}


// power function using binary exponentiation for fast calculation
long power(int a,int b){
    long res=1;
    while(b>0){
        if(b&1)
        res = res*a;
        
        a=a*a;
        b>>=1;
    }
    return res;
}

// hashing function
static int ht_hash(const char* s,const int p,const int m){
    long hash = 0;
    const int len = strlen(s);
    for(int i=0;i<len;i++)
    {
        hash+= power(p,len-(i+1))*s[i];
        hash = hash % m;
    }
    return (int)hash;
}

// using double hashing to prevent collisions
static int ht_get_hash(const char* s,const int num_buckets,const int attempt){
    const int hash_a = ht_hash(s,HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s,HT_PRIME_2, num_buckets);

    return (hash_a + (attempt * (hash_b+1))) % num_buckets;
}

void ht_insert(hash_table* ht, const char* key, const char* value);
char* ht_search(hash_table* ht, const char* key);
void ht_delete(hash_table* ht, const char* key);

// resize function 
static void ht_resize(hash_table* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }
    hash_table* new_ht = ht_new_sized(base_size);
    for (int i = 0; i < ht->capacity; i++) {
        ht_ele* ele = ht->elements[i];
        if (ele != NULL && ele != &HT_DELETED_ELEM) {
            ht_insert(new_ht, ele->key, ele->value);
        }
    }

    ht->base_capacity = new_ht->base_capacity;
    ht->count = new_ht->count;

    // To delete new_ht, we give it ht's capacity and items 
    const int tmp_capacity = ht->capacity;
    ht->capacity = new_ht->capacity;
    new_ht->capacity = tmp_capacity;

    ht_ele** tmp_elems = ht->elements;
    ht->elements = new_ht->elements;
    new_ht->elements = tmp_elems;

    ht_del_hash_table(new_ht);
}

// resize up for greater size
static void ht_resize_up(hash_table* ht) {
    const int new_size = ht->base_capacity * 2;
    ht_resize(ht, new_size);
}

// resize down for smaller size
static void ht_resize_down(hash_table* ht) {
    const int new_size = ht->base_capacity / 2;
    ht_resize(ht, new_size);
}

// insert function to insert a key-value pair in a given hash table with resize capabilities
void ht_insert(hash_table* ht, const char* key, const char* value){
    const int load = ht->count*100/ht->capacity;
    if(load>70){
        ht_resize_up(ht);
    }
    
    ht_ele* element = ht_new_element(key,value);
    int index = ht_get_hash(element->key,ht->capacity,0);
    ht_ele* cur_ele = ht->elements[index];
    int i=1;
    while(cur_ele!=NULL ){
        if(cur_ele != &HT_DELETED_ELEM){
            if(strcmp(cur_ele->key,key)==0){
                ht_del_element(cur_ele);
                ht->elements[index] = element;
                return;
            }
        }
        index = ht_get_hash(element->key,ht->capacity,i);
        cur_ele = ht->elements[index];
        i++;
    }
    ht->elements[index] = element;
    ht->count++;
}

// search function to search for a particular value using a given key
char* ht_search(hash_table* ht, const char* key){
    int index = ht_get_hash(key,ht->capacity,0);
    ht_ele* element = ht->elements[index];
    int i = 1;
    while (element != NULL) {
        
        if(element != &HT_DELETED_ELEM){
         if (strcmp(element->key, key) == 0) {
            return element->value;
         }
        }
        index = ht_get_hash(key, ht->capacity, i);
        element = ht->elements[index];
        i++;
    } 
    return NULL;
}

//to delete a key-value pair from a given hash table using key
void ht_delete(hash_table* ht, const char* key){
    const int load = ht->count*100/ht->capacity;
    if(load<10){
        ht_resize_down(ht);
    }
    int index = ht_get_hash(key, ht->capacity, 0);
    ht_ele* element = ht->elements[index];
    int i = 1;
    while (element != NULL) {
        if (element != &HT_DELETED_ELEM) {
            if (strcmp(element->key, key) == 0) {
                ht_del_element(element);
                ht->elements[index] = &HT_DELETED_ELEM;
            }
        }
        index = ht_get_hash(key, ht->capacity, i);
        element = ht->elements[index];
        i++;
    } 
    ht->count--;
}
