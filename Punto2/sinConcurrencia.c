#include <stdio.h>
#include <pthread.h>
#include <sqlite3.h>

#define NUM_TRANSACCIONES 10
#define MONTO_POR_TRANSACCION 300

sqlite3 *db;

void *realizar_transacciones(void *arg) {
        int saldo;
        sqlite3_stmt *saldo_stmt;
        const char *saldo_query = "SELECT saldo_disponible FROM saldo";
        sqlite3_prepare_v2(db, saldo_query, -1, &saldo_stmt, NULL);
        sqlite3_step(saldo_stmt);
        saldo = sqlite3_column_int(saldo_stmt, 0);
        sqlite3_finalize(saldo_stmt);

        if (saldo >= MONTO_POR_TRANSACCION) {
            char *query = "UPDATE saldo SET saldo_disponible = saldo_disponible - ?";
            sqlite3_stmt *stmt;
            sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
            sqlite3_bind_int(stmt, 1, MONTO_POR_TRANSACCION);
            int rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                fprintf(stderr, "Error al ejecutar la transacción: %s\n", sqlite3_errmsg(db));
            }
            sqlite3_finalize(stmt);
            printf("Transacción realizada. Nuevo saldo: %d\n", saldo - MONTO_POR_TRANSACCION);
        } else {
            printf("Transacción rechazada. Saldo insuficiente.\n");
        }
        
    	return NULL;
}

int main() {
    int rc = sqlite3_open("banco.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al abrir la base de datos: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    pthread_t threads[NUM_TRANSACCIONES];
    for (int i = 0; i < NUM_TRANSACCIONES; i++) {
        pthread_create(&threads[i], NULL, realizar_transacciones, NULL);
    }

    for (int i = 0; i < NUM_TRANSACCIONES; i++) {
        pthread_join(threads[i], NULL);
    }

    sqlite3_close(db);
    return 0;
}
