/**
 * Sistemas Operativos 2. Práctica 3.
 * Librería con la implementación de las funciones genéricas de
 * la operación de suma sobre un array
 *
 * @date 16/04/2024
 * @authors Arcos Salgado, Guillermo
 * @authors Vidal Villalba, Pedro
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "array_sum.h"

#define fail(message)  { perror(message); exit(EXIT_FAILURE); }

#ifdef SLEEP
extern struct {
    int production;
    int insertion;
    int contribution_p;
    int extraction;
    int consumption;
    int contribution_c;
} sleep_times;
#endif //SLEEP

void create_array(Array* array, int* values, int size, int start, int step) {
    int i;

    /* Copiar o inicializar el vector de valores */
    if (values) {
        array->values = values;
    } else {
        array->values = (int *) malloc(size * sizeof(int));
        for (i = 0; i < size; i++) {
            array->values[i] = i / 2;
        }
    }
    array->size = size;
    array->sum = 0; /* La suma de los elementos empieza en 0 */
    array->count = start;/* Empezar en la posición indicada */
    array->step = step;

    /* Reservamos memoria para el mutex */
    array->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    /* Inicializar el mutex para el array */
    if (pthread_mutex_init(array->mutex, NULL)) fail("ERROR: No se pudo crear el mutex para el array.");
}


void delete_array(Array* array) {
    if (array->values) free(array->values);
    array->values = NULL;
    array->size = -1;   /* Marcar con valores de error */
    array->count = -1;

    /* Cerrar mutex */
    if (pthread_mutex_destroy(array->mutex)) fail("ERROR: No se pudo destruir el mutex para el stack");
    free(array->mutex);
}


int contribute_sum(Array* array) {
    int done;
    int i;
    int r;  /* Número aleatorio de elementos a los que contribuir cada vez, entre 2 y 4 */

    /* Intentar bloquear el array para contribuir a la suma */
    if (pthread_mutex_trylock(array->mutex)) {  /* El array ya está siendo utilizado */
        return (array->count >= array->size);
    }
    /* Mutex bloqueado con éxito; estamos dentro de la región crítica */
#ifdef SLEEP
    if (array->count % 2)
        sleep(sleep_times.contribution_c);
    else
        sleep(sleep_times.contribution_p);
#endif //SLEEP

    r = (random() % 3) + 2;
    for (i = 0; i < r && array->count < array->size; i++) {
        array->sum += array->values[array->count];
        array->count += array->step;
    }
    done = (array->count >= array->size);   /* Acabamos si el contador iguala o supera al tamaño */
    pthread_mutex_unlock(array->mutex);
    /* Mutex desbloqueado */

    return done;
}
