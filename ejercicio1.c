/**
 * Sistemas Operativos 2.
 * Práctica 3. Sincronización de procesos con mutexes.
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


int producers_finished;     /* Flag global para indicar a los consumidores cuándo acabaron los productores */

int main(int argc, char** argv) {
    pthread_t producers[P], consumers[C];
    Stack stack;
    Array producer_array, consumer_array;
    struct thread_args producer_args, consumer_args;
    int i;

    /* todo: Iniciar las variables compartidas */
    /* Crear los arrays con los valores a sumar para productores y consumidores */
    create_array(&producer_array, NULL, 110 * (P + C), 0, 2);
    /* Crear el array de consumidores con el mismo array que el de productores */
    create_array(&consumer_array, producer_array->values, 110 * (P + C), 1, 2);

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
    producers_finished = 1;
    for (i = 0; i < C; i++) {
        pthread_join(consumers[i], NULL);
    }

    /* todo: destruir todas las variables */
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
        if (i < NUM_ITEMS) {
            produce(stack);
            i++;
        }
        if (!done) {
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
    while (!producers_finished || !done) {
        if (!producers_finished) {
            consume(stack);
        } else {    /* Si los productores ya acabaron de producir, hay que ir con cuidado */
            /* todo: Buscar una solución para eso con calma. 
             * NO es para nada trivial: mientras los productores no hayan acabado, los
             * consumidores tendrían que trabajar con normalidad.
             * Una vez los productores hayan terminado, hay que consumir las veces
             * necesarias para vaciar el stack, incluyendo tanto a los consumidores que
             * ya estaban esperando por el mutex, como posiblemente hacer que entren
             * más consumidores hasta vaciarlo, sin que nadie se quede esperando
             * dentro de la región crítica por una variable de condición y hacer que
             * todos terminen */
            /* Posibles soluciones: 
             *  @ Serializar esta parte, con otro mutex para controlar el acceso o
             *    usando un mutex recursivo (bloqueamos el stack fuera y otra vez 
             *    dentro de consume()).
             *  @ Usar pthread_cond_broadcast() para despertar a todos los hilos 
             *    que estuvieran ya esperando dentro de la región crítica.
             *    Implicaría tener que comprobar si la cuenta del stack ya está a 0
             *    después de despertar (se hace de todas formas en el while),
             *    y salir si es el caso (siempre y cuando ya 
             *    hayan terminado todos los hilos).
             */
        }
        if (!done) {
            done = contribute_sum(array);
            if (done) {
                consumer_printf("Suma de los elementos impares del array finalizada. Total = %d\n", array->sum);
            }
        }
    }
    
}
