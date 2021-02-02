#include "pbm.h"
#include <stdlib.h>

PPMImage * new_ppmimage( unsigned int w, unsigned int h, unsigned int m )
{
    PPMImage* imageIn;
    imageIn = malloc(sizeof(PPMImage));
    imageIn -> width = w;
    imageIn -> height = h;
    imageIn -> max = m;
    for (int i = 0; i < 3; i++){
        imageIn -> pixmap[i] = malloc(h * sizeof(int*));
        for (int j = 0; j < h; j++) {
            imageIn -> pixmap[i][j] = malloc(w * sizeof(int));
        }
    }
    return imageIn;    //question: would the struct be destoryed after this function;
}

PBMImage * new_pbmimage( unsigned int w, unsigned int h )
{
    PBMImage* image = malloc(sizeof(PBMImage));
    image -> width = w;
    image -> height = h;
    image -> pixmap = malloc(h * sizeof(int*));
    for(int i = 0; i < h; i++){
        image -> pixmap[i] = malloc(w * sizeof(int));
    }
    return image;
}

PGMImage * new_pgmimage( unsigned int w, unsigned int h, unsigned int m )
{
    PGMImage* image = malloc(sizeof(PGMImage));
    image -> width = w;
    image -> height = h;
    image -> max = m;
    image -> pixmap = malloc(h * sizeof(int*));
    for(int i = 0; i < h; i++){
        image -> pixmap[i] = malloc(w * sizeof(int));
    }
    return image;
}

void del_ppmimage( PPMImage * p )
{
    free(p);
}

void del_pgmimage( PGMImage * p )
{
    free(p);
}

void del_pbmimage( PBMImage * p )
{
    free(p);
}

