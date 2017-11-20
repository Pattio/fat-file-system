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
    mymkdir("/firstdir/seconddir");
    MyFILE *file = myfopen("/firstdir/seconddir/testfile1.txt", "w");
    myfputc('X', file);
    myfclose(file);
    listDirectory("/firstdir/seconddir");
    mychdir("/firstdir/seconddir");
    listDirectory("/firstdir/seconddir");
    MyFILE *file2 = myfopen("testfile2.txt", "w");
    myfputc('Y', file2);
    myfclose(file2);
    mymkdir("thirddir");
    MyFILE *file3 = myfopen("thirddir/testfile3.txt", "w");
    myfputc('Z', file3);
    myfclose(file3);
    writedisk("virtualdiskA5_A1_a");
    
    pwd();

    return 0;
}
