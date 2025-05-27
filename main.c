#include "memory_info.h"
#include "cpu_info.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

static void catch_function(int signo){
    char response[256];

    printf("\nDo you want to quit?(y/n): ");
    fflush(stdout);

    if(fgets(response, sizeof(response), stdin) != NULL){
        if(strncmp(response, "y", 1) == 0 || strncmp(response, "Y", 1) == 0){
            kill(0, SIGKILL);
        }
        else{
            return;
        }
    }
}

static void ignore_function(int signo){
    signal(signo, SIG_IGN);
}

int main(int argc, char *argv[]){
    //Check for ctrl+C
    if (signal(SIGINT, catch_function) == SIG_ERR) {
        fputs("An␣error␣occurred␣while␣setting␣a␣signal␣handler. \n ", stderr ) ;
        return EXIT_FAILURE;
    }

    //Check for ctrl+Z
    if (signal(SIGTSTP, ignore_function) == SIG_ERR) {
        fputs("An␣error␣occurred␣while␣setting␣a␣signal␣handler. \n ", stderr ) ;
        return EXIT_FAILURE;
    }

    int sample_size = 20;
    long delay = 500000;

    //Initializations for cpu
    cpu_samples user;
    user.prev_total = 0.0;
    user.prev_idle = 0.0;
    user.current_total = 0.0;
    user.current_idle = 0.0;

    //Flags
    int displayCPU = 0;
    int displayMEM = 0;
    int displayCORES = 0;
    int no_arg_given = 1;

    int positional_count = 0;
    //Parsing through commands given
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "--memory") == 0){
            displayMEM = 1;
            no_arg_given = 0;
        }
        else if(strcmp(argv[i], "--cpu") == 0){
            displayCPU = 1;
            no_arg_given = 0;
        }
        else if(strcmp(argv[i], "--cores") == 0){
            displayCORES = 1;
            no_arg_given = 0;
        }
        else if(strncmp(argv[i], "--samples=", 10) == 0){
            sample_size = atoi(argv[i] + 10);
        }
        else if(strncmp(argv[i], "--tdelay=", 9) == 0){
            delay = atol(argv[i] + 9);
        }
        else{
            if(i == 1){
                sample_size = atoi(argv[i]);
            }
            else if(i == 2){
                delay = atol(argv[i]);
            }
            positional_count++;
        }
    }

    //If no command is given
    if (no_arg_given) {
        displayMEM = 1;
        displayCPU = 1;
        displayCORES = 1;
    }

    //Initializations for memory
    int *values_mem = malloc(sample_size * sizeof(int));
    int *values_cpu = malloc(sample_size * sizeof(int));
    double *values_cpu2 = malloc(sample_size * sizeof(double));
    double total_memory = get_total_memory();

    if(displayMEM || displayCPU){
        //Displays the graphs
        for(int sample = 0; sample < sample_size; sample++){
            //Initialize to collect two pid values
            pid_t pid_m = -1;
            pid_t pid_c = -1;

            //Initialize pipes
            int pipe_m[2];
            if(pipe(pipe_m) == -1){
                perror("pipe_m");
                exit(1);
            }

            int pipe_c[2];
            if(pipe(pipe_c) == -1){
                perror("pipe_c");
                exit(1);
            }

            //Child info collection
            //memory
            pid_m = fork();
            if(pid_m < 0){
                perror("forking mem");
                exit(1);
            }
            if(pid_m == 0){
                double memory_used = get_memory_used();
                int position_mem = (int) ((memory_used / total_memory) * 10);
                //values_mem[sample] = position_mem;

                close(pipe_m[0]);
                write(pipe_m[1], &position_mem, sizeof(position_mem));
                close(pipe_m[1]);
                exit(0);
            }

            //cpu
            pid_c = fork();
            if(pid_c < 0){
                perror("forking cpu");
                exit(1);
            }
            if(pid_c == 0) {
                cpu_samples temp = user;

                temp.prev_total = temp.current_total;
                temp.prev_idle = temp.current_idle;
                temp.current_total = get_total_cpu();
                temp.current_idle = get_idle_cpu();

                close(pipe_c[0]);
                write(pipe_c[1], &temp, sizeof(temp));
                close(pipe_c[1]);
                exit(0);
                // int position_cpu = calculate_cpu_usage(user);
                // int position_cpu2 = calculate_cpu_usage_precise(user);
                // values_cpu[sample] = position_cpu;
                // values_cpu2[sample] = position_cpu2;
            }

            //Parent info processing
            
            //memory
            int status_mem;
            waitpid(pid_m, &status_mem, 0);
            int new_position_mem;
            close(pipe_m[1]);
            read(pipe_m[0], &new_position_mem, sizeof(new_position_mem));
            values_mem[sample] = new_position_mem;
            close(pipe_m[0]);

            //cpu
            int status_cpu;
            waitpid(pid_c, &status_cpu, 0);
            cpu_samples new_cpu;
            close(pipe_c[1]);
            read(pipe_c[0], &new_cpu, sizeof(new_cpu));
            int position_cpu = calculate_cpu_usage(new_cpu);
            int position_cpu2 = calculate_cpu_usage_precise(new_cpu);
            values_cpu[sample] = position_cpu;
            values_cpu2[sample] = position_cpu2;
            user = new_cpu; //have to set original cpu_sample to the new one that got calculated
            close(pipe_c[0]);

            printf("\033[2J\033[H");
            fflush(stdout);
            printf("Nbr of samples: %d -- every %ld microsecs\n\n", sample_size, delay);

            //If they only ask for --memory
            if(displayMEM){
                //Plots the memory graph
                printf("v Memory %.2lf GB", get_memory_used());
                printf("\n");
                plotting_mem(sample_size, values_mem);
                //Creates x-axis for memory graph
                printf("%6.2f GB  ", 0.0);
                for(int i = 0; i < sample_size; i++){
                    printf("—");
                }
                printf("\n\n");   
            }

            //If they only ask for --cpu
            if(displayCPU){
                //Plots the cpu graph
                printf("v CPU %.2lf%%", values_cpu2[sample]);
                printf("\n");
                plotting_cpu(sample_size, values_cpu);
                //Creates x-axis for cpu graph
                printf("%8d%%  ", 0);
                for(int i = 0; i < sample_size; i++){
                    printf("—");
                }
                printf("\n\n");
            }

            usleep(delay);
        }
    }

    //If they ask for --cores OR they don't specify
    if(displayCORES){
        pid_t pid_co = -1;
        pid_t pid_f = -1;

        //Initialize pipes
        int pipe_co[2];
        if(pipe(pipe_co) == -1){
            perror("pipe_co");
            exit(1);
        }

        int pipe_f[2];
        if(pipe(pipe_f) == -1){
            perror("pipe_f");
            exit(1);
        }

        pid_co = fork();
        if(pid_co < 0){
            perror("forking cores");
            exit(1);
        }
        if(pid_co == 0){
            long cores = get_cpu_cores();

            close(pipe_co[0]);
            write(pipe_co[1], &cores, sizeof(cores));
            close(pipe_co[1]);
            exit(0);
        }

        pid_f = fork();
        if(pid_f < 0){
            perror("forking frequency");
            exit(1);
        }
        if(pid_f == 0){
            double freq = get_max_frequency();

            close(pipe_f[0]);
            write(pipe_f[1], &freq, sizeof(freq));
            close(pipe_f[1]);
            exit(0);
        }

        int status_cores;
        waitpid(pid_co, &status_cores, 0);
        int no_cores;
        close(pipe_co[1]);
        read(pipe_co[0], &no_cores, sizeof(no_cores));
        close(pipe_co[0]);

        int status_freq;
        waitpid(pid_f, &status_freq, 0);
        double max_freq;
        close(pipe_f[1]);
        read(pipe_f[0], &max_freq, sizeof(max_freq));
        close(pipe_f[0]);
        
        //Displays the number of cores
        display_cores(no_cores, max_freq);
    }

    free(values_mem);
    free(values_cpu);
    free(values_cpu2);
}