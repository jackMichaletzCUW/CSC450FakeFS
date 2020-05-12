//
//  filesystem.h
//  FakeFS
//
//  Created by Jack Michaletz on 5/2/20.
//  Copyright © 2020 Jack Michaletz. All rights reserved.
//

#ifndef filesystem_h
#define filesystem_h

#include <stdio.h>

// singly linked list to store directory table
typedef struct directory_table_entry {
    char* file_name;
    int start_block_index; // the FAT index of the block that this file starts at
    struct directory_table_entry* next_directory_table_entry;
} directory_table_entry;

// structure to simulate a hard drive sector
typedef struct sector {
    int byte_count;
    char bytes[512];
} sector;

// structure to simulate file system blocks
typedef struct block {
    int used_sectors;
    sector* sectors;
} block;

typedef struct fat_entry {
    int is_being_used; // boolean
    block* block_pointer;
    int next_block_index; // points to the next block in the chain; if -1 means end of chain
} fat_entry;

// structure to simulate an unformatted (physical) hard drive
typedef struct hard_drive {
    int total_sectors;
    int used_sectors;
    int free_sectors;
    sector* sectors;
} hard_drive;

// structure to simulate a formatted hard drive
typedef struct formatted_hard_drive {
    int number_of_blocks;
    block* blocks;
    fat_entry* FAT;
    directory_table_entry* directory_table_head;
} formatted_hard_drive;

// constructors for structures
sector generate_sector(void);
block generate_block(sector* base_address_of_sector_group);
hard_drive* generate_hard_drive(int number_of_bytes);
formatted_hard_drive* format_hard_drive(hard_drive* target);

// allocates a file in the FAT and adds it to the directory table
void file_system_allocate(char* file_name, int byte_size, formatted_hard_drive* target_drive);

// initializes the file system to store a certain number of kilobytes
formatted_hard_drive* file_system_initialize(int size_in_kilobytes);

// prints out a representation of the filesystem
void file_system_print(formatted_hard_drive* target_drive);

// prints out the path a file takes through the FAT
void file_system_print_file(char* file_name, formatted_hard_drive* target_drive);

#endif /* filesystem_h */
