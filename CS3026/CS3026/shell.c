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
#include <pthread.h>

void listDirectory(char *directoryPath);
void *mt_createFile(void *filePath);
void *mt_deleteFile(void *filePath);

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
    
    /*
     // Move file within virtual disk
     format();
     copyToVirtualDisk("main/copy.txt", "file.txt");
     moveFile("main/copy.txt", "/copy.txt");
     writedisk("virtualdisk_insideFileMove");
     */
    
    /*
    // Multi-threaded access
    format();
    // Create and start two threads
    pthread_t first, second;
    pthread_create(&first, NULL, mt_createFile, "file.txt");
    pthread_create(&second, NULL, mt_deleteFile, "file.txt");
    writedisk("virtualdisk_multiThreaded");
    */
    
    
    // Disk encryption
    format();
    // Copy file from hard disk to virtual disk
    copyToVirtualDisk("main/copy.txt", "file.txt");
    // Save encrypted disk
    writeEncryptedDisk("virtualdisk_encrypted", "edvinas");
    // Try to read disk with wrong password
    readEncryptedDisk("virtualdisk_encrypted", "wrongpassword");
    // Save results
    writedisk("virtualdisk_encrypted_wrong_password");
    // Try to read disk with correct password
    readEncryptedDisk("virtualdisk_encrypted", "edvinas");
    // Save results
    writedisk("virtualdisk_encrypted_correct_password");
    
    return 0;
}

void *mt_createFile(void *filePath) {
    pthread_mutex_lock(getVirtualDiskLock());
    MyFILE *file = myfopen(filePath, "w");
    myfputc('X', file);
    myfclose(file);
    pthread_mutex_unlock(getVirtualDiskLock());
    pthread_exit(NULL);
}

void *mt_deleteFile(void *filePath) {
    pthread_mutex_lock(getVirtualDiskLock());
    myremove(filePath);
    pthread_mutex_unlock(getVirtualDiskLock());
    pthread_exit(NULL);
}

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
