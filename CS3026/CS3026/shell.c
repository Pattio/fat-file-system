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
    /*
    // Format new virtual disk
    format();
    // Create a directory “/firstdir/seconddir” in the virtual disk
    mymkdir("/firstdir/seconddir");
    // Call myfopen( “/firstdir/seconddir/testfile1.txt” )
    MyFILE *file = myfopen("/firstdir/seconddir/testfile1.txt", "w");
    myfputc('X', file);
    myfclose(file);
    // Call mylistdir(“/firstdir/seconddir”)
    listDirectory("/firstdir/seconddir");
    // Change to directory “/firstdir/seconddir”
    mychdir("/firstdir/seconddir");
    // Call mylistdir(“/firstdir/seconddir/” ) or mylistdir(“.”)
    listDirectory("/firstdir/seconddir");
    // Call myfopen( “testfile2.txt, “w” )
    MyFILE *file2 = myfopen("testfile2.txt", "w");
    myfputc('Y', file2);
    myfclose(file2);
    // Create directory “thirddir”
    mymkdir("thirddir");
    // Call myfopen( “thirddir/testfile3.txt, “w” )
    MyFILE *file3 = myfopen("thirddir/testfile3.txt", "w");
    myfputc('Z', file3);
    myfclose(file3);
    // Write out virtual disk to “virtualdiskA5_A1_a”
    writedisk("virtualdiskA5_A1_a");
    // Call myremove( “testfile1.txt” )
    myremove("testfile1.txt");
    // Call myremove( “testfile2.txt” )
    myremove("testfile2.txt");
    // Write out virtual disk to “virtualdiskA5_A1_b”
    writedisk("virtualdiskA5_A1_b");
    // Call mychdir (thirddir”)
    mychdir("thirddir");
    // Call myremove( “testfile3.txt”)
    myremove("testfile3.txt");
    // Write out virtual disk to “virtualdiskA5_A1_c”
    writedisk("virtualdiskA5_A1_c");
    // Call mychdir( “/firstdir/seconddir”) or mychdir(“..”)
    mychdir("/firstdir/seconddir");
    // Call myremdir( “thirddir” )
    myrmdir("thirddir");
    // Call mychdir(“/firstdir”)
    mychdir("/firstdir");
    // Call myrmdir ( “seconddir” )
    myrmdir("seconddir");
    // Call mychdir(“/”) or mychdir(“..”)
    mychdir("..");
    // Call myrmdir( “firstdir”)
    myrmdir("firstdir");
    // Write out virtual disk to “virtualdiskA5_A1_d”
    writedisk("virtualdiskA5_A1_d");
     */
    
    /*
    // Copy file from real disk to virtual disk
    format();
    copyToVirtualDisk("main/copy.txt", "file.txt");
    copyToRealDisk("copyFromVirtualDisk.txt", "main/copy.txt");
    writedisk("virtualdisk_fileCopy");
    */
    
    /*
    // Copy file within virtual disk
    format();
    copyToVirtualDisk("main/copy.txt", "file.txt");
    copyFile("main/copy.txt", "secondary/othercopy.txt");
    writedisk("virtualdisk_insideFileCopy");
    */
    
    return 0;
}
