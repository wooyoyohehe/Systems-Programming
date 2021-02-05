#include "pbm.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct
{
    char mode;
    int arg;
    char* infile_name;
    char* outfile_name;
    
}Option;

void option_b(Option op, PPMImage* image_in);
void option_g(Option op, PPMImage* image_in);
void option_i(Option op, PPMImage* image_in);
void option_r(Option op, PPMImage* image_in);
void option_s(Option op, PPMImage* image_in);
void option_m(Option op, PPMImage* image_in);
void option_t(Option op, PPMImage* image_in);
void option_n(Option op, PPMImage* image_in);
void option_o(Option op, PPMImage* image_in);


Option read_options(int arc, char*argv[]);//return the option, and handle the exceptions, assign file names;

void image_transform (Option op, PPMImage image_in);


int main( int argc, char *argv[] )
{
    Option option = read_options(argc, argv);
    
    PPMImage* image_in = read_ppmfile(option.infile_name);
    
    //do transformation base on option
    switch(option.mode) {     // finish all the rest things here?
        case 'b':
            option_b(option, image_in);
            break;
        case 'g':
            option_g(option, image_in);
            break;
        case 'i':
            option_i(option, image_in);
            break;
        case 'r':
            option_r(option, image_in);
            break;
        case 's':
            option_s(option, image_in);
            break;
        case 'm':
            option_m(option, image_in);
            break;
        case 't':
            option_t(option, image_in);
            break;
        case 'n':
            option_n(option, image_in);
            break;
    }
    
    
    
    
    
    return 0;
}



Option read_options(int argc, char*argv[]){
    int o_find = 0;
    int op_found = 0; //only accept one option except for -o
    Option option;
    
    int o;
    
    while( (o=getopt(argc,argv,"bg:i:r:smt:n:o:")) != -1){
        if(op_found && o != 'o'){    // check if multiple options are given
            fprintf(stderr, "Error: Multiple transformations specified\n");
            exit(1);
        }
        switch (o) {
            case 'b':
                option.mode = 'b';
                op_found = 1;
                break;
                
            case 'g':
                option.mode = 'g';
                op_found = 1;
                char* ptr;
                int num;
                num = (int)strtol(optarg, &ptr, 10);
                if(num > 65535){
                    fprintf(stderr, "Error: Invalid max grayscale pixel value: %d; must be less than 65,536\n", num);
                    exit(1);
                }
                option.arg = num;
                break;
                
            case 'i':
                option.mode = 'i';
                op_found = 1;
                if(strcmp(optarg, "red") == 0){
                    option.arg = 0;
                } else if(strcmp(optarg, "green") == 0){
                    option.arg = 1;
                } else if(strcmp(optarg, "blue") == 0){
                    option.arg = 2;
                } else {
                    fprintf(stderr, "Error: Invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", optarg);
                    exit(1);
                }
                break;
                
            case 'r':
                option.mode = 'r';
                op_found = 1;
                if(strcmp(optarg, "red") == 0){
                    option.arg = 0;
                } else if(strcmp(optarg, "green") == 0){
                    option.arg = 1;
                } else if(strcmp(optarg, "blue") == 0){
                    option.arg = 2;
                } else {
                    fprintf(stderr, "Error: Invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", optarg);
                    exit(1);
                }
                break;
                
            case 's':
                option.mode = 's';
                op_found = 1;
                break;
                
            case 'm':
                option.mode = 'm';
                op_found = 1;
                break;
                
            case 't':
                option.mode = 't';
                op_found = 1;
                char* ptr2;
                int num2;
                num2 = (int)strtol(optarg, &ptr2, 10);
                if(num2 > 8 || num2 < 1){
                    fprintf(stderr, "Error: Invalid scale factor: %d; must be 1-8\n", num2);
                    exit(1);
                }
                option.arg = num2;
                break;
                
            case 'n':
                option.mode = 'n';
                op_found = 1;
                char* ptr3;
                int num3;
                num3 = (int)strtol(optarg, &ptr3, 10);
                if(num3 > 8 || num3 < 1){
                    fprintf(stderr, "Error: Invalid scale factor: %d; must be 1-8\n", num3);
                    exit(1);
                }
                option.arg = num3;
                break;
                
            case 'o': // assign the output file.
                o_find = 1;
                option.outfile_name = optarg;
                
                       //not necessarily follow o.
//                printf("outfilename:%s\n", argv[optind]);
//                printf("inputfilename:%s\n", argv[optind+1]);
                break;
            default:
                fprintf(stderr, "Usage: ppmcvt [-bgirsmtno] [FILE]\n");
                exit(1);
                break;
        }
    }
    if(optind >= argc){
        fprintf(stderr,"Error: No input file specified\n");
        exit(1);
    }
    
    option.infile_name = argv[optind];
    
    if (!op_found)      // set default option b
        option.mode = 'b';
    
    if (o_find == 0) {
        fprintf(stderr, "Error: No output file specified\n");
        exit(1);
    }
    
    return option;
}


