#include "pbm.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

void option_b();
void option_g();
void option_i();
void option_r();
void option_s();
void option_m();
void option_t();
void opiton_n();
void option_o();

typedef struct
{
    char mode;
    char* arg;
    char* infile_name;
    char* outfile_name;
    
}Option;

Option read_options(int arc, char*argv[]);    //return the option, and handle the exceptions, assign file names;



int main( int argc, char *argv[] )
{
    Option option = read_options(argc, argv);
    
    
    return 0;
}

Option read_options(int argc, char*argv[]){
    int o_find = 0;
    int op_found = 0; //only accept one option except for -o?
    Option option;
    int o;
    while( (o=getopt(argc,argv,"bg:i:r:smt:n:o")) != -1){
//        if(option_found){
//            printf("too many options\n");
//            exit();
//        }
        switch (o) {
            case 'b':
                option.mode = 'b';
                op_found = 1;
                break;
            case 'g':
               
                break;
            case 'i':
                
                break;
            case 'r':
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
                if ((optind + 2) >= argc){
                    printf("Output file and Input file required\n");
                    exit(0);
                }
                option.outfile_name = argv[optind];
                option.infile_name = argv[optind + 1];
                break;
            default:
                printf("unexpected arg");
                break;
        }
    }
    
    if (o_find == 0) {
        printf("Expected an output opiton -o\n");
        exit(0);
    }
    
    return option;
}
