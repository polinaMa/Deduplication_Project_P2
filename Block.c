//
// Created by Polina on 12-Mar-18.
//

#include "Block.h"

/* ******************* START ******************* Block STRUCT Functions ******************* START ******************* */


Block block_create(char* block_id , unsigned long block_sn , unsigned int block_size){
    Block block = malloc(sizeof(*block)); //create a block
    if(block == NULL){ //Check memory allocation was successful
        return NULL;
    }

    block->block_id = malloc(sizeof(char)*(BLOCK_ID_LEN + 1)); //allocate string for block_id
    if(block->block_id == NULL){ //check successful allocation
        free(block);
        return NULL;
    }
    block->block_id = strcpy(block->block_id , block_id);
    block->block_sn = block_sn;
    block->shared_by_num_files = 0;
    block->block_size = block_size;

    block->files_ht = ht_createF('N');
    if(block->files_ht == NULL){
        free(block->block_id);
        free(block);
        return NULL;
    }

    return block;
}

void block_destroy(Block block){
    assert(block);
    free(block->block_id);
    hashTableF_destroy(block->files_ht);
    free(block);
}

long block_get_SN(Block block){
    assert(block);
    return block->block_sn;
}

char* block_get_ID(Block block){
    assert(block);
    return block->block_id;
}

ErrorCode block_add_file(Block block , char* file_id){
    if(file_id == NULL || block == NULL){ //Check input is valid
        return INVALID_INPUT;
    }

    EntryF result = ht_setF(block->files_ht, file_id);
    if(result == NULL){ //Check for memory allocation
        return OUT_OF_MEMORY;
    }

    (block->shared_by_num_files)++;
    return SUCCESS;
}

/* ******************** END ******************** Block STRUCT Functions ******************** END ******************** */


