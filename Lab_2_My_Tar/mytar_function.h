//
//  mytar_function.h
//  
//
//  Created by 龚晨 on 3/18/21.
//

#ifndef mytar_function_h
#define mytar_function_h
#include "inodemap.h"

#include <stdio.h>
#include <stdlib.h>

void print_tar (char* arch_name);

void create_archive (const char* dir_name, char* arch_name);

void create_archive_helper(FILE* F, const char* dir_name);





#endif /* mytar_function_h */
