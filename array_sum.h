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
    int* values;                /* Array de enteros sobre el que se realiza la operación */
    int size;                   /* Tamaño del array */
    int sum;                    /* Suma de elementos del array (hasta el momento) */
    int count;                  /* Posición del último elemento del array sumado */
    int step;                   /* Número de posiciones que avanzar en cada acceso */
    pthread_mutex_t mutex;      /* Mutex que regula el acceso a la suma y la cuenta */
} Array;


/**
 * Crea un Array sobre el que poder sumar sus valores.
 *
 * @param array     Puntero al Array en el que guardar la estructura a crear.
 * @param values    Vector de valores a contener en el array; 
 *                  si es NULL, se creará automáticamente como values[i] = i/2.
 * @param size      Tamaño del Array a crear o suministrado.
 * @param start     Posición inicial del Array en el que empezar el contador.      
 * @param step      Número de posiciones que avanzar en cada acceso al array.
 */
void create_array(Array* array, int* values, int size, int start, int step);

/**
 * Elimina un Array de memoria, liberando el vector de valores
 * y cerrando y destruyendo su mutex asociado.
 *
 * @param array Puntero al Array a destruir.
 */
void delete_array(Array* array);

/**
 * Contribuye a la suma de los valores del array, si es posible.
 * En caso de que el array esté bloqueado por otro hilo, o ya se haya completado
 * la suma, retorna.
 *
 * @param array Array sobre la que realizar la suma.
 *
 * @return  Si ya se han sumado todos los elementos del array, devuelve 1; en caso contrario, devuelve 0.
 */
int contribute_array(Array* array);

#endif //ARRAY_SUM
