//
//  inodemap.h
//  mytar
//
//  Created by Dorian Arnold on 10/8/20.
//  Copyright © 2020 Dorian Arnold. All rights reserved.
//

#ifndef inodemap_h
#define inodemap_h

#include <sys/types.h>
#include <sys/stat.h>

#define MAPSIZE 1024

const char * get_inode( ino_t );
void set_inode( ino_t, const char * );


#endif /* inodemap_h */
