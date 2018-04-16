//
// Created by Polina on 16-Mar-18.
//

#include "TextParsing.h"

void clear_line(char* line){
    if(strlen(line) >= 2){
        int len_buff = strcspn(line , "\r\n");
        line[len_buff] = '\n';
        line[len_buff + 1] = '\0';
    }
}