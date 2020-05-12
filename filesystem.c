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

//fat_entry* FAT;
//directory_table_entry* directory_table_head;
//int fat_size;
const int BLOCK_SIZE = 4096;

sector generate_sector() {
    sector return_value;
    return_value.byte_count = 0;
    
    for(int i = 0; i < 512; i++) {
        return_value.bytes[i] = 0;
    }
    
    return return_value;
}

block generate_block(sector* base_address_of_sector_group) {
    block return_value;
    return_value.sectors = base_address_of_sector_group;
    return_value.used_sectors = 0;
    return return_value;
}

hard_drive* generate_hard_drive(int number_of_bytes) {
    hard_drive* return_value = malloc(sizeof(hard_drive));
    
    return_value->total_sectors = (int)ceil(number_of_bytes / 512.0);
    return_value->free_sectors = return_value->total_sectors;
    return_value->used_sectors = 0;
    
    return_value->sectors = malloc(sizeof(sector) * return_value->total_sectors);
    
    for(int i = 0; i < return_value->total_sectors; i++) {
        return_value->sectors[i] = generate_sector();
    }
    
    return return_value;
}

formatted_hard_drive* format_hard_drive(hard_drive* target) {
    formatted_hard_drive* return_value = malloc(sizeof(formatted_hard_drive));
    
    int sectors_per_block = BLOCK_SIZE / 512;
    
    return_value->number_of_blocks = target->total_sectors / sectors_per_block;
    return_value->directory_table_head = NULL;
    return_value->FAT = malloc(sizeof(fat_entry) * return_value->number_of_blocks);
    return_value->blocks = malloc(sizeof(block) * return_value->number_of_blocks);
    
    for(int i = 0; i < return_value->number_of_blocks; i++) {
        return_value->blocks[i] = generate_block(target->sectors + (i * sectors_per_block));
        return_value->FAT[i].block_pointer = &return_value->blocks[i];
        return_value->FAT[i].is_being_used = 0;
        return_value->FAT[i].next_block_index = -1;
    }
    
    return return_value;
}

formatted_hard_drive* file_system_initialize(int size_in_kilobytes) {
    //fat_size = (int)ceil(size_in_kilobytes / (double)(BLOCK_SIZE / 1024));
    
    //FAT = malloc(sizeof(fat_entry) * fat_size);
    
    /*for(int i = 0; i < fat_size; i++) {
        FAT[i].is_being_used = 0;
        FAT[i].next_block_index = -1;
    }*/
    
    return format_hard_drive(generate_hard_drive(size_in_kilobytes * 1024));
}

directory_table_entry* get_directory_table_tail(directory_table_entry* directory_table_head) {
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

int get_unused_block(formatted_hard_drive* drive) {
    int index = -1;
    do {
        index = rand() % drive->number_of_blocks;
    } while(index != -1 && drive->FAT[index].is_being_used);
    return index;
}

void file_system_allocate(char* file_name, int byte_size, formatted_hard_drive* drive) {
    int needed_blocks = (int)ceil((double)byte_size / (double)BLOCK_SIZE);
    
    int start_block_index = get_unused_block(drive);
    drive->FAT[start_block_index].is_being_used = 1;
    drive->FAT[start_block_index].next_block_index = -1;
    needed_blocks--;
    
    int last_block_index = start_block_index;
    while(needed_blocks > 0) {
        int new_block_index = get_unused_block(drive);
        drive->FAT[last_block_index].next_block_index = new_block_index;
        drive->FAT[new_block_index].is_being_used = 1;
        drive->FAT[new_block_index].next_block_index = -1;
        last_block_index = new_block_index;
        needed_blocks--;
    }
    
    directory_table_entry* tail = get_directory_table_tail(drive->directory_table_head);
    
    if(tail) {
        directory_table_entry* file_entry = malloc(sizeof(directory_table_entry));
        file_entry->file_name = malloc(sizeof(char) * strlen(file_name));
        strcpy(file_entry->file_name, file_name);
        file_entry->next_directory_table_entry = NULL;
        file_entry->start_block_index = start_block_index;
        
        tail->next_directory_table_entry = file_entry;
    } else {
        // first allocation of FS
        drive->directory_table_head = malloc(sizeof(directory_table_entry));
        drive->directory_table_head->file_name = malloc(sizeof(char) * strlen(file_name));
        drive->directory_table_head->start_block_index = start_block_index;
        strcpy(drive->directory_table_head->file_name, file_name);
        drive->directory_table_head->next_directory_table_entry = NULL;
    }
}

directory_table_entry* get_directory_table_entry(char* file_name, formatted_hard_drive* drive) {
    directory_table_entry* current_entry = drive->directory_table_head;
    
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

void print_block_indices(int last_block_index, formatted_hard_drive* drive) {
    int next_block_index = drive->FAT[last_block_index].next_block_index;
    printf("%d", next_block_index);
    
    if(drive->FAT[next_block_index].next_block_index == -1) {
        printf("\n");
    } else {
        printf(" -> ");
        print_block_indices(next_block_index, drive);
    }
}

void file_system_print_file(char* file_name, formatted_hard_drive* drive) {
    directory_table_entry* entry = get_directory_table_entry(file_name, drive);
    int start_block = entry->start_block_index;
        
    printf("%s:\n\tstored in block order: %d", file_name, start_block);
    
    if(drive->FAT[start_block].next_block_index == -1) {
        printf("\n");
    } else {
        printf(" -> ");
        print_block_indices(start_block, drive);
    }
}

void file_system_print(formatted_hard_drive* drive) {
    // print FAT
    printf("FILE SYSTEM:\n\tIDX\tUSED\tNEXT\n");
    for(int i = 0; i < drive->number_of_blocks; i++) {
        printf("\t%d\t\t%d\t\t%d\n", i, drive->FAT[i].is_being_used, drive->FAT[i].next_block_index);
    }
    printf("\n");
    
    // show files in FS
    directory_table_entry* current_entry = drive->directory_table_head;
    
    while(current_entry) {
        file_system_print_file(current_entry->file_name, drive);
        current_entry = current_entry->next_directory_table_entry;
    }
    printf("\n");
}
