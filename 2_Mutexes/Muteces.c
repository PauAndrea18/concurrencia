#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>

int shared_variable = 0; // Variable compartida
pthread_mutex_t mutex; // Declaración del mutex

void* increment(void* arg) {
    for (int i = 0; i < 1000000; ++i) {
        pthread_mutex_lock(&mutex); // Bloquea el mutex
        shared_variable++; // Sección crítica
        pthread_mutex_unlock(&mutex); // Desbloquea el mutex
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    struct timespec start, end;
    struct rusage usage;
    
    FILE *file = fopen("mediciones.txt", "w");
    
    if (!file) {
        perror("Error al abrir el archivo");
        return 1;
    }

    pthread_mutex_init(&mutex, NULL); // Inicializa el mutex
    
     clock_gettime(CLOCK_MONOTONIC, &start); // Medir el tiempo de inicio

    pthread_create(&thread1, NULL, increment, NULL); // Crea el primer hilo
    pthread_create(&thread2, NULL, increment, NULL); // Crea el segundo hilo

    pthread_join(thread1, NULL); // Espera a que el primer hilo termine
    pthread_join(thread2, NULL); // Espera a que el segundo hilo termine
    
    clock_gettime(CLOCK_MONOTONIC, &end); // Medir el tiempo de finalización

    pthread_mutex_destroy(&mutex); // Destruye el mutex
    
    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    fprintf(file,"Valor final: %d\n", shared_variable); // Imprime el valor final
    
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
