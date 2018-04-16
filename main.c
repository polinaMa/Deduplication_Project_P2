/* *************************************************** INCLUDES ***************************************************** */
#include "Utilities.h"
#include "HashTable.h"
#include "TextParsing.h"
/* ************************************************ Global Params *************************************************** */
/* Serial number for counting the elements which insert to the system */
// files_sn is the logical sn-number
unsigned long blocks_sn = 0 , files_sn = 0 , dir_sn = 0, physical_files_sn = 0;

/* Hash-Tables for blocks, files , directories */
HashTable ht_files = NULL , ht_blocks = NULL , ht_dirs = NULL ,ht_physical_files = NULL;

/* Root Directory */
//Dir roots[NUM_INPUT_FILES];
Dir* roots = NULL;
char dedup_type;

/* *********************************************** Parsing Functions ************************************************ */

/* Line 13 is SV */
//TODO - Export to TextParsing.h
// returns true if a file was created
void case_13_VS(FILE *input_file , char buff[BUFFER_SIZE] , int* block_line_count ,
                bool* read_empty_line_chucnks ,unsigned short depth, char* object_id,
                unsigned int file_size , bool* file_was_created, bool* finished_process_blocks) {
    bool read_block = false;   /* Params initialization */
    bool object_exists = false;
    *read_empty_line_chucnks = false;
    char block_id[BLOCK_ID_LEN];
    unsigned int block_size = 0;
    Block new_block = NULL;

    while((read_block == false) && (*read_empty_line_chucnks == false)){
        switch ((int)(buff[0])) { //check next line
            case 'S':
                break;
            case 'V':
                break;
            case 'A':
                break;
                /* Empty line */
            case LINE_SPACE :
                *read_empty_line_chucnks = true;
                break;
                /* Data chunk */
            default:
                read_block = true;
                break;
        }
        if((*read_empty_line_chucnks != true)&&(read_block != true)){
            fgets(buff, BUFFER_SIZE, input_file);
            clear_line(buff);
            (*block_line_count)++;
        }
    }
    if (*read_empty_line_chucnks == true) {
        return;
    }
    // If we got here it means we have blocks to read - Add file to files hashtable
    File file_obj = NULL , file_obj_p = NULL;
    if(dedup_type == 'B'){ //Block level deduplication
        file_obj = ht_set(ht_files , object_id , depth ,files_sn , file_size ,'F',
                          &object_exists , physical_files_sn,dedup_type);
    } else { // File level deduplication
        file_obj = file_create(object_id , depth , files_sn , file_size , physical_files_sn , dedup_type);
        file_obj_p = file_create(object_id , depth , files_sn , file_size , physical_files_sn, dedup_type);
    }
    files_sn++; // logical_files_sn
    physical_files_sn++;

    *file_was_created = true;
    object_exists = false;

    /* Read all data chunks  - Add Block Objects to hashtable*/
    if ((int)(buff[0]) != LINE_SPACE) {
        do { //we already have one chunk in the buffer
            char size[CHUNKE_SIZE_LEN] = "aaaaa";
            block_size = 0;
            if (check_12_z(buff) == true) { //only first 12 digits are block_id
                strncpy(block_id, buff, STR_OF_Z);
                block_id[STR_OF_Z] = '\0';
                strncpy(size, &buff[(STR_OF_Z + 1)], CHUNKE_SIZE_LEN-1);
            } else {
                //only first 10 digits are block_id
                strncpy(block_id, buff, 10);
                block_id[CHUNKE_ID_LEN] = '\0';
                strncpy(size, &buff[(CHUNKE_ID_LEN + 1)], CHUNKE_SIZE_LEN-1);
            }

            block_size = (int)strtol(size,NULL, 10);

            file_add_block(file_obj , block_id , block_size);
            if(dedup_type == 'F'){
                file_add_block(file_obj_p , block_id , block_size);
            }

            new_block = ht_set(ht_blocks , block_id , 1 , blocks_sn , block_size , 'B',
                               &object_exists , 0 , dedup_type);
            block_add_file(new_block , file_obj->file_id );
            if(object_exists == false){
                blocks_sn++;
            }
            fgets(buff, BUFFER_SIZE, input_file);
            clear_line(buff);
            (*block_line_count)++;
            object_exists = false;
        } while (strlen(buff) > 1);
    }
    *finished_process_blocks = true;

    if(dedup_type == 'F'){ // Check if physical file already exists
        file_compare(ht_files ,ht_physical_files ,  file_obj , file_obj_p , &physical_files_sn , dedup_type);
    }
    return;
}

