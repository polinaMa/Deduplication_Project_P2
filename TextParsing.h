//
// Created by Polina on 16-Mar-18.
//

#ifndef DEDUPLICATIONPROJECT_TEXTPARSING_H
#define DEDUPLICATIONPROJECT_TEXTPARSING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashTable.h"
#include "Utilities.h"

/*
 * clear_line - receives a line in dos format (New line is represented by \r\n
 *              and converts it to Unix format (only \n for New Line)
 *
 * @line - Pointer to the line to be cleared.
 */
void clear_line(char* line);

bool check_12_z(char buff[STR_OF_Z]);

char* case_1_directory_name(char buff[BUFFER_SIZE]);

unsigned short case_4_get_depth(char buff[BUFFER_SIZE]);

unsigned int case_5_file_size(char buff[BUFFER_SIZE]);

char case_6_file_attribute(char buff[BUFFER_SIZE]);

char* case_7_hash_file_id(char buff[BUFFER_SIZE], int ind_num_of_file, char* file_system_id);


void print_ht_to_CSV(char dedup_type , char** files_to_read, int num_of_input_files ,
                     unsigned long blocks_sn, unsigned long files_sn, unsigned long dir_sn , unsigned long physical_files_sn,
                     HashTable ht_files , HashTable ht_blocks, HashTable ht_dirs, HashTable ht_physical_files);

#endif //DEDUPLICATIONPROJECT_TEXTPARSING_H
