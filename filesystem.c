//
//  filesystem.c
//  FakeFS
//
//  Created by Jack Michaletz on 5/2/20.
//  Copyright © 2020 Jack Michaletz. All rights reserved.
//

#include "filesystem.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

fat_entry* FAT;
directory_table_entry* directory_table_head;
int fat_size;
const int BLOCK_SIZE = 4096;

void file_system_initialize(int size_in_kilobytes) {
    fat_size = (int)ceil(size_in_kilobytes / (double)(BLOCK_SIZE / 1024));
    
    FAT = malloc(sizeof(fat_entry) * fat_size);
    
    for(int i = 0; i < fat_size; i++) {
        FAT[i].is_being_used = 0;
        FAT[i].next_block_index = -1;
    }
}

directory_table_entry* get_directory_table_tail() {
    if(directory_table_head) {
        directory_table_entry* furthest_yet = directory_table_head;
        while(furthest_yet->next_directory_table_entry) {
            furthest_yet = furthest_yet->next_directory_table_entry;
        }
        return furthest_yet;
    } else {
        return NULL;
    }
}

int get_unused_block() {
    int index = -1;
    do {
        index = rand() % fat_size;
    } while(index != -1 && FAT[index].is_being_used);
    return index;
}

void file_system_allocate(char* file_name, int byte_size) {
    int needed_blocks = (int)ceil((double)byte_size / (double)BLOCK_SIZE);
    
    int start_block_index = get_unused_block();
    FAT[start_block_index].is_being_used = 1;
    FAT[start_block_index].next_block_index = -1;
    needed_blocks--;
    
    int last_block_index = start_block_index;
    while(needed_blocks > 0) {
        int new_block_index = get_unused_block();
        FAT[last_block_index].next_block_index = new_block_index;
        FAT[new_block_index].is_being_used = 1;
        FAT[new_block_index].next_block_index = -1;
        last_block_index = new_block_index;
        needed_blocks--;
    }
    
    directory_table_entry* tail = get_directory_table_tail();
    
    if(tail) {
        directory_table_entry* file_entry = malloc(sizeof(directory_table_entry));
        file_entry->file_name = malloc(sizeof(char) * strlen(file_name));
        strcpy(file_entry->file_name, file_name);
        file_entry->next_directory_table_entry = NULL;
        file_entry->start_block_index = start_block_index;
        
        tail->next_directory_table_entry = file_entry;
    } else {
        // first allocation of FS
        directory_table_head = malloc(sizeof(directory_table_entry));
        directory_table_head->file_name = malloc(sizeof(char) * strlen(file_name));
        directory_table_head->start_block_index = start_block_index;
        strcpy(directory_table_head->file_name, file_name);
        directory_table_head->next_directory_table_entry = NULL;
    }
}

directory_table_entry* get_directory_table_entry(char* file_name) {
    directory_table_entry* current_entry = directory_table_head;
    
    //strncat(file_name, "\n", strlen(file_name) + 1);
    
    while(current_entry) {
        if(strcmp(current_entry->file_name, file_name) == 0) {
            return current_entry;
        } else {
            current_entry = current_entry->next_directory_table_entry;
        }
    }
    
    return NULL;
}

void print_block_indices(int last_block_index) {
    int next_block_index = FAT[last_block_index].next_block_index;
    printf("%d", next_block_index);
    
    if(FAT[next_block_index].next_block_index == -1) {
        printf("\n");
    } else {
        printf(" -> ");
        print_block_indices(next_block_index);
    }
}

void file_system_print_file(char* file_name) {
    directory_table_entry* entry = get_directory_table_entry(file_name);
    int start_block = entry->start_block_index;
        
    printf("%s:\n\tstored in block order: %d", file_name, start_block);
    
    if(FAT[start_block].next_block_index == -1) {
        printf("\n");
    } else {
        printf(" -> ");
        print_block_indices(start_block);
    }
}

void file_system_print() {
    // print FAT
    printf("FILE SYSTEM:\n\tIDX\tUSED\tNEXT\n");
    for(int i = 0; i < fat_size; i++) {
        printf("\t%d\t\t%d\t\t%d\n", i, FAT[i].is_being_used, FAT[i].next_block_index);
    }
    printf("\n");
    
    // show files in FS
    directory_table_entry* current_entry = directory_table_head;
    
    while(current_entry) {
        file_system_print_file(current_entry->file_name);
        current_entry = current_entry->next_directory_table_entry;
    }
    printf("\n");
}
