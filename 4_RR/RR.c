#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
} Process;

void calculate_times(Process processes[], int n, int quantum) {
    int current_time = 0;
    int completed = 0;

    while (completed != n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].remaining_time <= quantum) {
                    current_time += processes[i].remaining_time;
                    processes[i].remaining_time = 0;
                    processes[i].completion_time = current_time;
                    processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                    completed++;
                } else {
                    current_time += quantum;
                    processes[i].remaining_time -= quantum;
                }
            }
        }
    }
}

void print_processes(Process processes[], int n) {
    printf("PID\tTiempo de llegada\tTiempo de Rafaga\tTiempo de Competicion\tTurnaround Time\tTiempo de Espera\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].completion_time,
               processes[i].turnaround_time,
               processes[i].waiting_time);
    }
}

int main() {
    int n, quantum;
    struct timespec start, end;
    struct rusage usage;
    double elapsed_time;

    FILE *file = fopen("mediciones.txt", "w");
    
    if (!file) {
        perror("Error al abrir el archivo");
        return 1;
    }

    printf("Introduzca el número de procesos: ");
    scanf("%d", &n);
    
    Process *processes = (Process *)malloc(n * sizeof(Process));

    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        printf("Introduzca la hora de llegada del proceso %d: ", i + 1);
        scanf("%d", &processes[i].arrival_time);
        printf("Introduzca el tiempo de ráfaga para el proceso %d: ", i + 1);
        scanf("%d", &processes[i].burst_time);
        processes[i].remaining_time = processes[i].burst_time;
    }

    printf("Introduzca el cuanto de tiempo: ");
    scanf("%d", &quantum);

    clock_gettime(CLOCK_MONOTONIC, &start); // Medir el tiempo de inicio

    calculate_times(processes, n, quantum);
    print_processes(processes, n);

    clock_gettime(CLOCK_MONOTONIC, &end); // Medir el tiempo de finalización

    // Calcular el tiempo de ejecución
    elapsed_time = (end.tv_sec - start.tv_sec) + 
                   (end.tv_nsec - start.tv_nsec) / 1e9;
    fprintf(file, "Tiempo de ejecución: %.6f segundos\n", elapsed_time);

    // Obtener el uso del CPU
    getrusage(RUSAGE_SELF, &usage);
    double cpu_user_time = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6;
    double cpu_system_time = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
    fprintf(file, "Tiempo de CPU (usuario): %.6f segundos\n", cpu_user_time);
    fprintf(file, "Tiempo de CPU (sistema): %.6f segundos\n", cpu_system_time);

    // Obtener el uso de memoria
    FILE* statm = fopen("/proc/self/statm", "r");
    if (statm) {
        long size, resident, share, text, lib, data, dt;
        fscanf(statm, "%ld %ld %ld %ld %ld %ld %ld", &size, &resident, &share, &text, &lib, &data, &dt);
        fclose(statm);

        long page_size = sysconf(_SC_PAGESIZE); // Tamaño de página en bytes
        double resident_memory_kb = resident * page_size / 1024.0; // Convertir a KB

        // Leer la memoria total del sistema desde /proc/meminfo
        FILE* meminfo = fopen("/proc/meminfo", "r");
        if (meminfo) {
            char line[256];
            long total_memory_kb = 0;
            while (fgets(line, sizeof(line), meminfo)) {
                if (sscanf(line, "MemTotal: %ld kB", &total_memory_kb) == 1) {
                    break;
                }
            }
            fclose(meminfo);

            if (total_memory_kb > 0) {
                double memory_usage_percentage = (resident_memory_kb / total_memory_kb) * 100.0;
                fprintf(file, "Uso de memoria residente: %.2f%%\n", memory_usage_percentage);
            } else {
                fprintf(file, "No se pudo obtener la memoria total del sistema.\n");
            }
        } else {
            perror("No se pudo abrir /proc/meminfo");
        }
    } else {
        perror("No se pudo abrir /proc/self/statm");
    }

    fclose(file); // Cierra el archivo

    free(processes);

    return 0;
}

