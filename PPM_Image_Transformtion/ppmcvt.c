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

void option_b(Option op, PPMImage* image_in); //convert input file to a Portable Bitmap (PBM) file
void option_g(Option op, PPMImage* image_in);//convert input file to a Portable Gray Map (PGM) file using the specified max grayscale pixel value [1-65535].
void option_i(Option op, PPMImage* image_in);//isolate the specified RGB channel. Valid channels are “red”, “green”, or “blue”.
void option_r(Option op, PPMImage* image_in);//remove the specified RGB channel. Valid channels are “red”, “green”, or “blue”.
void option_s();
void option_m();
void option_t();
void opiton_n();
void option_o();


Option read_options(int arc, char*argv[]);//return the option, and handle the exceptions, assign file names;

void image_transform (Option op, PPMImage image_in);


int main( int argc, char *argv[] )
{
    Option option = read_options(argc, argv);
    
    printf("infilename in main:%s\n", option.infile_name);
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
            
            break;
        case 'm':
            
            break;
        case 't':
           
            break;
        case 'n':
            
            break;
    }
    
    
    
    
    
    return 0;
}



Option read_options(int argc, char*argv[]){
    int o_find = 0;
    int op_found = 0; //only accept one option except for -o?
    Option option;
    
    int o;
    
    while( (o=getopt(argc,argv,"bg:i:r:smt:n:o")) != -1){
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
                }
                break;
                break;
                
            case 's':
                
                break;
                
            case 'm':
                
                break;
                
            case 't':
               
                break;
                
            case 'n':
                
                break;
                
            case 'o': // assign the output file.
                o_find = 1;
                
                
                if (!op_found)      // set default option b
                    option.mode = 'b';
                if ((optind + 2) > argc){
                    fprintf(stderr, "Output file and Input file required\n");
                    exit(1);
                }
                option.outfile_name = argv[optind];       //not necessarily follow o.
                option.infile_name = argv[optind + 1];
                printf("outfilename:%s\n", argv[optind]);
                printf("inputfilename:%s\n", argv[optind+1]);
                break;
            default:
                printf("Usage: ppmcvt [-bgirsmtno] [FILE]\n");
                break;
        }
    }
    
    if (o_find == 0) {
        printf("Expected an output opiton -o\n");
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
    
    PGMImage* image_out = new_pgmimage(image_in -> width, image_in -> height, image_in -> max);
    
    //To compute grayscale pixels from RGB pixels use: (𝐴𝑣𝑒𝑟𝑎𝑔𝑒(𝑅 + 𝐺 + 𝐵)/𝑃𝑃𝑀𝑀𝑎𝑥) × 𝑃𝐺𝑀𝑀𝑎𝑥
    float PPMMax = (float)max;
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++){
            float RGB_sum = 0;
            for (int color = 0; color < 3; color++){
                RGB_sum = RGB_sum + (float)image_in -> pixmap[color][r][c];
            }
            
            float average_RGB = RGB_sum / 3;
            image_out -> pixmap[r][c] = (int)((average_RGB / PPMMax) * PGMMax);
        
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
