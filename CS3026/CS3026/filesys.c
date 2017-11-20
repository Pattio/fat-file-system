/* filesys.c
 * 
 * provides interface to virtual disk
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "filesys.h"


diskblock_t  virtualDisk [MAXBLOCKS] ;           // define our in-memory virtual, with MAXBLOCKS blocks
fatentry_t   FAT         [MAXBLOCKS] ;           // define a file allocation table with MAXBLOCKS 16-bit entries


dirblock_t *rootDirectoryBlock = &(virtualDisk[rootDirectoryIndex].dir);

direntry_t * currentDir              = NULL ;
fatentry_t   currentDirIndex         = 0 ;

/* writedisk : writes virtual disk out to physical disk
 * 
 * in: file name of stored virtual disk
 */

void writedisk ( const char * filename )
{
   printf ( "writedisk> virtualdisk[0] = %s\n", virtualDisk[0].data ) ;
   FILE * dest = fopen( filename, "w" ) ;

   if ( fwrite ( virtualDisk, sizeof(virtualDisk), 1, dest ) < 0 )
      fprintf ( stderr, "write virtual disk to disk failed\n" ) ;
   //write( dest, virtualDisk, sizeof(virtualDisk) ) ;
   fclose(dest) ;
   
}

void readdisk (const char *filename)
{
    FILE *dest = fopen(filename, "r");
    if (fread (virtualDisk, sizeof(virtualDisk), 1, dest) < 0)
        fprintf(stderr, "write virtual disk to disk failed\n");
    else
        loadFAT();
    fclose(dest) ;
}


/* the basic interface to the virtual disk
 * this moves memory around
 */

void writeblock ( diskblock_t * block, int block_address )
{
   //printf ( "writeblock> block %d = %s\n", block_address, block->data ) ;
   memmove ( virtualDisk[block_address].data, block->data, BLOCKSIZE ) ;
   //printf ( "writeblock> virtualdisk[%d] = %s / %d\n", block_address, virtualDisk[block_address].data, (int)virtualDisk[block_address].data ) ;
}


void format() {
    // Wipe alll data from virtual disk
    for(int i = 0; i < MAXBLOCKS; i++)
        memset(virtualDisk[i].data, 0, BLOCKSIZE);
    
    // Create block buffer
    diskblock_t block;

    // Clean block buffer
    memset(block.data, 0, BLOCKSIZE);
    // Put some data to buffer
    strcpy(block.data, "CS3026 Operating Systems Assessment");
    // Write buffer to virtual disk
    writeblock(&block, 0);

    // Clean block buffer
    memset(block.data, 0, BLOCKSIZE);
    // Setup root directory and write it to virtual disk
    block.dir.isdir = 1;
    block.dir.nextEntry = 0;
    writeblock(&block, 3);
    

    // Set all fat entries as unused and save FAT block to virtual disk
    memset(FAT, UNUSED, MAXBLOCKS * 2);
    FAT[0] = ENDOFCHAIN;
    FAT[1] = 2;
    FAT[2] = ENDOFCHAIN;
    FAT[3] = ENDOFCHAIN;
    saveFAT();
}

void printBlock ( int blockIndex )
{
   printf ( "virtualdisk[%d] = %s\n", blockIndex, virtualDisk[blockIndex].data ) ;
}


/*******************
 FAT table functions
 ********************/

// Load FAT table to memory
void loadFAT() {
    // Read first part of fat table
    memcpy(FAT, virtualDisk[1].fat, MAXBLOCKS);
    // Read second part of fat table
    memcpy(FAT + FATENTRYCOUNT - 1, virtualDisk[2].fat, MAXBLOCKS);
}

// Save FAT to virtual disk
void saveFAT() {
    memcpy(virtualDisk[1].fat, FAT, MAXBLOCKS);
    memcpy(virtualDisk[2].fat, FAT + FATENTRYCOUNT - 1, MAXBLOCKS);
}

// find free FAT entry, if not found return -1
int freeFAT() {
    for(int i = 0; i < MAXBLOCKS; i++) {
        if(FAT[i] == UNUSED) return i;
    }
    return -1;
}


/*******************
 File functions
 ********************/

direntry_t *findFileDirectoryInRoot(const char *filename) {
    for(int i = 0; i < DIRENTRYCOUNT; i++) {
        if(!rootDirectoryBlock->entrylist[i].isdir &&
           strcmp(rootDirectoryBlock->entrylist[i].name, filename) == 0)
            return &(rootDirectoryBlock->entrylist[i]);
    }
    return NULL;
}

// Open file
MyFILE *myfopen(const char *filename, const char *mode) {
    // Allocate and clean memory for file
    MyFILE *file = malloc(sizeof(MyFILE));
    memset(file->buffer.data, 0, BLOCKSIZE);
    // Setup file default data
    strcpy(file->mode, mode);
    file->pos = 0;
    file->currentBlock = 0;
    
    if(strcmp(mode, "r") == 0) myfopenRead(filename, &file);
    else if (strcmp(mode, "w") == 0) myfopenWrite(filename, &file);
    
    return file;
}

