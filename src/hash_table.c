#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<math.h>

#include "hash_table.h"

// insert function to insert a key-value pair
void ht_insert(hash_table* ht, const char* key, const char* value);

// search function to search for a particular value using a given key
char* ht_search(hash_table* ht, const char* key);

//to delete a key-value pair from a given hash table using key
void ht_delete(hash_table* ht, const char* key);


//driver function
int main(){
    hash_table* ht = ht_new();
    ht_insert(ht,"code","forces");
    ht_insert(ht,"name","sankalp");
    ht_insert(ht,"dept","ECE");
    ht_insert(ht,"dept","ECE");
    
    char* s = ht_search(ht,"code");
    ht_insert(ht,"name","sank");
    
    char *c = ht_search(ht,"name");
    int cht = ht->count;
    
    printf("The value of key \'code\' is %s and size is %d \n",s,cht);
    printf("The value of key \'name\' is %s \n",c);

    ht_del_hash_table(ht);
    return 0;
}