/* update_parent_dir_sn */
//TODO - Export to TextParsing.h
void update_parent_dir_sn(List previous , List current , int global_depth , int input_file_index){
    File temp_file = NULL;
    Dir temp_dir = NULL;
    Object_Info prev_list_iterator = NULL;
    Object_Info curr_list_iterator = NULL;
    int curr_level_objects_count = 0 , prev_list_size = 0 , curr_list_size = 0;

    if(global_depth == 0){ //We are at root Level directory just set everyone to be the children of root
        unsigned long root_sn = roots[input_file_index]->dir_sn;
        //Set root to be its own child
        dir_set_parent_dir_sn(roots[input_file_index] , root_sn);
        dir_add_sub_dir(roots[input_file_index] , root_sn);

        LIST_FOREACH(Object_Info , iter ,current){
            if(iter->object_type == 'F'){
                temp_file = (File)(ht_get(ht_files , iter->object_id));
                assert(temp_file);
                file_set_parent_dir_sn(temp_file ,root_sn);
                dir_add_file(roots[input_file_index],temp_file->file_sn);

            } else{
                temp_dir = (Dir)(ht_get(ht_dirs , iter->object_id));
                assert(temp_dir);
                dir_set_parent_dir_sn(temp_dir , root_sn);
                dir_add_sub_dir(roots[input_file_index],temp_dir->dir_sn);
            }
        }

    }else{ //Go over both lists and update accordingly
        prev_list_iterator = listGetFirst(previous);
        curr_list_iterator = listGetFirst(current);
        curr_level_objects_count = 0;
        prev_list_size = listGetSize(previous);
        curr_list_size = listGetSize(current);

        for(int i = 0 ; i < prev_list_size ; i++){ //iterate over Previous level list
            unsigned long current_sn_to_set = prev_list_iterator->object_sn;
            Dir parent_dir_object = (Dir)(ht_get(ht_dirs , prev_list_iterator->object_id));

            if(prev_list_iterator->object_type == 'F'){ //A File cant be Parent directory for anyone
                prev_list_iterator = listGetNext(previous); //advance to the next object in the previous level
                continue;
            }else{ //The object is a directory it can be a parent
                if(curr_level_objects_count >= curr_list_size){
                    prev_list_iterator = listGetNext(previous); //advance to the next object in the previous level
                    continue;
                }
                char* parent_id = curr_list_iterator->parent_dir_id;
                //now lets iterate over the current list while we have objects with the same parent id
                while((curr_list_iterator != NULL)&&(strcmp(parent_id , curr_list_iterator->parent_dir_id) == 0)){
                    if(curr_list_iterator->object_type == 'F'){
                        temp_file = (File)(ht_get(ht_files , curr_list_iterator->object_id));
                        assert(temp_file);
                        file_set_parent_dir_sn(temp_file ,current_sn_to_set);
                        //add to the prevDir object - dir_add_file
                        dir_add_file(parent_dir_object ,curr_list_iterator->object_sn);
                    } else{
                        temp_dir = (Dir)(ht_get(ht_dirs , curr_list_iterator->object_id));
                        assert(temp_dir);
                        dir_set_parent_dir_sn(temp_dir , current_sn_to_set);
                        //add to the prevDir object - dir_add_sub_dir
                        dir_add_sub_dir(parent_dir_object , curr_list_iterator->object_sn);
                    }
                    curr_list_iterator = listGetNext(current);//advance to the next object in the current level
                    curr_level_objects_count++;
                }
            }
            prev_list_iterator = listGetNext(previous); //advance to the next object in the previous level
        }
    }
}


