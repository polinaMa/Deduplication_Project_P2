//
// Created by Polina on 21-Dec-17.
//

#ifndef DEDUPLICATIONPROJECT_DIRECTORY_H
#define DEDUPLICATIONPROJECT_DIRECTORY_H

#include "List.h"
#include "Utilities.h"

/* ****************** START ****************** Directory STRUCT Definition ****************** START ***************** */
/*
 * Definition of a Directory structure:
 *                  - dir_sn -> a running index on all directories read from the file system
 *                  - dir_id -> a hushed id as appears in the input file
 *                  - parent_dir_sn -> the sn of the parent directory in the hierarchy
 *                  - dir_depth -> the depth of the directory in the hierarchical tree
 *                  - num_of_subdirs -> number of sub directories
 *                  - num_of_files -> number of files contained in the directory
 *                  - dirs_list -> list of directory sn contained in this directory
 *                  - files_list -> list of file sn contained in this directory
 *
 */
struct dir_t{
    unsigned long dir_sn;
    char* dir_id;
    unsigned long parent_dir_sn;
    int dir_depth;
    unsigned short num_of_subdirs;
    unsigned short num_of_files;
    List dirs_list; // list of serial numbers
    List files_list; //list of serial numbers
};
typedef struct dir_t *Dir;

/* ******************* END ****************** Directory STRUCT Definition ****************** END ******************** */
/* ****************************************************************************************************************** */
/* ****************************************************************************************************************** */
/* ****************** START ****************** Directory STRUCT Functions ****************** START ****************** */

/*
 * dir_create - Creates a new Directory object with the parameters received as input
 *              Returns pointer to the new directory object created
 *
 *
 * @dir_id - hashed id of the directory
 * @depth  - the depth of the directory in the file system (Root is in depth 0)
 * @dir_sn - serial number of the directory
 */
Dir dir_create(char* dir_id , unsigned int depth , unsigned long dir_sn);

/*
 * dir_set_parent_dir_sn - updates the parent serial number of the directory received as input
 *
 * @dir - the directory who's parent directory serial number we want to update
 * @sn  - the serial number of the parent directory
 */
ErrorCode dir_set_parent_dir_sn(Dir dir , unsigned long sn);

/*
 * dir_destroy - Destroys struct of Directory
 *
 * @dir - Pointer to the directory structure that should be destroyed
 */
void dir_destroy(Dir dir);

/*
 * dir_get_SN - Return the serial number of directory
 *
 * @dir - pointer to the directory
 */
unsigned long dir_get_SN(Dir dir);

/*
 * dir_get_ID - Return pointer to the ID of directory
 *
 * @dir - pointer to the directory
 */
char* dir_get_ID(Dir dir);

/*
 * dir_get_depth - Return the depth of the directory
 *
 * @dir - pointer to the directory
 */
unsigned int dir_get_depth(Dir dir);

/*
 * dir_add_file - Adds file to a directory object by saving its' ID in the files list of the directory
 *                and updates the files counter of the folder
 *
 *  @dir     - pointer to the directory
 *  @file_sn - the serial number of the file that should be added to the directory
 */
ErrorCode dir_add_file(Dir dir , unsigned long file_sn);

/* Adding sub_dir into the directory */
/*
 * dir_add_sub_dir - Adds a sub-directory to a directory object by saving its' ID in the sub-directories list of
 *                   the directory and updates the files counter of the folder
 *
 *  @dir    - pointer to the directory
 *  @dir_sn - the serial number of the sub-directory that should be added to the directory
 */
ErrorCode dir_add_sub_dir(Dir dir , unsigned long dir_sn);

/* ******************* END ******************* Directory STRUCT Functions ******************* END ******************* */

#endif //DEDUPLICATIONPROJECT_DIRECTORY_H