static void myfopenRead(const char *filePath, MyFILE **file) {
    // Find file directory block
    char *fileName = NULL;
    dirblock_t *directoryBlock = findDirectoryBlock(filePath, &fileName, 0);
    // If directory or file not found set file to null and stop execution
    if(directoryBlock == NULL || fileName == NULL) {
        *file = NULL;
        return;
    }
    
    // Go through each entry in directory and if file is found
    // fill file with all the data
    int foundFile = 0;
    for(int i = 0; i < directoryBlock->nextEntry; i++) {
        if(strcmp(directoryBlock->entrylist[i].name, fileName) == 0 &&
           !directoryBlock->entrylist[i].isdir) {
                (*file)->blockno = directoryBlock->entrylist[i].firstblock;
                (*file)->dirEntry = &directoryBlock->entrylist[i];
                memcpy((*file)->buffer.data, virtualDisk[(*file)->blockno].data, BLOCKSIZE);
                foundFile = 1;
        }
    }
    
    if(!foundFile) *file = NULL;
}

static void myfopenWrite(const char *filePath, MyFILE **file) {
    char *fileName = NULL;
    dirblock_t *directoryBlock = findDirectoryBlock(filePath, &fileName, 1);
    
    // Failed to create directory block
    if(directoryBlock == NULL) return;
    
    // Go through each entry in directory and if file is found
    // fill file with all the data
    for(int i = 0; i < directoryBlock->nextEntry; i++) {
        if(strcmp(directoryBlock->entrylist[i].name, fileName) == 0 &&
           !directoryBlock->entrylist[i].isdir) {
            // TODO: CHANGE TO USE BLOCKNO OF CURRENT BUFFER NOT BEGINING
                (*file)->blockno = directoryBlock->entrylist[i].firstblock;
                (*file)->dirEntry = &directoryBlock->entrylist[i];
                memcpy((*file)->buffer.data, virtualDisk[(*file)->blockno].data, BLOCKSIZE);
                return;
        }
    }
    
    // Find and use free block on FAT table
    int freeBlockIndex = freeFAT();
    (*file)->blockno = freeBlockIndex;
    FAT[freeBlockIndex] = ENDOFCHAIN;
    saveFAT();
    
    // Create and setup directory entry
    direntry_t *fileDirectory = malloc(sizeof(direntry_t));
    memset(fileDirectory, 0, sizeof(direntry_t));
    fileDirectory->filelength = 0;
    fileDirectory->isdir = 0;
    fileDirectory->firstblock = freeBlockIndex;
    strcpy(fileDirectory->name, fileName);
    (*file)->dirEntry = fileDirectory;
    
    // Put file entry into current directory
    directoryBlock->entrylist[directoryBlock->nextEntry] = *fileDirectory;
    directoryBlock->nextEntry++;
}

// Close and save file
void myfclose(MyFILE *stream) {
    // Check that file exists
    if(stream == NULL) return;
    writeblock(&stream->buffer, stream->blockno);
//    findDirectoryBlock(<#const char *path#>, <#char **filename#>, <#int modify#>)
//    stream->dirEntry
//    direntry_t *fileDirectory = findFileDirectoryInRoot(stream->dirEntry->name);
//    memcpy(fileDirectory, stream->dirEntry, sizeof(direntry_t));
    free(stream);
}

// Put char into file
void myfputc(int b, MyFILE *stream) {
    // If file is in read mode don't let to modify it
    if(stream == NULL || strcmp(stream->mode, "r") == 0) return;
    
    // Put byte into the buffer
    stream->buffer.data[stream->pos] = b;
    // Increase current active byte position
    stream->pos++;
    stream->dirEntry->filelength++;
    
    // If after writing new byte buffer becomes full, write it to virtualdisk
    // and allocate new block
    if(stream->pos == BLOCKSIZE - 1) {
        // Write old block
        writeblock(&stream->buffer, stream->blockno);
        int freeBlockIndex = freeFAT();
        // Point to next block
        FAT[stream->blockno] = freeBlockIndex;
        FAT[freeBlockIndex] = ENDOFCHAIN;
        saveFAT();
        stream->blockno = freeBlockIndex;
        stream->pos = 0;
    }
}

// Read char from file
int myfgetc(MyFILE *stream) {
    // Check if file exists
    if(stream == NULL) return EOF;
    // If it's end of the file return EOF
    if((stream->currentBlock * BLOCKSIZE + stream->pos) == stream->dirEntry->filelength) return EOF;
    
    // Get char from buffer and increase buffer position
    int charInt = stream->buffer.data[stream->pos];
    stream->pos++;
    
    // If reached end of the buffer try to load
    // new block
    if(stream->pos == BLOCKSIZE) {
        int nextBlock = FAT[stream->blockno];
        if(nextBlock == UNUSED) {
            return EOF;
        }
        memcpy(stream->buffer.data, virtualDisk[nextBlock].data, BLOCKSIZE);
        stream->blockno = nextBlock;
        stream->pos = 0;
        stream->currentBlock++;
    }

    return charInt;
}


/*******************
 Directory functions
 ********************/