/* ****************************************************** MAIN ******************************************************** */
int main(int argc , char** argv){
    /* ----------------------- Parameters Declarations & Initialization ----------------------- */
    /* Define Files to be read */
    int num_input_files = 0;
    char* current_working_directory = NULL;
    char** files_to_read = NULL;
    if(argc==1){
        printf("No Extra Command Line Argument Passed Other Than Program Name\n");
        return 0;
    }

    dedup_type = argv[1][0];
    printf("%c\n" , dedup_type);

    num_input_files = atoi(argv[2]);
    printf("%d\n" , num_input_files);

    current_working_directory = calloc((strlen(argv[3]) + 1) , sizeof(char));
    strcpy(current_working_directory , argv[3]);
    printf("%s\n" , current_working_directory);

    /* Read the rest of the line to get all file names */
    files_to_read = malloc(num_input_files * sizeof(char*));
    for(int i = 0 ; i < num_input_files ; i++){
        files_to_read[i] = (char*)malloc((strlen(argv[4 + i]) + 1) * sizeof(char));
        strcpy(files_to_read[i] , argv[4 + i]);
        printf("-----> %s\n",files_to_read[i]);
    }
    roots = malloc(num_input_files* sizeof(*roots));

    /* File  Manipulation Variables */
    FILE *input_file = NULL;
    char buff[BUFFER_SIZE];
    bool read_empty_line_chucnks = false;
    bool finished_process_blocks = false;
    int block_line_count = 0;
    char* current_file = NULL;
    bool finished_reading_file = false;

    /* Initialize Global Variables */
    ht_files = ht_create('F');
    ht_physical_files = ht_create('F');
    ht_blocks = ht_create('B');
    ht_dirs = ht_create('D');


    if(ht_files == NULL || ht_blocks == NULL || ht_dirs == NULL ||ht_physical_files == NULL){
        printf("(Parser)--> Failed Allocating Hash Tables in parser =[ \n");
        return 0;
    }

    /* Define parameters for global data Manipulation (over entire input file) */
    List curr_depth_objects = NULL , previous_depth_objects = NULL;
    int global_current_depth = 0 ;
    curr_depth_objects = listCreate(object_info_copy , object_info_destroy);
    previous_depth_objects = listCreate(object_info_copy , object_info_destroy);

    /* Define parameters for reading data regarding SINGLE OBJECT */
    char file_system_ID[FILE_SYSTEM_ID_LEN+2];
    char* parent_dir_id = NULL; // Hashed ID of parent Directory
    unsigned short depth = 0; //Depth of current object in the hierarchy
    unsigned int file_size = 0; //File Size (if the object is a file)
    char obj_type = 'Z'; //Hexadecimal Value - Tells the type of the object ( A combination of binary flags set)
    char* object_id = NULL; //Hashed ID of the current object
    bool is_zero_size_file = false;
    bool file_was_created = false;
    bool object_exists_in_hash_already = false;
    bool set_root = false;

    /* ----------------------- Parameters Declarations & Initialization ----------------------- */
    /* ---------------------------------------------------------------------------------------- */
    /* ------------------------------------- File Reading ------------------------------------- */
    /* Go over all file systems */
    for (int i = 0; i < num_input_files ; ++i) { /* (1) Read an Input File */
        current_file = calloc((strlen(current_working_directory) + strlen(files_to_read[i]) + 1) , sizeof(char));
        strcpy(current_file , current_working_directory);
        strcat(current_file , files_to_read[i]);
        printf("(Parser)--> ----- Opening File %s ----- \n" , current_file);
        input_file = fopen(current_file , "r");
        if(input_file == NULL){ //check the file was opened successfully - if not terminate
            printf("(Parser)--> Can't open input file/s =[ \n");
            return 0;
        }

        free(current_file);
        printf("(Parser)-->  ----- Start Reading the file ----- \n");
        fgets(buff, BUFFER_SIZE , input_file); //Read First Line
        clear_line(buff);
        fgets(buff, BUFFER_SIZE , input_file); //Read Second Line
        clear_line(buff);
        fgets(buff, BUFFER_SIZE , input_file); //READFile System ID - get last 3 digits
        clear_line(buff);
        strncpy(file_system_ID , buff + 9 , 3);
        file_system_ID[FILE_SYSTEM_ID_LEN]='_';
        file_system_ID[FILE_SYSTEM_ID_LEN + 1]='\0';

        /* Skip till the first empty line - over the file system description */
        do{
            fgets(buff, BUFFER_SIZE , input_file);
            clear_line(buff);
        } while(strlen(buff) > 1);

        set_root = true;
        printf("(Parser)--> --- Skipped over the file-system data block successfully--- \n");

        /* Read File till the end - parse each block and add it to the corresponding structure */
        while(!feof(input_file)){
            fgets(buff, BUFFER_SIZE , input_file);
            clear_line(buff);
            block_line_count++;
            /* Check if we have reached the end of the file, nothing more to read */
            if((strcmp(buff , "LOGCOMPLETE\r\n") == 0) || (strcmp(buff , "LOGCOMPLETE\n") == 0)){
                /* Read the last line before EOF */
                fgets(buff, BUFFER_SIZE , input_file);
                clear_line(buff);
                finished_reading_file = true;
            }

            /* We haven't seen the LOGCOMPLETE line yet */
            /* Check if we haven't reached the end of the current input block */
            /***********************************************************************************************/
            while (strlen(buff) > 1 && !feof(input_file)){ /* Processing Object */
                switch(block_line_count){
                    case 1: /* DIRECTORY NAME */
                        parent_dir_id = case_1_directory_name(buff);
                        break;
                    case 4: /* NAMESPACE DEPTH */
                        depth = case_4_get_depth(buff);
                        //Check if current depth (in variable depth) is bigger than the one in global_current_depth
                        if(depth > global_current_depth){
                            //This means we have reached a new depth and can update parent_dir_sn for objects from previous levels
                            update_parent_dir_sn(previous_depth_objects , curr_depth_objects , global_current_depth , i);
                            //Update Object lists
                            printf("List Destroy - Depth %d \n",global_current_depth);
                            listDestroy(previous_depth_objects); //Empty the previous_depth_objects list
                            previous_depth_objects = listCopy(curr_depth_objects);//Copy the curr_depth_objects list to the previous_depth_objects
                            printf("List Clear - Depth %d \n",global_current_depth);
                            listClear(curr_depth_objects); //Empty the curr_depth_objects list
                            global_current_depth = depth;
                        }
                        break;
                    case 5: /* FILE SIZE */
                        file_size = case_5_file_size(buff);
                        break;
                    case 6: /* FILE ATTRIBUTES VALUE */
                        obj_type = case_6_file_attribute(buff);
                        if( ( file_size == 0 ) && ( obj_type == 'F' ) ){ // ignore zero size files
                            is_zero_size_file = true;
                        }
                        break;
                    case 7: /* FILE ID */
                        object_id = case_7_hash_file_id(buff , i , file_system_ID);
                        //Adding Directory Object to HashTable
                        if(obj_type == 'D'){
                            if( dir_sn == 0 || set_root == true){ //Creating Dummy Root Node using the Parent_dir_id of the first object in the input file
                                char* root_id = calloc(ROOT_ID_LEN , sizeof(char)); // The value is 8 chars
                                if(root_id == NULL){
                                    printf("(Parser) --> Can't Allocate place for ROOT_ID \n");
                                    break;
                                }
                                strcpy(root_id , file_system_ID);
                                strcat(root_id , "root");
                                root_id[8] = '\0';
                                roots[i] = ht_set(ht_dirs , root_id , -1 , dir_sn ,DIR_SIZE , 'D' , &object_exists_in_hash_already , 0 , dedup_type);
                                //root_directory = ht_set(ht_dirs , root_id , -1 , dir_sn ,DIR_SIZE , 'D' , &object_exists_in_hash_already);
                                dir_sn++;
                                free(root_id);
                                set_root = false;
                            }
                            //Create Directory Object with the retrieved data
                            ht_set(ht_dirs, object_id, depth, dir_sn, DIR_SIZE , 'D' , &object_exists_in_hash_already , 0, dedup_type);
                            dir_sn++;
                        }
                        break;
                    case 13: /* Line 13 is SV */
                        case_13_VS(input_file , buff , &block_line_count ,
                                   &read_empty_line_chucnks , depth , object_id,file_size,
                                   &file_was_created, &finished_process_blocks);
                        // Add object (File or Directory) to curr_depth_objects list
                        if ((obj_type == 'F') && (is_zero_size_file == false) && (file_was_created == true)){
                            Object_Info oi_file = object_info_create(object_id , (files_sn - 1) , parent_dir_id , 'F');
                            listInsertLast(curr_depth_objects , oi_file);
                            object_info_destroy(oi_file); //The list adds a copy of this object and it is no longer needed
                        } else if(obj_type == 'D'){ //Adding Directory Object to HashTable
                            //add directory to curr_depth_objects list in order to later find the parent directory
                            Object_Info oi_dir = object_info_create(object_id , (dir_sn - 1) , parent_dir_id , 'D');
                            listInsertLast(curr_depth_objects , oi_dir);
                            object_info_destroy(oi_dir); //The list adds a copy of this object and it is no longer needed
                        }
                        break;
                    default:
                        break;
                }

                if(read_empty_line_chucnks == false && finished_process_blocks == false){
                    fgets(buff, BUFFER_SIZE , input_file); //read next line in current block
                    clear_line(buff);
                    block_line_count++;
                }
            } /* Processing Object */

            /***********************************************************************************************/
            /******************* WE HAVE REACHED THE END OF THE CURRENT INPUT OBJECT !!! ********************/
            if(!feof(input_file)){ //Update parameters that are relevant to a single object
                block_line_count = 0; /* Zero the line count for the next block */
                read_empty_line_chucnks = false;
                is_zero_size_file = false;
                free(parent_dir_id);
                free(object_id);
                file_was_created = false;
                finished_process_blocks = false;
                parent_dir_id = NULL; // Hashed ID of parent Directory
                object_id = NULL;
                object_exists_in_hash_already = false;
            }
        }
        fclose(input_file);
        free(parent_dir_id);

        printf("(Parser) --> --- Finished reading the input file - Now lets start processing ---\n");
        //This means we have reached a new depth and can update parent_dir_sn for objects from previous levels
        update_parent_dir_sn(previous_depth_objects , curr_depth_objects , global_current_depth , i);

        if(finished_reading_file == true){
            global_current_depth = 0;
            printf("List Clear - Final - Curr \n",global_current_depth);
            listClear(curr_depth_objects); //Empty the curr_depth_objects list
            printf("List Clear - Final - Prev \n",global_current_depth);
            listClear(previous_depth_objects); //Empty the curr_depth_objects list
            block_line_count = 0; /* Zero the line count for the next block */
            read_empty_line_chucnks = false;
            is_zero_size_file = false;
            file_was_created = false;
            finished_process_blocks = false;
            object_exists_in_hash_already = false;
            finished_reading_file = false;
        }
    } /* (1) Read an Input File */

    printf("(Parser) --> Printing Results to CSV ................\n");
    print_ht_to_CSV(dedup_type, files_to_read , num_input_files,
                    blocks_sn, files_sn, dir_sn, physical_files_sn,
                    ht_files, ht_blocks, ht_dirs, ht_physical_files);

    printf("(Parser) --> Calculating hueristic ................\n");
    //TODO  - Save global depth/max depth for multiple input files

    //TODO - Create arrays of objects for each depth (save pointers to Dirs or Files)

    //TODO - Go to desired depth and define Merged Files for each folder

    //TODO - GO Over All Lower Depths and according to the parent folder MF merge the files to it

    //TODO - Reorganize all Serial numbers of files and folders

    //Free All Hash tables and Lists
    hashTable_destroy(ht_files , 'F' , dedup_type);
    hashTable_destroy(ht_dirs , 'D' , dedup_type);
    hashTable_destroy(ht_blocks , 'B' , dedup_type);
    hashTable_destroy(ht_physical_files , 'F' , dedup_type);
    printf("List Destroy - Final - Curr \n");
    listDestroy(curr_depth_objects);
    printf("List Prev - Final - Curr \n");
    listDestroy(previous_depth_objects);
    free(current_working_directory);
    for(int i = 0 ; i < num_input_files ; i++){
        free(files_to_read[i]);
    }
    free(files_to_read);
    free(roots);
    return 0;
}
