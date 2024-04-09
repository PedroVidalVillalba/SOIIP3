/**
 * Sistemas Operativos 2. Práctica 3.
 * Librería con las cabeceras de las funciones genéricas de
 * la operación de suma sobre un array
 *
 * @date 09/04/2024
 * @authors Arcos Salgado, Guillermo
 * @authors Vidal Villalba, Pedro
 */
#ifndef ARRAY_SUM
#define ARRAY_SUM

#include <pthread.h>

/**
 * Estructura en la que incluir la información sobre la operación de suma
 * en el array
 */
typedef struct{
    int* array;                 /* Array de enteros sobre el que se realiza la operación */
    int sum;                    /* Suma total del array */
    int count;                  /* Posición del último elemento del array sumado */
    pthread_mutex_t mutex;      /* Mutex que regula el acceso a la suma */
} Array;

#endif //ARRAY_SUM
