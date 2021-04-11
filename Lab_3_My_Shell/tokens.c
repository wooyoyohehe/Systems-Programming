//
//  parse.c
//  mysh
//
//  Created by Dorian Arnold on 10/22/20.
//  Copyright © 2020 Dorian Arnold. All rights reserved.
//

#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char ** get_tokens( const char * line ) {
    char **tokens=NULL;
    char * line_copy;
    const char * delim = " \t\n";
    char * cur_token;
    int num_tokens=0;
    
    tokens = (char**) malloc( sizeof(char*) );
    tokens[0] = NULL;
    
    if( line == NULL )
        return tokens;
    
    line_copy = strdup( line );
    cur_token = strtok( line_copy, delim );
    if( cur_token == NULL )
        return tokens;
    
    do {
        num_tokens++;
        tokens = ( char ** ) realloc( tokens, (num_tokens+1) * sizeof( char * ) );
        tokens[ num_tokens - 1 ] = strdup(cur_token);
        tokens[ num_tokens ] = NULL;
    } while( (cur_token = strtok( NULL, delim )) );
    free(line_copy);
    
    return tokens;
}

void free_tokens( char ** tokens ) {
    if( tokens == NULL )
        return;
    
    for( int i=0; tokens[i]; i++ )
        free(tokens[i]);
    free(tokens);
}