// If directory path is given function returns last element directory
// if directory with filename is given it will return last element directory
// and changes filename to point to name of the file
dirblock_t * findDirectoryBlock(const char *path, char **filename, int modify) {
    char blockPath[MAXPATHLENGTH];
    strcpy(blockPath, path);
    
    dirblock_t *parentDirectoryBlock = NULL;
    int isAbsolute = blockPath[0] == '/';
    if(isAbsolute) parentDirectoryBlock = rootDirectoryBlock;
    
    char *head, *tail = blockPath;
    while ((head = strtok_r(tail, "/", &tail))) {
        // If current token is file set filename
        // and return its parent directory
        if(strchr(head, '.') != NULL) {
            *filename = malloc(MAXNAME);
            strcpy(*filename, head);
            return parentDirectoryBlock;
        }
        
        int found = 0;
        for(int i = 0; i < parentDirectoryBlock->nextEntry; i++) {
            if(strcmp(parentDirectoryBlock->entrylist[i].name, head) == 0 &&
               parentDirectoryBlock->entrylist[i].isdir) {
                parentDirectoryBlock = &(virtualDisk[parentDirectoryBlock->entrylist[i].firstblock].dir);
                found = 1;
            }
        }
        
        // If directory not found and modify is set to true, create new directory
        if(found == 0 && modify == 0) return NULL;
        else if(found == 0 && modify) parentDirectoryBlock = createDirectoryBlock(parentDirectoryBlock, head);
    
    }
    return parentDirectoryBlock;
}

dirblock_t *getChildDirectoryBlock(dirblock_t *parentDirectoryBlock, const char *childDirectoryName) {
    for(int i = 0; i < parentDirectoryBlock->nextEntry; i++) {
        if(strcmp(parentDirectoryBlock->entrylist[i].name, childDirectoryName) == 0 &&
           parentDirectoryBlock->entrylist[i].isdir) {
            return &(virtualDisk[parentDirectoryBlock->entrylist[i].firstblock].dir);
        }
    }
    return NULL;
}

dirblock_t *createDirectoryBlock(dirblock_t *parentDirectoryBlock, const char *directoryName) {
    // Go thourgh all child directories and if directory block is found return it
    for(int i = 0; i < parentDirectoryBlock->nextEntry; i++) {
        if(strcmp(parentDirectoryBlock->entrylist[i].name, directoryName) == 0 &&
           parentDirectoryBlock->entrylist[i].isdir) {
            return &(virtualDisk[parentDirectoryBlock->entrylist[i].firstblock].dir);
        }
    }
    
    // Can't create more directories, because it's already full
    if(parentDirectoryBlock->nextEntry == DIRENTRYCOUNT) {
        fprintf(stderr, "Can't create child directory, because parent directory is full");
        return NULL;
    }

    int newDirectoryIndex = freeFAT();
    // Create directory block
    diskblock_t block;
    memset(block.data, 0, BLOCKSIZE);
    block.dir.isdir = 1;
    block.dir.nextEntry = 0;
    writeblock(&block, newDirectoryIndex);
    FAT[newDirectoryIndex] = ENDOFCHAIN;
    saveFAT();
    
    // Create and add directory entry to parent directory
    direntry_t directoryEntry;
    memset(&directoryEntry, 0, sizeof(direntry_t));
    directoryEntry.isdir = 1;
    directoryEntry.firstblock = newDirectoryIndex;
    strcpy(directoryEntry.name, directoryName);
    parentDirectoryBlock->entrylist[parentDirectoryBlock->nextEntry] = directoryEntry;
    parentDirectoryBlock->nextEntry++;
    return &virtualDisk[newDirectoryIndex].dir;
}

void mymkdir(const char *path) {
    
    // Copy content of char pointer to char array
    char directoryPath[MAXPATHLENGTH];
    strcpy(directoryPath, path);
    // Check if given path is absolute
    int isAbsolute = directoryPath[0] == '/';
    
    dirblock_t *parent = NULL;
    if(isAbsolute) parent = rootDirectoryBlock;

    // Create directories from given string
    char *head;
    char *tail = directoryPath;
    while ((head = strtok_r(tail, "/", &tail))) {
        if(parent == NULL) {
            fprintf(stderr, "Can't create directory, parent directory either full or doesn't exist \n");
            break;
        }
        parent = createDirectoryBlock(parent, head);
    }
}

char **mylistdir(const char *path) {
    // Find directory block of last element in path
    dirblock_t *directoryBlock = findDirectoryBlock(path, NULL, 0);
    // If directory doesn't exist return null
    if(directoryBlock == NULL) return NULL;
    
    // Allocate memory for directory entries
    char **directoryEntries = malloc((DIRENTRYCOUNT + 1) * sizeof(char *));
    for(int i = 0; i < DIRENTRYCOUNT + 1; i++) directoryEntries[i] = malloc(MAXNAME);
    
    // Copy names from directory to directory enrries
    for(int i = 0; i < directoryBlock->nextEntry; i++) {
        strcpy(directoryEntries[i], directoryBlock->entrylist[i].name);
    }
    
    // Set last element to null
    directoryEntries[directoryBlock->nextEntry] = NULL;
    return directoryEntries;
}
