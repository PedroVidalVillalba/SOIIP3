/**
 * Sistemas Operativos 2. Práctica 2.
 * Librería con las cabeceras de las funciones genéricas de
 * para crear, representar y destruir un stack compartido controlado
 *
 * @date 09/04/2024
 * @authors Arcos Salgado, Guillermo
 * @authors Vidal Villalba, Pedro
 */

#ifndef SHARED_STACK
#define SHARED_STACK

#include <sys/types.h>
#include <pthread.h>

#define fail(message)  { perror(message); exit(EXIT_FAILURE); }

/**
 * Estructura en la que incluir toda la información relevante de un buffer compartido con
 * sus correspondientes semáforos
 */
typedef struct {
    int* buffer;                    /* Buffer en memoria compartida donde guardar los datos */
    int size;                       /* Tamaño del buffer */
    int count;                      /* Posición del último hueco libre que hay actualmente en el buffer */
    int production_finished;        /* Flag para indicar a los consumidores cuándo acabaron los productores */
    pthread_mutex_t* mutex;         /* Mutex que regula el acceso al buffer */
    pthread_mutex_t* empty;         /* Mutex emulando variable de condición para los consumidores */
    pthread_mutex_t* full;          /* Mutex emulando variable de condición para los productores */
} Stack;

/**
 * Crea un nuevo Stack , de tamaño <u>size</u>,
 * y con sus correspondientes mutex y variables de condición.
 *
 * @param stack Dirección en la que guardar el nuevo Stack.
 * @param size  Tamaño del buffer.
 */
void create_stack(Stack* stack, int size);

/**
 * Elimina un Stack de memoria, liberando el buffer de memoria compartida,
 * cerrando y destruyendo sus mutex y variables de condición asociadas.
 *
 * @param stack Puntero al Stack a destruir.
 */
void delete_stack(Stack* stack);

/**
 * Actualiza la representación textual del <u>stack</u>.
 *
 * @param stack Stack a actualizar.
 * @param representation    Representación textual del stack para cada hilo.
 *                          Debe estar ya alojada y tener espacio suficiente (3 * stack->size + 2).
 */
void update_representation(Stack* stack, char* representation);

#endif //SHARED_STACK
