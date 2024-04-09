/**
 * Sistemas Operativos 2. Práctica 2.
 * Librería con las cabeceras de las funciones genéricas de
 * para crear, representar y destruir un stack compartido controlado
 *
 * @date 07/04/2024
 * @authors Falgueras Casarejos, Yago
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
    int count;                      /* Posición del último elemento que hay actualmente en el buffer, según lo ve cada proceso */
    pthread_mutex_t mutex;          /* Mutex que regula el acceso al buffer */
    pthread_cond_t cond_c;          /* Variable de condición para los consumidores */
    pthread_cond_t cond_p;          /* Variable de condición para los productores */
} Stack;

/**
 * Crea un nuevo Stack en memoria compartida, de tamaño <u>size</u>,
 * y con sus correspondientes semáfóros identificados por el nombre
 * (e.g., "/mutex_<name>").
 *
 * @param stack Dirección en la que guardar el nuevo Stack.
 * @param size  Tamaño del buffer.
 * @param name  Nombre con el que identificar a los semáforos del nuevo Stack.
 */
void create_stack(Stack* stack, int size, const char* name);

/**
 * Elimina un Stack de memoria, liberando el buffer de memoria compartida,
 * su representación textual y cerrando y destruyendo sus semáforos asociados.
 *
 * @param stack Puntero al Stack a destruir.
 */
void delete_stack(Stack* stack);

/**
 * Actualiza la representación textual del <u>stack</u>.
 *
 * @param stack Stack a actualizar.
 */
void update_representation(Stack* stack);

#endif //SHARED_STACK
