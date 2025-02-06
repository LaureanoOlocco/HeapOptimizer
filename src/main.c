#include "memory.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void initialize_and_display(int *arr, int start, int end) {
  for (int i = start; i < end; i++) {
    arr[i] = i * 10;
    printf("arr[%d] = %d\n", i, arr[i]);
  }
}

void check_and_report(int *arr) {
  printf("\nCalling check_heap function\n");
  check_heap(arr);
  printf("\nCalling memory_usage function to report memory usage\n");
  memory_usage();
}

void test_performance(int method) {

  set_method(method);
  clock_t start, end;
  double cpu_time_used;
  int *allocations[NUM_TESTS];
  size_t total_allocated = 0;
  size_t total_freed = 0;

  start = clock();
  for (int i = 0; i < NUM_TESTS; i++) {
    size_t size = rand() % MAX_ALLOC_SIZE + 1;
    allocations[i] = (int *)my_malloc(size * sizeof(int));
    if (allocations[i] != NULL) {
      total_allocated += size;
    }
  }

  for (int i = 0; i < NUM_TESTS; i++) {
    if (allocations[i] != NULL) {
      my_free(allocations[i]);
      total_freed++;
    }
  }
  end = clock();
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000;

  printf("Method: %d\n", method);
  printf("Total allocated: %zu\n", total_allocated);
  printf("Total freed: %zu\n", total_freed);
  printf("CPU time used: %.2f microseconds\n", cpu_time_used);
}

void test_random_fragmentation(int num_blocks, int max_block_size) {
  // Array para mantener los punteros a los bloques
  void **blocks = my_malloc(num_blocks * sizeof(void *));

  for (int i = 0; i < num_blocks; i++) {
    // Tamaño aleatorio entre 1 y max_block_size
    int size = (rand() % max_block_size + 1) * sizeof(int);
    blocks[i] = my_malloc(size);
    // printf("Bloque %d: Asignado %d bytes\n", i, size);
  }

  int blocks_to_free = num_blocks / 2; // Liberar aproximadamente la mitad
  for (int i = 0; i < blocks_to_free; i++) {
    int index;
    do {
      index = rand() % num_blocks;
    } while (blocks[index] == NULL);

    my_free(blocks[index]);
    // printf("Liberado bloque %d\n", index);
    blocks[index] = NULL;
  }

  // Calcular fragmentación usando la función existente
  printf("\n--- Calculando fragmentación ---\n");
  external_frag();

  // Liberar los bloques restantes
  for (int i = 0; i < num_blocks; i++) {
    if (blocks[i] != NULL) {
      my_free(blocks[i]);
    }
  }
  my_free(blocks);
}

void test_memory_functions() {

  srand(time(NULL));

  set_method(0); // 0 para First Fit
  // write(STDOUT_FILENO, "Allocating memory using malloc\n", 32);
  printf("\nAllocating memory using malloc\n");
  int *arr = (int *)my_malloc(10 * sizeof(int));

  if (arr == NULL) {
    printf("Error allocating memory\n");
    return;
  }

  // Inicializar y mostrar valores
  initialize_and_display(arr, 0, 10);
  check_and_report(arr);

  // Reasignar memoria usando realloc
  printf("\nReallocating memory to increase size\n");
  arr = (int *)my_realloc(arr, 20 * sizeof(int));
  if (arr == NULL) {
    printf("Error reallocating memory\n");
    return;
  }

  // Inicializar nuevos elementos y mostrar todos los valores
  initialize_and_display(arr, 10, 20);
  check_and_report(arr);

  // Asignar memoria usando calloc
  printf("\nAllocating memory using calloc\n");
  int *zeroed_arr = (int *)my_calloc(5, sizeof(int));
  if (zeroed_arr == NULL) {
    printf("Error allocating memory with calloc\n");
    return;
  }

  // Mostrar valores inicializados a cero
  printf("\nZero-initialized values:\n");
  for (int i = 0; i < 5; i++) {
    printf("zeroed_arr[%d] = %d\n", i, zeroed_arr[i]);
  }

  // Liberar memoria
  my_free(arr);
  my_free(zeroed_arr);

  // Comprobar el estado del heap después de liberar
  check_and_report(arr);
}

int main() {

  test_memory_functions();

  test_performance(0);
  test_performance(1);
  test_performance(2);

  for (int i = 0; i < NUM_ITERATIONS; i++) {
    test_random_fragmentation(20, 100); // 20 bloques, tamaño máximo de 100
    sleep(1);
  }

  return 0;
}