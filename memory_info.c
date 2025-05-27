#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "memory_info.h"

//Displaying Memory Info (How much memory is used)

//First read file and gets memory info and then returns the amount of memory used
double get_total_memory(){
    FILE *f = fopen("/proc/meminfo", "r");
    if(f == NULL){
        perror("Error opening file");
        return 1;
    }

    char label[256];

    unsigned long memory_total_kb;

    fscanf(f, "%s %lu", label, &memory_total_kb);

    fclose(f);

    double memory_total_gb = memory_total_kb / (1024 * 1024);

    return memory_total_gb;
}

double get_memory_used() {
    FILE *f = fopen("/proc/meminfo", "r");
    if (f == NULL) {
        perror("Error opening file");
        return -1;
    }

    unsigned long memory_total_kb = 0;
    unsigned long memory_free_kb = 0;
    unsigned long memory_available_kb = 0;

    fscanf(f, "MemTotal: %lu kB\nMemFree: %lu kB\nMemAvailable: %lu kB", &memory_total_kb, &memory_free_kb, &memory_available_kb);

    fclose(f);

    double memory_total_gb = memory_total_kb / (1024.0 * 1024.0);
    // double memory_free_gb = memory_free_kb / (1024.0 * 1024.0);
    double memory_available_gb = memory_available_kb / (1024.0 * 1024.0);

    return memory_total_gb - memory_available_gb;
}

//Then plot the points on the graph
//Given the used memory
//does not return anything, prints the point in it's relative position
void plotting_mem(long sample_size, int *values_mem){
    for(int line = 10; line > 0; line--){
        if (line == 10) {
            printf("%6.2f GB |", get_total_memory());
        } else {
            printf("          |");
        }
        for(int pos = 0; pos < sample_size; pos++){
            if(line == values_mem[pos]){
                printf("#");
            }
            else if (values_mem[pos] == -1) {
                printf(" ");
            }
            else{
                printf(" ");
            }
        }
        printf("\n");
    }
}