void helper_check_PPMIImage(PPMImage* image_in){
    int h = image_in -> height;
    int w = image_in -> width;
    int max = image_in -> max;
    
    printf("PPMMAX is %d", max);
    float PPMMax = (float)image_in -> max;
    float PPMMaxabovetwo = PPMMax/2;
    printf("PPMMax/2 = %f\n", PPMMaxabovetwo);
    
    for (int color = 0; color < 3; color++){
        for(int r = 0; r < h; r++){
            for(int c = 0; c < w; c++){
                printf(" %d", image_in -> pixmap[color][r][c]);
            }
            printf("\n");
        }
        printf("\n\n");
    }
    
}

void option_b(Option op, PPMImage* image_in){  //convert input file to a Portable Bitmap (PBM) file
    //alloc new outfile
    //transform
    //write
    float PPMMax = (float)image_in -> max;
    float PPMMaxabovetwo = PPMMax/2;
    
    PBMImage* image_out = new_pbmimage(image_in -> width, image_in -> height);
//    helper_check_PPMIImage(image_in);
    //To compute black and white bits from RGB pixels use: 𝐴𝑣𝑒𝑟𝑎𝑔𝑒(𝑅 + 𝐺 + 𝐵) < 𝑃𝑃𝑀𝑀𝑎𝑥/2
    for (int r = 0; r < image_in -> height; r++) {
        for (int c = 0; c < image_in -> width; c++){
            float RGB_sum = 0;
            for (int color = 0; color < 3; color++){
                RGB_sum = RGB_sum + (float)image_in -> pixmap[color][r][c];
            }
            float average_RGB = RGB_sum / 3;
            
//            printf("RGB/3 = %f\n", average_RGB);
            if (average_RGB < PPMMax/2){
                image_out -> pixmap[r][c] = 1;
//                printf("[%d][%d] is 0\n\n", r,c);
            } else {
                image_out -> pixmap[r][c] = 0;
//                printf("[%d][%d] is 1\n\n", r,c);
            }
        }
    }
    
    write_pbmfile(image_out, op.outfile_name);
    del_ppmimage(image_in);
    del_pbmimage(image_out);
    
}


