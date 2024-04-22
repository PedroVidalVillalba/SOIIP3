/**
 * Sistemas Operativos 2. Práctica 2.
 * Librería con las cabeceras de las funciones genéricas de
 * productor y consumidor usando un stack compartido controlado
 * por mutexes y variables de condición.
 *
 * @date 07/04/2024
 * @authors Arcos Salgado, Guillermo
 * @authors Vidal Villalba, Pedro
 */

#ifndef PRODUCER_CONSUMER
#define PRODUCER_CONSUMER

#define _GNU_SOURCE         /* Para poder utilizar gettid() para identificar al hilo */
#include "shared_stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/* Definir constantes y macros para la impresión */
#define PRODUCER_COLOR  "\x1b[36m"
#define CONSUMER_COLOR  "\x1b[31m"
#define NO_COLOR        "\x1b[0m"
#define bold(text) "\x1b[1m" text "\x1b[22m"

#define producer_printf(format, ...) ( printf(PRODUCER_COLOR "[%d] " format NO_COLOR, gettid(), ##__VA_ARGS__) )
#define consumer_printf(format, ...) ( printf(CONSUMER_COLOR "[%d] " format NO_COLOR, gettid(), ##__VA_ARGS__) )

/*** 
 * No se incluyen funciones de producer y consumer como tal, ya que estas podrían
 * cambiar entre ejercicios. Sí se incluyen las cabeceras de las funciones genéricas,
 * que se mantienen iguales entre los diferentes apartados.
 ***/

/**
 * Genera un entero aleatorio entre 0 y 10.
 *
 * @return Entero entre 0 y 10
 */
int produce_item(void);

/**
 * Coloca el entero <u>item</u> en <u>stack</u>.
 *
 * @param stack Stack en el que colocar el item.
 * @param item	Entero a colocar en el stack.
 *
 */
void insert_item(Stack* stack, int item);

/**
 * Retira el último elemento del <u>stack</u>.
 *
 * @param stack Stack del que retirar el item.
 *
 * @return	Entero retirado del stack.
 */
int remove_item(Stack* stack);

/**
 * Muestra por pantalla el entero consumido <u>item</u>, así como la
 * representación del <u>stack</u> actualizado.
 *
 * @param stack Stack del que se leyó el valor.
 * @param item	Último valor leído del stack.
 * @param position  Posición de la que se consumió el item.
 * @param representation    Representación textual del stack.
 */
void consume_item(Stack* stack, int item, int position, char* representation);

/**
 * Produce un item y lo introduce en el stack.
 *
 * @param stack Stack en el que introducir el item.
 */
void produce(Stack* stack);

/**
 * Consume un item del stack.
 *
 * @param stack Stack del que consumir el item.
 */
void consume(Stack* stack);

#endif //PRODUCER_CONSUMER
