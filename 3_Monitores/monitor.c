#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>

// Monitor
typedef struct {
    int shared_variable;
    pthread_mutex_t mtx;
    pthread_cond_t cv;
    int ready;
} Monitor;

void monitor_init(Monitor *monitor) {
    monitor->shared_variable = 0;
    pthread_mutex_init(&monitor->mtx, NULL);
    pthread_cond_init(&monitor->cv, NULL);
    monitor->ready = 0;
}

void monitor_destroy(Monitor *monitor) {
    pthread_mutex_destroy(&monitor->mtx);
    pthread_cond_destroy(&monitor->cv);
}

void monitor_increment(Monitor *monitor) {
    pthread_mutex_lock(&monitor->mtx);
    while (!monitor->ready) {
        pthread_cond_wait(&monitor->cv, &monitor->mtx);
    }
    for (int i = 0; i < 1000000; ++i) {
        monitor->shared_variable++;
    }
    monitor->ready = 0;
    pthread_cond_signal(&monitor->cv);
    pthread_mutex_unlock(&monitor->mtx);
}

void monitor_set_ready(Monitor *monitor) {
    pthread_mutex_lock(&monitor->mtx);
    monitor->ready = 1;
    pthread_cond_signal(&monitor->cv);
    pthread_mutex_unlock(&monitor->mtx);
}

int monitor_get_shared_variable(Monitor *monitor) {
    pthread_mutex_lock(&monitor->mtx);
    int value = monitor->shared_variable;
    pthread_mutex_unlock(&monitor->mtx);
    return value;
}

void* thread_function(void* arg) {
    Monitor *monitor = (Monitor *)arg;
    monitor_set_ready(monitor);
    monitor_increment(monitor);
    return NULL;
}

int main() {
    Monitor monitor;
    monitor_init(&monitor);
    struct timespec start, end;
    struct rusage usage;
    double elapsed_time;
    
    FILE *file = fopen("mediciones.txt", "w");
    
    if (!file) {
        perror("Error al abrir el archivo");
        return 1;
    }

    pthread_t thread1, thread2;
    
     clock_gettime(CLOCK_MONOTONIC, &start); // Medir el tiempo de inicio

    pthread_create(&thread1, NULL, thread_function, &monitor);
    pthread_create(&thread2, NULL, thread_function, &monitor);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    clock_gettime(CLOCK_MONOTONIC, &end); // Medir el tiempo de finalización


    fprintf(file, "Valor final: %d\n", monitor_get_shared_variable(&monitor));

    monitor_destroy(&monitor);
    
     fprintf(file, "Tiempo de ejecución: %.6f segundos\n", elapsed_time);

    // Obtener el uso del CPU
    getrusage(RUSAGE_SELF, &usage);
    double cpu_system_time = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
    fprintf(file, "Tiempo de CPU: %.6f segundos\n", cpu_system_time);

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



    return 0;
}