void option_g(Option op, PPMImage* image_in){
    //convert input file to a Portable Gray Map (PGM) file using the specified max grayscale pixel value [1-65535].
    int h = image_in -> height;
    int w = image_in -> width;
    int max = image_in -> max;
    int PGMMax = op.arg;
    
    
    
    PGMImage* image_out = new_pgmimage(image_in -> width, image_in -> height, PGMMax);
    
    //To compute grayscale pixels from RGB pixels use: (𝐴𝑣𝑒𝑟𝑎𝑔𝑒(𝑅 + 𝐺 + 𝐵)/𝑃𝑃𝑀𝑀𝑎𝑥) × 𝑃𝐺𝑀𝑀𝑎𝑥
    float PPMMax = (float)max;
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++){
            float RGB_sum = 0;
            for (int color = 0; color < 3; color++){
                RGB_sum = RGB_sum + (float)image_in -> pixmap[color][r][c];
            }
            
            float average_RGB = RGB_sum / 3;
            int value = (int)((average_RGB / PPMMax) * PGMMax);
            if(value > max){
                image_out -> pixmap[r][c] = max;
            } else {
                image_out -> pixmap[r][c] = value;
            }
            
        
        }
    }
    
    write_pgmfile(image_out, op.outfile_name);
    del_ppmimage(image_in);
    del_pgmimage(image_out);
}


void option_i(Option op, PPMImage* image_in){
    //isolate the specified RGB channel. Valid channels are “red”, “green”, or “blue”.
    
    int h = image_in -> height;
    int w = image_in -> width;
    int max = image_in -> max;
    for (int r = 0; r < h; r++){
        for (int c = 0; c < w; c++){
            for (int color = 0; color < 3; color++ ){
                if(color == op.arg)
                    continue;
                image_in -> pixmap[color][r][c] = 0;
            }
            
        }
    }
    write_ppmfile(image_in, op.outfile_name);
    del_ppmimage(image_in);
    
}

void option_r(Option op, PPMImage* image_in){
    int color = op.arg;
    int h = image_in -> height;
    int w = image_in -> width;
    int max = image_in -> max;
    for (int r = 0; r < h; r++){
        for (int c = 0; c < w; c++){
            image_in -> pixmap[color][r][c] = 0;
        }
    }
    write_ppmfile(image_in, op.outfile_name);
    del_ppmimage(image_in);
}

void option_s(Option op, PPMImage* image_in){
    //apply a sepia transformation
    
    int h = image_in -> height;
    int w = image_in -> width;
    int max = image_in -> max;
    int PGMMax = op.arg;
    
    //𝑁𝑒𝑤𝑅 = 0.393(𝑂𝑙𝑑𝑅) + 0.769(𝑂𝑙𝑑𝐺) + 0.189𝑥(𝑂𝑙𝑑𝐵)
    //𝑁𝑒𝑤𝐺 = 0.349(𝑂𝑙𝑑𝑅) + 0.686(𝑂𝑙𝑑𝐺) + 0.168𝑥(𝑂𝑙𝑑𝐵)
    //𝑁𝑒𝑤𝐵 = 0.272(𝑂𝑙𝑑𝑅) + 0.534(𝑂𝑙𝑑𝐺) + 0.131𝑥(𝑂𝑙𝑑𝐵)
    
    PPMImage* image_out = new_ppmimage(w, h, max);
    
    for(int r = 0; r < h; r++){
        for(int c = 0; c < w; c++){
            float oldR = (float)image_in -> pixmap[0][r][c];
            float oldG = (float)image_in -> pixmap[1][r][c];
            float oldB = (float)image_in -> pixmap[2][r][c];
            
            float newR, newG, newB;
            newR = 0.393 * oldR + 0.769 * oldG + 0.189 * oldB;
            newG = 0.349 * oldR + 0.686 * oldG + 0.168 * oldB;
            newB = 0.272 * oldR + 0.534 * oldG + 0.131 * oldB;
            
            if(newR > max){
                image_out -> pixmap[0][r][c] = max;
            } else {
                image_out -> pixmap[0][r][c] = newR;
            }
            if(newG > max){
                image_out -> pixmap[1][r][c] = max;
            } else {
                image_out -> pixmap[1][r][c] = newG;
            }
            if(newB > max){
                image_out -> pixmap[2][r][c] = max;
            } else {
                image_out -> pixmap[2][r][c] = newB;
            }
            
//            image_out -> pixmap[0][r][c] = newR;
//            image_out -> pixmap[1][r][c] = newG;
//            image_out -> pixmap[2][r][c] = newB;
//
        }
    }
    
    write_ppmfile(image_out, op.outfile_name);
        
    del_ppmimage(image_in);
    del_ppmimage(image_out);
    
}

