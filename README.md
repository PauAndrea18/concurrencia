# Taller de Concurrencia

Este taller tiene como objetivo comprender y comparar diferentes métodos de concurrencia en la programación, así como implementar una aplicación que maneje concurrencia utilizando una base de datos SQLite. 

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

Los resultados se mostrarán en tablas (como en la imagen `Tabla datos.PNG`) y se graficarán para una mejor comparación (como en la imagen `Grafica comparacion.PNG`).

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

Se programará una aplicación en C utilizando SQLite para simular una operación bancaria. La base de datos contendrá una tabla que representa el saldo de una persona (ver imagen `Base de datos.jpg`). Para evitar accesos concurrentes que puedan causar inconsistencias, se utilizará un mecanismo de concurrencia (en este caso, un mutex).

### Requisitos

1. **Base de Datos:** SQLite.
2. **Problema a Resolver:** Evitar accesos concurrentes no controlados a la actualización del saldo al realizar transacciones.
3. **Herramienta de Concurrencia:** Mutex.

### Ejecución del Programa sin Concurrencia

El programa `sinConcurrencia.c` muestra el problema de acceder al saldo desde múltiples hilos sin control de concurrencia, lo que puede llevar a saldos negativos (ver imagen `Salida programa sinConcurrencia.jpg`).

### Implementación del Programa con Concurrencia

Se implementará un programa utilizando mutex para garantizar la exclusión mutua y así evitar inconsistencias en el saldo de la cuenta.

### Explicación del Mutex

Los mutex (abreviatura de mutual exclusion) son objetos de sincronización que permiten a los hilos coordinar el acceso a recursos compartidos de manera segura, evitando condiciones de carrera y garantizando la consistencia de los datos. Cuando un hilo adquiere un mutex, bloquea el acceso a otros hilos hasta que lo libera, asegurando que solo un hilo pueda ejecutar una sección crítica del código a la vez.

En este escenario, estamos accediendo a una base de datos compartida desde múltiples hilos, y queremos asegurarnos de que solo un hilo pueda realizar una transacción a la vez para evitar problemas de consistencia en los datos. El uso de un mutex nos permite lograr esto de manera bastante directa y comprensible.

Otras opciones, como semáforos o barreras, podrían haber sido utilizadas también para lograr la misma funcionalidad. Sin embargo, en este caso, un mutex parece ser la opción más simple y apropiada dada la naturaleza de la operación de acceso a la base de datos.

### Ejecución del Programa con Concurrencia

El programa con concurrencia (`conConcurrencia.c`) evita que el saldo llegue a ser negativo (ver imagen `Salida programa conConcurrencia.jpg`), demostrando así la eficacia de la implementación de mutex en este contexto.

## Conclusión

En este taller se ha demostrado la importancia de utilizar mecanismos de concurrencia para garantizar la integridad de los datos en aplicaciones que acceden a recursos compartidos. La comparación de diferentes métodos de concurrencia y la implementación de una solución práctica utilizando mutex han permitido comprender mejor cómo prevenir problemas como las condiciones de carrera y asegurar la consistencia de los datos.