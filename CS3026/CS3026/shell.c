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
    mymkdir("/edvinas/byla/awesome/");
    writedisk("virtualdiskB3_B1");
    return 0;
}
