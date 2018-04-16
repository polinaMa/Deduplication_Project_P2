//
// Created by Polina on 16-Mar-18.
//

#include "TextParsing.h"

/* ************************************************ Helper Functions ************************************************ */
void clear_line(char* line){
    if(strlen(line) >= 2){
        int len_buff = strcspn(line , "\r\n");
        line[len_buff] = '\n';
        line[len_buff + 1] = '\0';
    }
}

/* Compare between current buffer and string of "Z"*/
bool check_12_z(char buff[STR_OF_Z]){
    for (int i = 0; i < STR_OF_Z ; ++i) {
        if(buff[i] != 'z'){
            return false;
        }
    }
    return true;
}
/* ************************************************ Helper Functions ************************************************ */
/* ****************************************************************************************************************** */
/* *********************************************** Parsing Functions ************************************************ */
/* DIRECTORY NAME */
char* case_1_directory_name(char buff[BUFFER_SIZE]){
    //only first 10 digits depict the hashed directory name
    char* dir_name_hash = calloc(DIR_NAME_LEN,sizeof(char));
    if(!dir_name_hash){
        return NULL;
    }
    strncpy(dir_name_hash , buff , DIR_NAME_HASH);
    dir_name_hash[DIR_NAME_HASH] = '\0';
    return dir_name_hash;
}

/* NAMESPACE DEPTH */
unsigned short case_4_get_depth(char buff[BUFFER_SIZE]){
    unsigned short namespace_depth = (unsigned short)strtol(buff,(char**)NULL, 10);
    return namespace_depth ;
}

/* FILE SIZE */
unsigned int case_5_file_size(char buff[BUFFER_SIZE]){
    unsigned int file_size = (unsigned int)strtol(buff,(char **)NULL, 10);
    return file_size;
}

/* FILE ATTRIBUTES VALUE
 * Returns one from { 'D' , 'F'}
 *  @ 'D' - for directory
 *  @ 'F' - for file
 */
char case_6_file_attribute(char buff[BUFFER_SIZE]){
    unsigned int file_attribute = (unsigned int)strtol(buff,(char **)NULL, 16);
    char res;

    //Check for a directory, otherwise it is a file
    //The fifth bit should be set if this is a directory
    if((FILE_ATTRIBUTE_DIRECTORY & file_attribute) == FILE_ATTRIBUTE_DIRECTORY){
        res = 'D';
    } else{
        res = 'F';
    }
    return res;
}

/* FILE ID */
char* case_7_hash_file_id(char buff[BUFFER_SIZE], int ind_num_of_file, char* file_system_id){
    int text_len = strlen(buff) + 7;
    //FILE_ID_LEN
    char* file_id = calloc(text_len , sizeof(char)); // The value is 15 chars + 2 chars for index +1 for eol (end of line)
    if(file_id == NULL){
        return NULL;
    }
    //only first 15 digits depict the hashed directory name
    strcpy(file_id , file_system_id);
    strcat(file_id , buff);
    int id_len = strlen(file_id);
    file_id[id_len - 1] = '\0';
    return file_id;
}

/* *********************************************** Parsing Functions ************************************************ */

