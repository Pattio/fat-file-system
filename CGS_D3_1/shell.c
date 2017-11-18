//
//  shell.c
//  CS3026
//
//  Created by Edvinas on 18/11/2017.
//  Copyright © 2017 Edvinas. All rights reserved.
//

#include <stdio.h>
#include "filesys.h"

int main(int argc, const char * argv[]) {
    format();
    writedisk("virtualdiskD3_D1");
    return 0;
}
