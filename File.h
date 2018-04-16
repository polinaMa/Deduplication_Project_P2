//
// Created by Polina on 11-Dec-17.
//

#ifndef DEDUPLICATION_PROJECT_FILE_H
#define DEDUPLICATION_PROJECT_FILE_H

#include "Utilities.h"
/* ****************************************************************************************************************** */
/* ****************************************************************************************************************** */
/* ******************* START ******************* File STRUCT Definition ******************* START ******************* */
/*
 * Definition of a File structure:
 *                  - file_sn     -> a running index on all files read from the file system
 *                  - file_id     -> a hushed id as appears in the input file
 *                  - file_depth  -> the depth of the file in the hierarchical tree
 *                  - dir_sn      -> Serial number of the directory containing this file
 *                  - num_blocks  -> number of blocks the file consists of
 *                  - file_size   -> the size of the file
 *                  - blocks_list -> List of Block_info elements of blocks contained in the file
 */
struct file_t{
    char flag; // L - logical_file , P - physical_file
    unsigned long file_sn;
    char* file_id;
    unsigned int file_depth;
    unsigned long dir_sn;
    int num_blocks;
    unsigned int file_size;
    List blocks_list;

    // Used Only For File-Level Deduplication
    List logical_files_list; // A list of serial numbers of logical files that belong to the physical file
    unsigned int num_files; // should be use only for flag = 'P'
    unsigned long physical_sn;

};
typedef struct file_t *File;

/* ******************** END ******************** File STRUCT Definition ******************** END ******************** */
/* ****************************************************************************************************************** */
/* ****************************************************************************************************************** */
/* ******************* START ******************* File STRUCT Functions ******************* START ******************** */
/*
 *  file_create - Creates a new file object with the input parameters
 *                      - file id - a hashed id as appears in the input file
 *                      - depth
 *                      -file sn - running index on all files in the filesystem
 *                      - dir sn
 *
 *
 * @file_id     - hashed id of the file
 * @depth       - the depth of the file in the file system (Root directory starts at 0)
 * @file_sn     - serial number of the file object
 * @size        - the size of the file
 * @physical_sn - in case of file level deduplication, there are 2 types of files - physical and logical
 */
File file_create(char* file_id , unsigned int depth , unsigned long file_sn , unsigned int size ,
                 unsigned long physical_sn , char dedup_type);

/*
 *  file_destroy - Destroys and frees space of a file structure
 *
 *  @file - Pointer to the file object to be destroyed
 */
void file_destroy(File file , char dedup_type);

/*
 *  file_get_SN - Returns the SN of the file
 *
 *  @file - Pointer to the file object
 */
unsigned long file_get_SN(File file);

/*
 * file_get_ID - Returns pointer to the hashed ID of the file
 *
 * @file - Pointer to the file object
 */
char* file_get_ID(File file);

/*
 *  file_get_depth - Returns the depth of the file in the hierarchy
 *
 *  @file - Pointer to the file object
 */
unsigned int file_get_depth(File file);
/*
 *  file_get_num_blocks - returns the number of blocks the file contains
 *
 *  @file - Pointer to the file object
 */
int file_get_num_blocks(File file);

/*
 *  file_get_num_blocks - returns the number of blocks the file contains
 *
 *  @file - Pointer to the file object
 */
ErrorCode file_set_parent_dir_sn(File file , unsigned long dir_sn);

/*
 *  file_set_physical_sn - Set the value of the physical serial number of the file object
 *
 *  @file             - Pointer to the file object
 *  @physical_file_sn - value of the serial number to be set
 */
ErrorCode file_set_physical_sn(File file , unsigned long physical_file_sn);

/*
 *  file_set_logical_flag - Set the File object to be a logical file
 *
 *  @file - Pointer to the file object
 */
ErrorCode file_set_logical_flag(File file);


/*
 *  file_add_block - Add block to the file object that contains it
 *
 *  @file       - Pointer to the file object
 *  @block_id   - hashed id of the block
 *  @block_size - size of the block
 */
ErrorCode file_add_block(File file , char* block_id , int block_size);

/* ******************** END ******************** File STRUCT Functions ******************** END ********************* */
#endif //DEDUPLICATION_PROJECT_FILE_H
