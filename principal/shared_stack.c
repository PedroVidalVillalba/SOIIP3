/**
 * Sistemas Operativos 2. Práctica 2.
 * Librería con las implementaciones de las funciones genéricas de
 * para crear, representar y destruir un stack compartido controlado
 * por mutexes y variables de condición
 *
 * @date 16/04/2024
 * @authors Arcos Salgado, Guillermo
 * @authors Vidal Villalba, Pedro
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "shared_stack.h"


void create_stack(Stack* stack, int size) {
    /* Alojar memoria para el buffer (al usar hilos es la misma para todos) */
    stack->buffer = (int *) calloc(size, sizeof(int));
    stack->size = size;
    stack->count = 0;   		/* El buffer se inicia vacío */
    stack->production_finished = 0; 	/* La producción empieza sin terminar */

    /* Alojar memoria para el mutex y las variables de condición */
    stack->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    stack->cond_c = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    stack->cond_p = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));

    /* Inicializar mutex y variables de condición */
    if (pthread_mutex_init(stack->mutex, NULL)) fail("ERROR: No se pudo crear el mutex para el stack.");
    if (pthread_cond_init(stack->cond_c, NULL)) fail("ERROR: No se pudo crear la variable de condición para los consumidores");
    if (pthread_cond_init(stack->cond_p, NULL)) fail("ERROR: No se pudo crear la variable de condición para los productores");
}


void delete_stack(Stack* stack) {
    /* Liberar el buffer en memoria compartida */
    free(stack->buffer);
    stack->buffer = NULL;
    stack->size = -1;   /* Marcar con valores de error */
    stack->count = -1;

    /* Cerrar mutex y variables de condición */
    if (pthread_mutex_destroy(stack->mutex)) fail("ERROR: No se pudo destruir el mutex para el stack");
    if (pthread_cond_destroy(stack->cond_c)) fail("ERROR: No se pudo destruir la variable de condición para los consumidores");
    if (pthread_cond_destroy(stack->cond_p)) fail("ERROR: No se pudo destruir la variable de condición para los productores");
    free(stack->mutex);
    free(stack->cond_c);
    free(stack->cond_p);
}


void update_representation(Stack* stack, char* representation) {
    int i;

    representation[0] = '[';
    /* Imprimir los items en el stack hasta count */
    for (i = 0; i < stack->count; i++) {
        sprintf(representation + 1 + 3 * i, "%2d|", stack->buffer[i]);
    }
    /* Rellenar con huecos el resto de espacios */
    for (; i < stack->size; i++) {
        sprintf(representation + 1 + 3 * i, "  |");
    }
    representation[3 * stack->size] = ']';
}

