#include "memory.h"
#include <memory.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define NUM_TESTS 1300    /** Número de pruebas a ejecutar. */
#define MAX_ALLOC_SIZE 64 /** Tamaño máximo de asignación de memoria. */
#define NUM_ITERATIONS                                                         \
  10 /** Número de iteraciones para las pruebas de rendimiento. */

/**
 * @brief Inicializa y muestra un array de enteros.
 *
 * @param arr Puntero al array de enteros.
 * @param start Índice de inicio.
 * @param end Índice de fin.
 */
void initialize_and_display(int *arr, int start, int end);

/**
 * @brief Comprueba y reporta el estado del heap.
 *
 * @param arr Puntero al array de enteros.
 */
void check_and_report(int *arr);

/**
 * @brief Ejecuta una prueba de asignación y liberación de memoria.
 *
 * @param method Método de asignación a usar.
 */
void test_performance(int method);
