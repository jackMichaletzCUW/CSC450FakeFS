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

typedef struct fat_entry {
    int is_being_used; // boolean
    int next_block_index; // points to the next block in the chain; if -1 means end of chain
} fat_entry;

// allocates a file in the FAT and adds it to the directory table
void file_system_allocate(char* file_name, int byte_size);

// initializes the file system to store a certain number of kilobytes
void file_system_initialize(int size_in_kilobytes);

// prints out a representation of the filesystem
void file_system_print(void);

// prints out the path a file takes through the FAT
void file_system_print_file(char* file_name);

#endif /* filesystem_h */
