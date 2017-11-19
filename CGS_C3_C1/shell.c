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
#define testTextSize 4 * BLOCKSIZE
FILE *outputFile;

void putTestData(MyFILE *file) {
    int currentChar = 'A';
    char myRandomText[testTextSize];
    for(int i = 0; i < testTextSize; i++) {
        myRandomText[i] = currentChar++;
        if(i % 25 == 0 && i != 0) currentChar = 'A';
    }
    for(int i = 0; i < testTextSize; i++) myfputc(myRandomText[i], file);
}

void readData(MyFILE *file) {
    outputFile = fopen("testfileC3_C1_copy.txt", "w");
    int number = 0;
    int value = myfgetc(file);
    while(value != EOF) {
        printf("%c", value);
        // Put char into output file
        fputc(value, outputFile);
        number++;
        value = myfgetc(file);
    }
    fclose(outputFile);
}

int main(int argc, const char * argv[]) {
    // Format new virtual disk
    format();
    // Create file on virtual disk
    MyFILE *file = myfopen("testfile.txt", "w");
    // Put test data to file
    putTestData(file);
    // Close file and save virtual disk
    myfclose(file);
    file = NULL;
    writedisk("virtualdiskC3_C1");
    
    // Open already existing file and read data from it
    file = myfopen("testfile.txt", "w");
    readData(file);
    myfclose(file);
    
    return 0;
}
