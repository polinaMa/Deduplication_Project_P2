//
// Created by Polina on 12-Mar-18.
//

/* **************************************************** INCLUDES **************************************************** */
#include "HashTableF.h"
/* **************************************************** INCLUDES **************************************************** */
/* ******************** START ******************** HashTable Functions ******************** START ******************* */

HashTableF ht_createF(char type) {
    HashTableF ht = NULL;

    /* Allocate the table itself */
    ht = malloc(sizeof(*ht));
    if(!ht){ //check allocation was successful
        return NULL;
    }
    switch(type){
        case 'N': //for Hashtable in file object
            ht->size_table = BLOCKS_IN_FILE_SIZE;
            break;
        default:
            ht->size_table = INIT_SIZE; //Shouldn't really get here
            break;
    }
    /* Allocate pointers to the head nodes */
    ht -> table = malloc(sizeof(EntryF) * (ht->size_table));
    if(!ht -> table ){ //check array of pointers was allocated successfully
        free(ht);
        return NULL;
    }
    for(int i = 0; i < (ht->size_table) ; i++ ){
        ht->table[i] = NULL;
    }
    return ht;
}

long int ht_hashF( HashTableF ht, char *key ) {
    unsigned long int hashval = 0;
    int i = 0;

    /* Convert our string to an integer */
    while((hashval < ULONG_MAX) && (i < strlen(key))){
        hashval = hashval << 8;
        hashval += key[i];
        i++;
    }

    return hashval % (ht->size_table);
}

EntryF ht_newpairF(char *key){
    EntryF newpair  = malloc(sizeof(*newpair));
    if(newpair == NULL){
        return NULL;
    }

    newpair->key = malloc(sizeof(char)*(strlen(key)+1));
    if(newpair->key == NULL){
        free(newpair);
        return NULL;
    }
    newpair->key = strcpy(newpair->key , key);
    newpair->data = NULL;
    newpair->next = NULL;
    return newpair;
}

EntryF ht_setF(HashTableF ht, char *key) {
    EntryF newpair = NULL;
    EntryF next = NULL;
    EntryF last = NULL;

    long int hash_key = ht_hashF( ht , key );
    next = ht->table[hash_key];

    /* Advance until get the end of the list OR first matching key*/
    while( next != NULL && next->key != NULL && strcmp( key, next->key ) != 0 ) {
        last = next;
        next = next->next;
    }

    /* There's already a pair. Let's replace that string. */
    if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {
        //Return the pointer to the Block/File that already exists in the hash
        return next;
    } else { /* Nope, couldn't find it.  Time to grow a pair. */
        newpair = ht_newpairF(key); //allocate new pair
        if(newpair == NULL){
            return NULL;
        }
        /* We're at the start of the linked list in this hash_key. */
        if( next == ht->table[hash_key] ){ // If we in an empty list
            newpair->next = next;
            ht->table[hash_key] = newpair;

            /* We're at the end of the linked list in this hash_key. */
        } else if ( next == NULL ) {
            last->next = newpair;

        } else  { /* We're in the middle of the list. */
            newpair->next = next;
            last->next = newpair;
        }
        return newpair;
    }
}


DataF ht_getF(HashTableF ht, char *key ) {
    long int hash_key = ht_hashF(ht, key);
    EntryF pair = ht->table[hash_key];

    /* Step through the hash_key, looking for our value. */
    while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) != 0 ) {
        pair = pair->next;
    }

    /* Did we actually find anything? */
    if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
        //didn't find anything
        return NULL;

    }
    //found the key - return the data
    return pair->data;
}

void hashTableF_destroy(HashTableF ht){
    long size_table = ht->size_table;
    //long size_of_lists = 0;
    struct entryf_t* temp_to_free;
    // Remove lists elements of each HashTable cell
    for(int i = 0 ; i < size_table ; i++){ // free each list element of cell i
        while(ht->table[i]) {
            temp_to_free = ht->table[i];
            ht->table[i] = temp_to_free->next;
            // Destroy elements fields
            free(temp_to_free->key);
            free(temp_to_free);
        }
        assert(ht->table[i]==NULL);
    }
    free(ht->table);
    free(ht);
}

/* ********************* END ********************* HashTable Functions ********************* END ******************** */

