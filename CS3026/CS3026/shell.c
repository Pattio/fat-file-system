//
//  shell.c
//  CS3026
//
//  Created by Edvinas on 18/11/2017.
//  Copyright © 2017 Edvinas. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filesys.h"

void listDirectory(char *directoryPath) {
    char **directoryEntries = mylistdir(directoryPath);
    printf("Printing contents of %s:\n", directoryPath);
    if(directoryEntries != NULL) {
        int i = 0;
        while(directoryEntries[i] != NULL) {
            printf("%s \n", directoryEntries[i]);
            free(directoryEntries[i]);
            i++;
        }
    } else {
        printf("Directory is empty\n");
    }
    printf("---------------\n");
}

int main(int argc, const char * argv[]) {
    // Format new virtual disk
    format();
    
    mymkdir("/myfirstdir/myseconddir/mythirddir");
    listDirectory("/myfirstdir/myseconddir");
    mychdir("/myfirstdir/myseconddir/mythirddir");

    return 0;
}
