//
// Created by Polina on 12-Mar-18.
//

#include "Directory.h"


/* ******************** START ******************** Helper Functions  ********************* START ******************** */
/*
 * copy_directory_info - returns pointer to a copy of the serial number of the directory received as input
 *
 * @directory_info - pointer to the serial number of the directory to be copied
 */
static ListElement copy_directory_info(ListElement directory_info){
    assert(directory_info);
    unsigned long* sn = (unsigned long*)(directory_info);
    unsigned long* sn_copy = malloc(sizeof(*sn_copy));
    if(sn_copy == NULL){
        return NULL;
    }
    *sn_copy = *sn;
    return sn_copy;
}

/*
 * free_dir_info - frees the allocated space to the serial number of a directory
 *
 * @directory_info - pointer to the serial number that should be freed
 */
static  void free_dir_info(ListElement directory_info){
    free(directory_info);
}

/* ********************* END ********************* Helper Functions  ********************** END ********************* */
/* ****************************************************************************************************************** */
/* ****************************************************************************************************************** */
/* ****************** START ****************** Directory STRUCT Functions ****************** START ****************** */
Dir dir_create(char* dir_id , unsigned int depth , unsigned long dir_sn){
    Dir dir = malloc(sizeof(*dir));
    if(dir == NULL){
        return NULL;
    }
    dir->dir_id = malloc((sizeof(char)*(strlen(dir_id) + 1)));
    if(!(dir->dir_id)){
        free(dir);
        return NULL;
    }
    dir->dir_id = strcpy(dir->dir_id , dir_id);
    dir->dir_depth = depth;
    dir->dir_sn = dir_sn;
    dir->num_of_files = 0;
    dir->num_of_subdirs = 0;
    dir->parent_dir_sn = 0; //  not known in the time of creation
    dir->dirs_list = listCreate(copy_directory_info , free_dir_info);
    dir->files_list = listCreate(copy_directory_info , free_dir_info);

    if((!dir->files_list) || (!dir->dirs_list)){
        free(dir->dir_id);
        free(dir);
        return NULL;
    }

    return dir;
}

ErrorCode dir_set_parent_dir_sn(Dir dir , unsigned long sn){
    assert(dir);
    dir->parent_dir_sn = sn;
    return SUCCESS;
}

void dir_destroy(Dir dir){
    assert(dir);
    free(dir->dir_id);
    printf("List Destroy - DIR - SUB_DIRS - %lu \n",dir->dir_sn);
    listDestroy(dir->dirs_list);
    printf("List Destroy - DIR - SUB_FILES - %lu \n",dir->dir_sn);
    listDestroy(dir->files_list);
    free(dir);
}
/*
unsigned long dir_get_SN(Dir dir){
    assert(dir);
    return dir->dir_sn;
}

char* dir_get_ID(Dir dir){
    assert(dir);
    return dir->dir_id;
}

unsigned int dir_get_depth(Dir dir){
    assert(dir);
    return dir->dir_depth;
}
*/
ErrorCode dir_add_file(Dir dir , unsigned long file_sn){
    if(dir == NULL || file_sn < 0){
        return INVALID_INPUT;
    }
    unsigned long* temp = malloc(sizeof(*temp));
    if(temp == NULL){
        return OUT_OF_MEMORY;
    }

    *temp = file_sn;
    ListResult res = listInsertFirst(dir->files_list , temp);
    if(res != LIST_SUCCESS){
        free(temp);
        return OUT_OF_MEMORY;

    }
    (dir->num_of_files)++;
    free(temp);
    return SUCCESS;
}

ErrorCode dir_add_sub_dir(Dir dir , unsigned long dir_sn){
    if(dir == NULL || dir_sn < 0){
        return INVALID_INPUT;
    }
    unsigned long* temp = malloc(sizeof(*temp));
    if(temp == NULL){
        return OUT_OF_MEMORY;
    }
    *temp = dir_sn;
    ListResult res = listInsertFirst(dir->dirs_list, temp);
    if(res != LIST_SUCCESS){
        free(temp);
        return OUT_OF_MEMORY;
    }
    (dir->num_of_subdirs)++;
    free(temp);
    return SUCCESS;
}

/* ******************* END ******************* Directory STRUCT Functions ******************* END ******************* */