void option_m(Option op, PPMImage* image_in){
    int h = image_in -> height;
    int w = image_in -> width;
    int max = image_in -> max;

    
    
    for(int color = 0; color < 3; color++){
        for (int r = 0; r < h; r++){
            for(int c = 0; c < w/2; c++){
                
                image_in -> pixmap[color][r][w - 1 - c] = image_in -> pixmap[color][r][c];
                
            }
        }
    }

    write_ppmfile(image_in, op.outfile_name);
    del_ppmimage(image_in);
    
    
}

void option_t(Option op, PPMImage* image_in){
//reduce the input image to a thumbnail based on the given scaling factor [1-8].
    int h = image_in -> height;
    int w = image_in -> width;
    int max = image_in -> max;
    int scale = op.arg;
    int new_h, new_w;
    
    //size for output image
    if(h % scale == 0){
        new_h = h / scale;
    } else {
        new_h = h / scale + 1;
    }
    if(w % scale == 0){
        new_w = w / scale;
    } else {
        new_w = w / scale + 1;
    }
    
    PPMImage* image_out = new_ppmimage(new_w, new_h, max);
    
    for(int r = 0; r < new_h; r++){
        for(int c = 0; c < new_w; c++){
            image_out -> pixmap[0][r][c] = image_in -> pixmap[0][r * scale][c * scale];
            image_out -> pixmap[1][r][c] = image_in -> pixmap[1][r * scale][c * scale];
            image_out -> pixmap[2][r][c] = image_in -> pixmap[2][r * scale][c * scale];
        }
    }
    
    write_ppmfile(image_out, op.outfile_name);
    del_ppmimage(image_in);
    del_ppmimage(image_out);
}

void option_n(Option op, PPMImage* image_in){
    int h = image_in -> height;
    int w = image_in -> width;
    int max = image_in -> max;
    int scale = op.arg;
    int new_h, new_w;
    
    //size of the thumbnail
    if(h % scale == 0){
        new_h = h / scale;
    } else {
        new_h = h / scale + 1;
    }
    if(w % scale == 0){
        new_w = w / scale;
    } else {
        new_w = w / scale + 1;
    }
    
    PPMImage* thumbnail = new_ppmimage(new_w, new_h, max);
    PPMImage* image_out = new_ppmimage(w, h, max);
    
    //the first thumail on the up left
    for(int r = 0; r < new_h; r++){
        for(int c = 0; c < new_w; c++){
            thumbnail -> pixmap[0][r][c] = image_in -> pixmap[0][r * scale][c * scale];
            thumbnail -> pixmap[1][r][c] = image_in -> pixmap[1][r * scale][c * scale];
            thumbnail -> pixmap[2][r][c] = image_in -> pixmap[2][r * scale][c * scale];
        }
    }

    
    for(int i = 0; i < scale; i++){    // i for the rows' scaling
        for(int j = 0; j < scale; j++){  // j for the columns' scaling
            
            for(int r = 0; r < new_h; r++){
                if((r + i * new_h) >= h)
                    break;
                
                
                for(int c = 0; c < new_w; c++){
                    if ((c + j * new_w) >= w)
                        break;
                    
                    image_out -> pixmap[0][r + i * new_h][c + j * new_w] = thumbnail -> pixmap[0][r][c];
                    image_out -> pixmap[1][r + i * new_h][c + j * new_w] = thumbnail -> pixmap[1][r][c];
                    image_out -> pixmap[2][r + i * new_h][c + j * new_w] = thumbnail -> pixmap[2][r][c];
                }
            }
        }
    }
    
    
    
    
    
    
    write_ppmfile(image_out, op.outfile_name);
    
    del_ppmimage(image_out);
    del_ppmimage(thumbnail);
    del_ppmimage(image_in);
    
}
