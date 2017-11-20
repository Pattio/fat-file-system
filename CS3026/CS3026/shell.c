//
//  shell.c
//  CS3026
//
//  Created by Edvinas on 18/11/2017.
//  Copyright © 2017 Edvinas. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "filesys.h"


int main(int argc, const char * argv[]) {
    // Format new virtual disk
    format();
    mymkdir("/myfirstdir/myseconddir/mythirddir");
    
    char **directoryEntries = mylistdir("/myfirstdir/myseconddir");
    if(directoryEntries != NULL) {
        while(*directoryEntries != NULL) printf("%s \n", *(directoryEntries++));
        // free memory from directoriesEntriess
    }
    
    writedisk("virtualdiskB3_B1_a");
    printf("----write---\n");
    myfopen("/myfirstdir/myseconddir/testfile.txt", "w");
    
    char **newk = mylistdir("/myfirstdir/myseconddir");
    if(newk != NULL) {
        while(*newk != NULL) printf("%s \n", *(newk++));
        // free memory from directoriesEntriess
    }
    
    writedisk("virtualdiskB3_B1_b");
    return 0;
}
