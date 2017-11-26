/* filesys.h */

#ifndef FILESYS_H
#define FILESYS_H

#include <time.h>

#define rootDirectoryIndex 3
#define MAXBLOCKS     1024
#define BLOCKSIZE     1024
#define FATENTRYCOUNT (BLOCKSIZE / sizeof(fatentry_t))
#define DIRENTRYCOUNT ((BLOCKSIZE - (2*sizeof(int)) ) / sizeof(direntry_t))
#define MAXNAME       256
#define MAXPATHLENGTH 1024

#define UNUSED        -1
#define ENDOFCHAIN     0

typedef unsigned char Byte;
typedef short fatentry_t;
typedef fatentry_t fatblock_t [FATENTRYCOUNT];

typedef struct direntry {
   int entrylength;
   Byte isdir;
   Byte unused;
   time_t modtime;
   int filelength;
   fatentry_t firstblock;
   char name [MAXNAME];
} direntry_t;

typedef struct dirblock {
   int isdir;
   int nextEntry;
   direntry_t entrylist [DIRENTRYCOUNT];
} dirblock_t;

typedef Byte datablock_t [BLOCKSIZE];

// Volume block
typedef struct volumeBlock {
    pthread_mutex_t lock;
    char name[MAXNAME];
} volumeblock_t;

// Diskblock can be either a directory block,
// a FAT block, actual data or volume block
typedef union block {
    datablock_t data;
    dirblock_t dir;
    fatblock_t fat;
    volumeblock_t volume;
} diskblock_t;

// A list of diskblocks
extern diskblock_t virtualDisk [MAXBLOCKS];

typedef struct filedescriptor {
    int pos;
    int currentBlock;
    char mode[3];
    Byte writing;
    fatentry_t blockno;
    diskblock_t buffer;
    direntry_t *dirEntry;
} MyFILE;

/*******************
 Virtual disk functions
 ********************/
void format(void);
void readdisk (const char *filename);
void writedisk (const char * filename);
void writeEncryptedDisk(const char *filename, const char *password);
void readEncryptedDisk(const char *filename, const char *password);
void cleanVirtualDisk(short firstFATIndex);
pthread_mutex_t *getVirtualDiskLock(void);

/*******************
 FAT table functions
********************/
void loadFAT(void);
void saveFAT(void);
int freeFAT(void);

/*******************
 File functions
 ********************/
MyFILE *myfopen(const char *filename, const char *mode);
void myfclose(MyFILE *stream);
void myfputc(int b, MyFILE *stream);
int myfgetc(MyFILE *stream);
void myremove(const char *path);
void copyToVirtualDisk(const char *virtualDiskPath, const char *realDiskPath);
void copyToRealDisk(const char *realDiskPath, const char *virtualDiskPath);
int copyFile(const char *source, const char *destination);
void moveFile(const char *source, const char *destination);

/*******************
 Directory functions
 ********************/
dirblock_t *findDirectoryBlock(const char *path, char **filename, int modify);
dirblock_t *getChildDirectoryBlock(dirblock_t *parentDirectoryBlock, const char *childDirectoryName);
dirblock_t *createDirectoryBlock(dirblock_t *parentDirectoryBlock, const char *directoryName);
dirblock_t *findParentBlock(dirblock_t *startingBlock, dirblock_t *block);
void deleteBlock(dirblock_t *block);
void mymkdir(const char *path);
void myrmdir(const char *path);
char **mylistdir(const char *path);
void mychdir(const char *path);
void pwd(void);
#endif