void print_ht_to_CSV(char dedup_type , char** files_to_read, int num_of_input_files ,
                     unsigned long blocks_sn, unsigned long files_sn, unsigned long dir_sn , unsigned long physical_files_sn,
                     HashTable ht_files , HashTable ht_blocks, HashTable ht_dirs, HashTable ht_physical_files){
    Entry pair = NULL;
    File temp_file = NULL;
    Block temp_block = NULL;
    Dir temp_dir = NULL;
    FILE *results_file = NULL;
    char* fileName = malloc(350*sizeof(char));
    fileName = strcpy(fileName , "Parsing_Results_");

    for(int i = 0 ; i < num_of_input_files ; i++){
        char file_proc[5];
        strncpy(file_proc , files_to_read[i] , 4);
        file_proc[4] = '\0';
        fileName = strcat(fileName, file_proc);
        if(i < (num_of_input_files -1)){
            fileName = strcat(fileName , "_");
        }
    }
    if( dedup_type == 'B'){
        fileName = strcat(fileName , "_B.csv");
    } else {
        fileName = strcat(fileName , "_F.csv");
    }

    // Open the output file
    results_file = fopen(fileName , "w+");
    if(results_file == NULL){
        printf("Results file can not be opened - please try again \n");
        free(fileName);
        return;
    }

    if(dedup_type == 'B'){
        fprintf(results_file ,"# Output type: block-level\n");
    } else {
        fprintf(results_file ,"# Output type: file-level\n");
    }

    fprintf(results_file ,"# Input files: ");
    for(int i =0 ; i < num_of_input_files ; i++){
        if(i == num_of_input_files - 1){
            fprintf(results_file ,"%s" , files_to_read[i]);
        } else{
            fprintf(results_file ,"%s," , files_to_read[i]);
        }

    }
    fprintf(results_file ,"\n");

    fprintf(results_file ,"# Num files: %lu\n" , (files_sn));
    fprintf(results_file ,"# Num directories: %lu\n" , (dir_sn));
    if(dedup_type == 'B'){
        fprintf(results_file ,"# Num Blocks: %lu\n", (blocks_sn));
    } else {
        fprintf(results_file ,"# Num physical files: %lu\n", (physical_files_sn));
    }

    if(dedup_type == 'B'){ //Block level deduplication
        //Print Files - Logical
        for(int i = 0 ; i < (ht_files->size_table) ;i++){
            pair = ht_files->table[i];
            while( pair != NULL && pair->key != NULL) {
                temp_file = ((File)(pair->data));
                fprintf(results_file , "F,%lu,%s,%lu,%d,",
                        temp_file->file_sn, temp_file->file_id , temp_file->dir_sn,
                        temp_file->num_blocks);
                //Object_Info temp_oi;
                LIST_FOREACH(Block_Info , iter ,temp_file->blocks_list){
                    unsigned long block_sn = ((Block)(ht_get(ht_blocks , iter->id)))->block_sn;
                    fprintf(results_file ,"%lu,%d," , block_sn , iter->size);
                }
                //temp_oi = listGetFirst(temp_file->blocks_list);
                fprintf(results_file ,"\n");
                pair = pair->next;
            }
        }
        //Print Blocks
        for(int i = 0 ; i < (ht_blocks->size_table) ;i++){
            pair = ht_blocks->table[i];
            while( pair != NULL && pair->key != NULL) {
                temp_block = ((Block)(pair->data));
                fprintf(results_file , "B,%lu,%s,%d,",
                        temp_block->block_sn , temp_block->block_id,
                        temp_block->shared_by_num_files);
                for(int j = 0 ; j < (temp_block->files_ht->size_table) ; j++){
                    EntryF pair_file_id = temp_block->files_ht->table[j];
                    while( pair_file_id != NULL && pair_file_id->key != NULL) {
                        unsigned long file_sn = ((File)(ht_get(ht_files , pair_file_id->key)))->file_sn;
                        fprintf(results_file ,"%lu," , file_sn);
                        pair_file_id = pair_file_id->next;
                    }
                }
                fprintf(results_file ,"\n");
                pair = pair->next;
            }
        }
    }else{//Print logical files
        for(int i = 0 ; i < (ht_files->size_table) ;i++){
            pair = ht_files->table[i];
            while( pair != NULL && pair->key != NULL) {
                temp_file = ((File)(pair->data));
                fprintf(results_file , "F,%lu,%s,%lu,%d,%lu,%d,\n",
                        temp_file->file_sn, temp_file->file_id , temp_file->dir_sn,
                        1, temp_file->physical_sn, temp_file->file_size);
                pair = pair->next;
            }
        }

        //Print physical files
        for(int i = 0 ; i < (ht_physical_files->size_table) ;i++){
            pair = ht_physical_files->table[i];
            while( pair != NULL && pair->key != NULL) {
                temp_file = ((File)(pair->data));
                fprintf(results_file , "P,%lu,%s,%d,",
                        temp_file->physical_sn, temp_file->file_id ,
                        temp_file->num_files);
                LIST_FOREACH(unsigned long* , iter1 ,temp_file->logical_files_list){
                    fprintf(results_file ,"%lu," , *iter1);
                }
                fprintf(results_file ,"\n");
                pair = pair->next;
            }
        }
    }

    //Print Directories
    for(int i = 0 ; i < (ht_dirs->size_table) ;i++){
        pair = ht_dirs->table[i];
        while( pair != NULL && pair->key != NULL) {
            temp_dir = ((Dir)(pair->data));
            printf("D - %lu - %s\n",temp_dir->dir_sn,temp_dir->dir_id);
            if(temp_dir->dir_depth == -1){
                fprintf(results_file , "R,");
            }else {
                fprintf(results_file , "D,");
            }
            fprintf(results_file , "%lu,%s,%lu,%d,%d," ,
                    temp_dir->dir_sn, temp_dir->dir_id, temp_dir->parent_dir_sn,
                    temp_dir->num_of_subdirs, temp_dir->num_of_files);
            LIST_FOREACH(unsigned long* , iter , temp_dir->dirs_list){
                fprintf(results_file ,"%lu," , *(iter));
            }
            LIST_FOREACH(unsigned long* , iter , temp_dir->files_list){
                fprintf(results_file ,"%lu," , *(iter));
            }
            fprintf(results_file , "\n");
            pair = pair->next;
        }
    }

    fclose(results_file);
    free(fileName);
}