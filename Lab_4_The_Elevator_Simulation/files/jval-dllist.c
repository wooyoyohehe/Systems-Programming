#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "dllist.h"
#include "jval.h"


typedef struct {
    int rank;
    char * country;
    int population;
} PopulationData;

PopulationData data[] = {
    {1, "China", 1439323776},
    {2, "India", 1380004385},
    {3, "United States", 331002651},
    {4, "Indonesia", 273523615},
    {5, "Pakistan", 220892340},
    {6, "Brazil", 212559417},
    {7, "Nigeria", 206139589},
    {8, "Bangladesh", 164689383},
    {9, "Russia", 145934462},
    {10, "Mexico", 128932753} };

int main()
{
    setlocale(LC_NUMERIC, "");

    //declare an array of Jvals (generic type)
    Jval v[10];
    
    //initialize array as a list of integers
    for( int i=0; i<10; i++){
        v[i] = new_jval_i(i*i);
    }
    //print array integer vals
    printf("JVal Array as ints: \n");
    for( int i=0; i<10; i++){
        printf("v[i]: %d\n", jval_i(v[i]));
    }
    
    //now set array to be a list of strings
    for( int i=0; i<10; i++){
        v[i] = new_jval_s( data[i].country );
    }
    //print array of strings
    printf("\nJVal Array as strings: \n");
    for( int i=0; i<10; i++){
        printf("v[i]: %s\n", jval_s(v[i]));
    }
    
    Dllist countries = new_dllist();
    Dllist curnode;
    printf("\nCreating a Dllist of Population Data ... \n");
    for (int i = 0; i < 10; i++) {
        printf("\t Inserting rank: %d, country: %s, population: %'d\n", data[i].rank, data[i].country, data[i].population);
        dll_append(countries, new_jval_v(&(data[i])));
    }
    
    printf("\nTraversing and printing Dllist of Population Data ... \n");
    dll_traverse( curnode, countries ){
        PopulationData * curpd;
        curpd = (PopulationData *)jval_v( dll_val(curnode));
        
        printf("\trank: %d, country: %s, population: %'d\n", curpd->rank, curpd->country, curpd->population);
    }
    
    printf("\nTraversing and printing Dllist of Population Data in reverse ... \n");
    dll_rtraverse( curnode, countries ){
        PopulationData * curpd;
        curpd = (PopulationData *)jval_v( dll_val(curnode));
        
        printf("\trank: %d, country: %s, population: %'d\n", curpd->rank, curpd->country, curpd->population);
    }
    
    printf("\nDeleting every other node ... \n");
    int i=0;
    Dllist tmpnode;
    dll_traverse( curnode, countries ){
        if( i%2 ) {
            PopulationData * curpd;
            curpd = (PopulationData *)jval_v( dll_val(curnode));
            printf("\t deleting rank: %d, country: %s, population: %'d\n", curpd->rank, curpd->country, curpd->population);

            tmpnode = curnode->blink;
            dll_delete_node(curnode);
            curnode = tmpnode;
        }
        i++;
    }

    printf("\nTraversing and printing Dllist of Population Data ... \n");
    dll_traverse( curnode, countries ){
        PopulationData * curpd;
        curpd = (PopulationData *)jval_v( dll_val(curnode));
        
        printf("\trank: %d, country: %s, population: %'d\n", curpd->rank, curpd->country, curpd->population);
    }
}
