#ifndef CPU_INFO_H
#define CPU_INFO_H

//Create a struct to save sample and usage info
typedef struct {
    double prev_total;
    double prev_idle;
    double current_total;
    double current_idle;
} cpu_samples;

long get_total_cpu();
long get_idle_cpu();
long calculate_cpu_usage(cpu_samples position);
double calculate_cpu_usage_precise(cpu_samples position);
void plotting_cpu(long sample_size, int *values_cpu);
long get_cpu_cores();
double get_max_frequency();
void display_cores(long no_cores, double max_freq);

#endif