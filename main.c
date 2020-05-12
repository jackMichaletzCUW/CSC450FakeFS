//
//  main.c
//  FakeFS
//
//  Created by Jack Michaletz on 5/2/20.
//  Copyright © 2020 Jack Michaletz. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "filesystem.h"
#include "string.h"

char* input;
formatted_hard_drive* drive;

void process_input() {
    
    if(strcmp(input, "ADDFILE") == 0) {
        printf("What would you like your file to be called? ");
        scanf("%s", input);
        printf("How many bytes should %s be? ", input);
        int request = 0;
        scanf("%d", &request);
        
        file_system_allocate(input, request, drive);
        file_system_print_file(input, drive);
    } else if(strcmp(input, "PRINTFILE") == 0) {
        printf("What is the name of the file you would like to print? ");
        scanf("%s", input);
        
        file_system_print_file(input, drive);
    } else if(strcmp(input, "PRINTFS") == 0) {
        file_system_print(drive);
    } else {
        printf("Invalid command. Please try again.\n");
    }
    
    for(int i = 0; i < 100; i++) {
        input[i] = '\0';
    }
}

int main(int argc, const char * argv[]) {
    input = malloc(sizeof(char) * 100);
    
    printf("How many KB should I make your hard drive? ");
    int hdsize = 0;
    scanf("%d", &hdsize);
    
    drive = file_system_initialize(hdsize);
    
    printf("The commands you can use are ADDFILE, PRINTFILE, PRINTFS.\n");
    
    while(1) {
        printf("ENTER A COMMAND: ");
        scanf("%s", input);
        
        process_input();
    }
    
    return 0;
}
