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
    // Set file mode
    strcpy(file->mode, mode);
    // Set first byte as the one is currently read/written
    file->pos = 0;
    file->currentBlock = 0;

    // If file already exists fill existing file data
    direntry_t *fileDirectory = findFileDirectoryInRoot(filename);
    if(fileDirectory != NULL) {
        file->blockno = fileDirectory->firstblock;
        file->dirEntry = fileDirectory;
        memcpy(file->buffer.data, virtualDisk[fileDirectory->firstblock].data, BLOCKSIZE);
        return file;
    }
    
    // If it's read only mode and we didn't find file return null
    if(strcmp(mode, "r") == 0) return NULL;
    
    // Find free block on FAT table
    int freeBlockIndex = freeFAT();
    // Set file to use that free block
    file->blockno = freeBlockIndex;
    // Update FAT table, which is store in memory
    FAT[freeBlockIndex] = ENDOFCHAIN;
    saveFAT();
    
    // Create and setup directory entry
    fileDirectory = malloc(sizeof(direntry_t));
    memset(fileDirectory, 0, sizeof(direntry_t));
    fileDirectory->filelength = 0;
    fileDirectory->isdir = 0;
    fileDirectory->firstblock = freeBlockIndex;
    strcpy(fileDirectory->name, filename);
    
    file->dirEntry = fileDirectory;
    // Put file entry into root directory
    rootDirectoryBlock->entrylist[rootDirectoryBlock->nextEntry] = *fileDirectory;
    rootDirectoryBlock->nextEntry++;
    
    return file;
}

// Close and save file
void myfclose(MyFILE *stream) {
    // Check that file exists
    if(stream == NULL) return;
    writeblock(&stream->buffer, stream->blockno);
    direntry_t *fileDirectory = findFileDirectoryInRoot(stream->dirEntry->name);
    memcpy(fileDirectory, stream->dirEntry, sizeof(direntry_t));
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
dirblock_t *getChildDirectoryBlock(dirblock_t *parentDirectoryBlock, const char *childDirectoryName) {
    for(int i = 0; i < parentDirectoryBlock->nextEntry; i++) {
        if(strcmp(parentDirectoryBlock->entrylist[i].name, childDirectoryName) == 0 &&
           parentDirectoryBlock->entrylist[i].isdir) {
            return &(virtualDisk[parentDirectoryBlock->entrylist[i].firstblock].dir);
        }
    }
    return NULL;
}

// Can find arbitary directory
dirblock_t *findDirectoryBlock(dirblock_t *parentDirectoryBlock, const char *directoryName) {
    // Go thourgh all child directories and if directory block is found return it
    for(int i = 0; i < parentDirectoryBlock->nextEntry; i++) {
        if(strcmp(parentDirectoryBlock->entrylist[i].name, directoryName) == 0 &&
           parentDirectoryBlock->entrylist[i].isdir) {
            return &(virtualDisk[parentDirectoryBlock->entrylist[i].firstblock].dir);
        } else if(parentDirectoryBlock->entrylist[i].isdir) {
            return findDirectoryBlock(&virtualDisk[parentDirectoryBlock->entrylist[i].firstblock].dir, directoryName);
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
    diskblock_t block; //= malloc(sizeof(dirblock_t));
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
    // Copy content of char pointer to char array
    char directoryPath[MAXPATHLENGTH];
    strcpy(directoryPath, path);
    // Check if given path is absolute
    int isAbsolute = directoryPath[0] == '/';
    dirblock_t *childDirectory = NULL;
    if(isAbsolute) childDirectory = rootDirectoryBlock;
    
    char *head, *tail = directoryPath;
    while ((head = strtok_r(tail, "/", &tail))) {
        if(childDirectory == NULL) {
            fprintf(stderr, "Couldn't find directory \n");
            return NULL;
        }
        childDirectory = getChildDirectoryBlock(childDirectory, head);
    }
    
    // Allocate memory for directory entries
    char **directoryEntries = malloc((DIRENTRYCOUNT + 1) * sizeof(char *));
    for(int i = 0; i < DIRENTRYCOUNT + 1; i++) directoryEntries[i] = malloc(MAXNAME);
    
    // Copy names from directory to directory enrries
    for(int i = 0; i < childDirectory->nextEntry; i++) {
        strcpy(directoryEntries[i], childDirectory->entrylist[i].name);
    }
    // Set last element to null
    directoryEntries[childDirectory->nextEntry] = NULL;
    return directoryEntries;
}
