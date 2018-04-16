//
// Created by Polina on 12-Mar-18.
//

#include "File.h"
/* ******************* START ******************* File STRUCT Functions ******************* START ******************** */
File file_create(char* file_id , unsigned int depth , unsigned long file_sn , unsigned int size ,
                 unsigned long physical_sn , char dedup_type){
    File file = malloc(sizeof(*file));
    if(file == NULL){
        return NULL;
    }

    file->file_id = malloc(sizeof(char)* (FILE_ID_LEN + 1));
    if(file->file_id == NULL){
        free(file);
        return NULL;
    }

    file->file_id = strcpy(file->file_id , file_id);
    file->file_sn = file_sn;
    file->dir_sn = 0; //not known in the time of creation
    file->num_blocks = 0;
    file->file_depth = depth;
    file->file_size = size;
    file->num_files = 1;
    file->flag = 'P';
    file->physical_sn = physical_sn; // will be updated from file_compare

    file->blocks_list = listCreate(copy_block_info , free_block_info);
    if(file->blocks_list == NULL){
        free(file->file_id);
        free(file);
        return NULL;
    }

    if(dedup_type == 'F'){
        file->logical_files_list = listCreate(copy_sn , free_sn);
        if(file->blocks_list == NULL){
            free(file->file_id);
            free(file);
            return NULL;
        }
        listInsertLast(file->logical_files_list , &(file->file_sn));
    }

    return file;
}

void file_destroy(File file , char dedup_type){
    assert(file);
    printf("List Destroy - FILE - %lu \n",file->file_sn);
    if((file->file_sn) == 9880){
        printf("Sarit HAdad <3 \n");
        listDestroy(file->blocks_list);
    } else {
        listDestroy(file->blocks_list);
    }
    //listDestroy(file->blocks_list);
    free(file->file_id);
    if(dedup_type == 'F'){
        listDestroy(file->logical_files_list);
    }
    free(file);
}

unsigned long file_get_SN(File file){
    assert(file);
    return file->file_sn;
}

char* file_get_ID(File file){
    assert(file);
    return file->file_id;
}

unsigned int file_get_depth(File file){
    assert(file);
    return file->file_depth;
}

int file_get_num_blocks(File file){
    assert(file);
    return file->num_blocks;
}

ErrorCode file_set_parent_dir_sn(File file , unsigned long dir_sn){
    assert(file);
    file->dir_sn = dir_sn;
    return SUCCESS;
}

ErrorCode file_set_physical_sn(File file , unsigned long physical_file_sn){
    assert(file);
    file->physical_sn = physical_file_sn;
    return SUCCESS;
}

ErrorCode file_set_logical_flag(File file){
    file->flag = 'L';
    return SUCCESS;
}

ErrorCode file_add_block(File file , char* block_id , int block_size){
    if(file == NULL || block_id == NULL || block_size < 0){
        return INVALID_INPUT;
    }

    Block_Info bi = malloc(sizeof(*bi));
    if(bi == NULL){
        return OUT_OF_MEMORY;
    }
    bi->id =  malloc(sizeof(char)*(strlen(block_id) +1));
    if(bi->id == NULL){
        free(bi);
        return OUT_OF_MEMORY;
    }
    strcpy(bi->id , block_id);
    bi->size = block_size;

    ListResult res = listInsertLast(file->blocks_list , bi);

    if(res != LIST_SUCCESS){
        free(bi->id);
        free(bi);
        return OUT_OF_MEMORY;
    }

    (file->num_blocks)++;
    free(bi->id);
    free(bi);
    return SUCCESS;
}

/* ******************** END ******************** File STRUCT Functions ******************** END ********************* */

