/**
 * Sistemas Operativos 2. Práctica 2.
 * Librería con las implementaciones de las funciones genéricas de
 * productor y consumidor usando un stack compartido controlado
 * por mutexes y variables de condición.
 *
 * @date 16/04/2024
 * @authors Arcos Salgado, Guillermo
 * @authors Vidal Villalba, Pedro
 */

#include "producer_consumer.h"

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

int produce_item() {
#ifdef SLEEP
    sleep(sleep_times.production);
#endif //SLEEP
    return ((int) random() % 11);
}


void insert_item(Stack* stack, int item) {
#ifdef SLEEP
    sleep(sleep_times.insertion);
#endif //SLEEP
    /* Almacenar el número de items actualmente en el stack, según lo ve el proceso que inserta,
     * en el campo count del stack */
    stack->buffer[(stack->count)++] = item; /* Insertar el item y aumentar el contador */
}

void produce(Stack* stack){
    int local_count, item;
    char* representation;

    representation = (char *) calloc(3 * stack->size + 2, sizeof(char));
    item = produce_item();
    pthread_mutex_lock(stack->mutex);                                                   /* Entra a la región crítica */
    while (stack->count == stack->size) pthread_cond_wait(stack->cond_p, stack->mutex); /* Se bloquea si no puede añadir más items */
    insert_item(stack, item);                                                       	/* Poner item en el stack */
    update_representation(stack, representation);                                       /* Actualizar la representación del buffer */
    local_count = stack->count;                                                         /* Guardar localmente el contador */
    pthread_cond_signal(stack->cond_c);                                             	/* Despertar algún consumidor */
    pthread_mutex_unlock(stack->mutex);                                                 /* Sale de la región crítica */

    /* Como ya aumentamos el contador, no tenemos que sumarle 1 para pasarlo a indexado en 1 */
    producer_printf("Añadido el item   "bold("%2d")" a la posición  "bold("%2d")". Buffer: %s\n", item, local_count, representation);
    free(representation);
}


int remove_item(Stack* stack) {
    int item;

#ifdef SLEEP
    sleep(sleep_times.extraction);
#endif //SLEEP
    /* Almacenar el número de items actualmente en el stack, según lo ve el proceso que elimina,
     * en el campo count del stack */
    item = stack->buffer[--(stack->count)];
    stack->buffer[stack->count] = 0; /* Eliminar el entero del buffer */

    return item;
}


void consume_item(Stack* stack, int item, int position, char* representation) {
#ifdef SLEEP
    sleep(sleep_times.consumption);
#endif //SLEEP
    consumer_printf("Eliminado el item "bold("%2d")" de la posición "bold("%2d")". Buffer: %s\n", item, position, representation);
}

void consume(Stack* stack){
    char* representation;
    int local_count, item;

    representation = (char *) calloc(3 * stack->size + 2, sizeof(char));
    pthread_mutex_lock(stack->mutex);       /* Entrar en la región crítica */
    while (!stack->count) {                 /* Comprobar si el buffer está vacío; si no lo está ejecutar consumir un item con normalidad */
        if (stack->production_finished) {   /* Comprobar si han acabado los productores; si ya acabaron, salir */
            pthread_mutex_unlock(stack->mutex);
            return;
        }
        pthread_cond_wait(stack->cond_c, stack->mutex);     /* Si los productores aún no terminaron, esperar */
    }
    item = remove_item(stack);                      		/* Eliminar elemento de la cola */
    update_representation(stack, representation);   		/* Actualizar la representación del buffer */
    local_count = stack->count;                     		/* Guardar localmente el contador */
    pthread_cond_signal(stack->cond_p);         		/* Despertar productores */
    pthread_mutex_unlock(stack->mutex);             		/* Salir de la región crítica */
    consume_item(stack, item, local_count + 1, representation); /* Imprimir elemento */
    free(representation);
}
