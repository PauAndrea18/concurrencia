# Taller de Concurrencia

Este taller tiene como objetivo comprender y comparar diferentes métodos de concurrencia en la programación, así como implementar una aplicación que maneje concurrencia utilizando una base de datos y observar la diferencia con una aplicación sin concurrencia. 

## Punto 1: Comparación de Métodos de Concurrencia

Ejecutaremos y compararemos los siguientes programas:

- `semaforo.c`
- `mutex.c`
- `monitor.c`
- `RR.c`

Para cada uno de estos programas, se medirán los siguientes parámetros:

1. **Tiempo de ejecución** (medido en segundos)
2. **Procesamiento (tiempo de CPU)** (medido en segundos)
3. **% de uso de memoria**

Los resultados se mostrarán en tablas como se muestra a continuación:

![Tabla de Datos](images/Tabla_datos.PNG)

Y se graficarán para una mejor comparación:

![Gráfica de Comparación](images/Grafica_comparacion.PNG)

### Ejecución de los Programas

1. Compila los programas:
    ```sh
    gcc semaforo.c -o semaforo -lpthread
    gcc mutex.c -o mutex -lpthread
    gcc monitor.c -o monitor -lpthread
    gcc RR.c -o RR -lpthread
    ```

2. Ejecuta los programas y mide los tiempos y uso de memoria:
    ```sh
    /usr/bin/time -v ./semaforo
    /usr/bin/time -v ./mutex
    /usr/bin/time -v ./monitor
    /usr/bin/time -v ./RR
    ```

3. Registra los datos obtenidos y compáralos en tablas y gráficas.

## Punto 2: Implementación de una Aplicación Bancaria con Concurrencia

Se programará una aplicación en C utilizando SQLite para simular una operación bancaria. La base de d
