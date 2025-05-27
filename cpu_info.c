#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "cpu_info.h"

//First get cpu info
// using /proc/stat

//Get total usage
long get_total_cpu(){
    unsigned long user, nice, system, idle , iowait, irq, softirq, steal, guest, guest_nice;
    unsigned long total_kb;

    FILE *f = fopen("/proc/stat", "r");
    if(f == NULL){
        perror("Error opening file");
        return 1;
    }

    fscanf(f, "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

    fclose(f);

    total_kb = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
    // double total_gb = total_kb / (1024 * 1024);

    return total_kb;
}

//get idle usage
long get_idle_cpu(){
    unsigned long idle_kb;
    unsigned long dummy;

    FILE *f = fopen("/proc/stat", "r");
    if(f == NULL){
        perror("Error opening file");
        return 1;
    }

    fscanf(f, "cpu %lu %lu %lu %lu", &dummy, &dummy, &dummy, &idle_kb);

    fclose(f);

    // double idle_gb = idle_kb / (1024 * 1024);

    return idle_kb;
}

//get the number of cpu cores
long get_cpu_cores(){
    char label[256];
    unsigned long siblings;

    FILE *f = fopen("/proc/cpuinfo", "r");
    if(f == NULL){
        perror("Error opening file");
        return 1;
    }

    while(fscanf(f, "%s", label)){
        if(strcmp(label, "siblings") == 0){
            fscanf(f, "%*s %lu", &siblings);
            break;
        }
    }

    fclose(f);

    return siblings;
}

//get the max cpu frequency
double get_max_frequency(){
    long max_freq_kb;

    FILE *f = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", "r");
    if(f == NULL){
        perror("Error opening file");
        return 1;
    }

    fscanf(f, "%ld", &max_freq_kb);

    fclose(f);

    double max_freq_gb = max_freq_kb / 1000000.0;

    return max_freq_gb;
}

long calculate_cpu_usage(cpu_samples position){
    double delta_total = position.current_total - position.prev_total;
    double delta_idle = position.current_idle - position.prev_idle;
    double cpu_usage = round(((delta_total - delta_idle) / delta_total) * 10);
    //multiplying by 10 since we want it's position on the 10 increment y-axis
    //again we round since there are 10 increments so we can't display decimal positions 
    //(ie. 4.7 needs to be on y-level 5)

    return ((long) cpu_usage);
}

double calculate_cpu_usage_precise(cpu_samples position){
    double delta_total = position.current_total - position.prev_total;
    double delta_idle = position.current_idle - position.prev_idle;

    if (delta_total == 0) {
        return 0.0;
    }

    double cpu_usage = ((delta_total - delta_idle) / delta_total) * 100;
    return cpu_usage;
}

void display_cores(long no_cores, double max_freq){
    long no_columns = sqrt(no_cores);
    if(no_columns < 1){
        no_columns = 1;
    }
    long no_rows = 0;

    //This gets the number of rows needed to hold all the boxes
    while((no_rows * no_columns) < no_cores){
        no_rows++;
    }

    printf("v Number of Cores: %ld @ %.2lf GHz\n", no_cores, max_freq);

    for(int r = 0; r < no_rows; r++){
        for(int c = 0; c < no_columns && ((r * no_columns) + c) < no_cores; c++){
            printf("+---+ ");
        }
        printf("\n");
        for(int c = 0; c < no_columns && ((r * no_columns) + c) < no_cores; c++){
            printf("|   | ");
        }
        printf("\n");
        for(int c = 0; c < no_columns && ((r * no_columns) + c) < no_cores; c++){
            printf("+---+ ");
        }
        printf("\n");
    }
}

//Then plot the points on the graph
//Given the used cpu
//does not return anything, prints the point in it's relative position
void plotting_cpu(long sample_size, int *values_cpu){
    for(int line = 10; line > 0; line--){
        if (line == 10) {
            printf("     %d%% |", 100);
        }
        else {
            printf("          |");
        }
        for(int pos = 0; pos < sample_size; pos++){
            if(line == values_cpu[pos]){
                printf(":");
            }
            else if(values_cpu[pos] == -1){
                printf(" ");
            }
            else{
                printf(" ");
            }
        }
        printf("\n");
    }
}