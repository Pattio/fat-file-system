# fat-file-system

**File system** based on simplified concepts of **File Allocation Table** (FAT).

## Virtual disk
* Create virtual disk in memory
* Save virtual disk on hard disk
* Encrypt virtual disk
* Decrypt virtual disk

## Files
* Create new files
* Open existing files
* Write data into files
* Read data from files
* Delete files
* Copy files within virtual disk
* Move files within virtual disk
* Copy files from hard disk to virtual disk
* Copy files from virtual disk to hard disk

## Directories
* Create new directories
* Delete existing directories
* List directory content
* Change directory 
* Print current working directory

## Virtual disk hexdump
<img src="/resources/normal_disk.png" width="600" alt="Normal disk"/>

| Memory address           | Description            |
| ------------------------ |:----------------------:| 
| 00000000 - 00000400      | Special reserved space |
| 00000400 - 00000c00      | FAT table              |
| 00000c00 - 00001000      | root directory         |
| 00001000 - 00001400      | main directory         |
| 00001400 - 00001800      | copy.txt               |
| 00001800 - 00001c00      | secondary directory    |
| 00001c00 - 00002000      | othercopy.txt          |

## Encrypted virtual disk hexdump
<img src="/resources/encrypted_disk.png" width="600" alt="Encrypted disk"/>
