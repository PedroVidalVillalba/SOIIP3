/**
 * Sistemas Operativos 2.
 * Práctica 3. Sincronización de procesos con mutexes.
 * Ejercicio optativo: resolución sin variables de condición.
 *
 * @date 21/04/2024
 * @authors Arcos Salgado, Guillermo
 * @authors Vidal Villalba, Pedro
 */

#define _GNU_SOURCE
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

#ifdef SLEEP
#define usage(exe_name) fprintf(stderr, "ERROR: Número de parámetros incorrecto\n"  \
                                        "Modo de ejecución: %s <production> <insertion> <contribution_p> <extraction> <consumption> <contribution_c>\n\n", exe_name)
/* Variable global con los tiempos de cada sleep */
struct {
    int production;
    int insertion;
    int contribution_p;
    int extraction;
    int consumption;
    int contribution_c;
} sleep_times;
#endif //SLEEP

int main(int argc, char** argv) {
    pthread_t producers[P], consumers[C];
    Stack stack;
    Array producer_array, consumer_array;
    struct thread_args producer_args, consumer_args;
    int i;

#ifdef SLEEP
    /* Procesado de argumentos */
    if (argc != 7) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    sleep_times = (typeof(sleep_times)) {
            .production      = atoi(argv[1]),
            .insertion       = atoi(argv[2]),
            .contribution_p  = atoi(argv[3]),
            .extraction      = atoi(argv[4]),
            .consumption     = atoi(argv[5]),
            .contribution_c  = atoi(argv[6]),
    };
#endif //SLEEP

    /* Inicializar las variables compartidas */
    create_stack(&stack, N);
    /* Crear los arrays con los valores a sumar para productores y consumidores */
    /* En el array de productores se suman los pares (empieza en 0 y va de 2 en 2) */
    create_array(&producer_array, NULL, 110 * (P + C), 0, 2);
    /* Crear el array de consumidores con el mismo array que el de productores */
    /* En el array de consumidores se suman los impares (empieza en 1 y va de 2 en 2) */
    create_array(&consumer_array, producer_array.values, 110 * (P + C), 1, 2);

    /* Configurar las estructuras de entrada para cada hilo */
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
    stack.production_finished = 1;
    /* En vez de despertar (ya no hay variables de condición), despertar a los consumidores 
     * que esperaban porque el buffer estaba vacío (se despierta a uno y el resto se despiertan entre sí) */
    pthread_mutex_unlock(stack.empty);
    for (i = 0; i < C; i++) {
        pthread_join(consumers[i], NULL);
    }

    delete_stack(&stack);

    delete_array(&producer_array);
    consumer_array.values = NULL;   /* Se acaban de liberar en la función anterior */
    delete_array(&consumer_array);

    exit(EXIT_SUCCESS);
}


void* producer(void* args) {
    int i;
    int done;   /* Flag para saber si ya se terminó la suma */
    Stack* stack = ((struct thread_args*) args)->stack;
    Array* array = ((struct thread_args*) args)->array;

    i = 0;
    done = 0;
    while (i < NUM_ITEMS || !done) {
        if (i < NUM_ITEMS) { /* Comprobar si se han producido todos los items */
            produce(stack);
            i++;
        }
        if (!done) { /* Comprobar si ya se ha completado la contribución a la suma */
            done = contribute_sum(array);
            if (done) {
                producer_printf("Suma de los elementos pares del array finalizada.   Total = %d\n", array->sum); 
            }
        }
    }

    pthread_exit(NULL);
}


void* consumer(void* args) {
    int done;   /* Flag para saber si ya se terminó la suma */
    Stack* stack = ((struct thread_args*) args)->stack;
    Array* array = ((struct thread_args*) args)->array;
    
    done = 0;
    while (!(stack->production_finished) || stack->count || !done) {
        if (!(stack->production_finished) || stack->count) {    /* Hay que consumir hasta que los productores hayan acabado y el stack esté vacío */
            consume(stack);
        }
        if (!done) { /* Comprobar si ya se ha completado la contribución a la suma */
            done = contribute_sum(array);
            if (done) {
                consumer_printf("Suma de los elementos impares del array finalizada. Total = %d\n", array->sum);
            }
        }
    }

    pthread_exit(NULL);
}
