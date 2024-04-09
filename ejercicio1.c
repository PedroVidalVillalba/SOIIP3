/**
 * Sistemas Operativos 2.
 * Práctica 3. Sincronización de procesos con mutexes
 *
 * @date 09/04/2024
 * @authors Arcos Salgado, Guillermo
 * @authors Vidal Villalba, Pedro
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include "shared_stack.h"
#include "producer_consumer.h"
#include "array_sum.h"


#define P 5             /* Número de hilos productores */
#define C 4             /* Número de hilos consumidores */
#define N 12            /* Tamaño del buffer */
#define NUM_ITEMS 18    /* Número de items a generar por cada productor */


/* Estructura para almacenar los parámetros que hay que pasar a cada hilo */
struct thread_args {
    Stack* stack;
    Array* array;
};

/**
 * @brief Función a ejecutar por cada hilo productor.
 *
 * @details
 * Cada hilo productor producirá un item en un stack compartido y, una vez hecho eso,
 * intentará contribuir a la suma de los valores en posiciones pares de un array.
 *
 * @param args  Puntero a una estructura con el Stack y Array a utilizar por el hilo.
 *
 * @return NULL
 */
void* producer(void* args);

/**
 * @brief Función a ejecutar por cada hilo consumidor.
 *
 * @details
 * Cada hilo consumidor producirá un item en un stack compartido y, una vez hecho eso,
 * intentará contribuir a la suma de los valores en posiciones impares de un array.
 *
 * @param args  Puntero a una estructura con el Stack y Array a utilizar por el hilo.
 *
 * @return NULL
 */
void* consumer(void* args);

int main(int argc, char** argv) {
    pthread_t producers[P], consumers[C];
    Stack stack;
    Array producer_array, consumer_array;
    struct thread_args producer_args, consumer_args;
    int i;



    /* todo: Iniciar las variables compartidas */
    producer_args.stack = &stack;
    producer_args.array = &producer_array;

    consumer_args.stack = &stack;
    consumer_args.array = &consumer_array;

    for (i = 0; i < P; i++) {
        if (pthread_create(producers + i, NULL, producer, &producer_args)) {
            fprintf(stderr, "ERROR: No se pudo crear el productor %d.\n", i + 1);
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < C; i++) {
        if (pthread_create(consumers + i, NULL, consumer, &consumer_args)) {
            fprintf(stderr, "ERROR: No se pudo crear el consumidor %d.\n", i + 1);
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < P; i++) {
        pthread_join(producers[i], NULL);
    }
    for (i = 0; i < C; i++) {
        pthread_join(consumers[i], NULL);
    }

    /* todo: destruir todas las variables compartidas */

    exit(EXIT_SUCCESS);
}


void* producer(void* args) {
    int i;
    int done;   /* Flag para saber si ya se terminó la suma */
    Stack* stack = ((struct thread_args*) args)->stack;
    Array* array = ((struct thread_args*) args)->array;

    for (i = 0, done = 0; i < NUM_ITEMS) {
        produce(stack);
        if (!done) {
            done = contribute_sum(array);
            if (done) {
                printf(""); /* todo: Imprimir el resultado de la suma */
            }
        }
    }

    pthread_exit(NULL);
}
