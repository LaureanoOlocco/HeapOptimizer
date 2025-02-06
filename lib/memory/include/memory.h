/**
 * @file memory.h
 * @brief Memory management library with custom allocation functions.
 *
 * Esta biblioteca define funciones de asignación de memoria dinámica
 * y gestión de bloques para crear un asignador de memoria personalizado.
 */

#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

/**
 * @brief Macro para alinear una cantidad de bytes al siguiente múltiplo de 8.
 *
 * @param x Cantidad de bytes a alinear.
 */
#define align(x) (((((x) - 1) >> 3) << 3) + 8)

/** Tamaño mínimo de un bloque de memoria. */
#define BLOCK_SIZE 40
/** Tamaño de página en memoria. */
#define PAGESIZE 4096
/** Política de asignación First Fit. */
#define FIRST_FIT 0
/** Política de asignación Best Fit. */
#define BEST_FIT 1
/** Política de asignación Worst Fit. */
#define WORST_FIT 2
/** Tamaño del bloque */
#define DATA_START 1
/** Dirección inválida */
#define INVALID_ADDR 0


typedef enum {
    ALLOC_TYPE_MALLOC,
    ALLOC_TYPE_CALLOC,
    ALLOC_TYPE_REALLOC,
    ALLOC_TYPE_FREE
} alloc_type;

/**
 * @struct s_block
 * @brief Estructura para representar un bloque de memoria.
 *
 * Contiene la información necesaria para gestionar la asignación y
 * liberación de un bloque de memoria.
 */
struct s_block
{
    size_t size;           /**< Tamaño del bloque de datos. */
    struct s_block *next;  /**< Puntero al siguiente bloque en la lista enlazada. */
    struct s_block *prev;  /**< Puntero al bloque anterior en la lista enlazada. */
    int free;              /**< Indicador de si el bloque está libre (1) o ocupado (0). */
    void *ptr;             /**< Puntero a la dirección de los datos almacenados. */
    char data[DATA_START]; /**< Área donde comienzan los datos del bloque. */
};

/** Tipo de puntero para un bloque de memoria. */
typedef struct s_block *t_block;

/**
 * @brief Obtiene el bloque que contiene una dirección de memoria dada.
 *
 * @param p Puntero a la dirección de datos.
 * @return t_block Puntero al bloque de memoria correspondiente.
 */
t_block get_block(void *p);

/**
 * @brief Verifica si una dirección de memoria es válida.
 *
 * @param p Dirección de memoria a verificar.
 * @return int Retorna 1 si la dirección es válida, 0 en caso contrario.
 */
int valid_addr(void *p);

/**
 * @brief Encuentra un bloque libre que tenga al menos el tamaño solicitado.
 *
 * @param last Puntero al último bloque.
 * @param size Tamaño solicitado.
 * @return t_block Puntero al bloque encontrado, o NULL si no se encuentra ninguno.
 */
t_block find_block(t_block *last, size_t size);

/**
 * @brief Expande el heap para crear un nuevo bloque de memoria.
 *
 * @param last Último bloque del heap.
 * @param s Tamaño del nuevo bloque.
 * @return t_block Puntero al nuevo bloque creado.
 */
t_block extend_heap(t_block last, size_t s);

/**
 * @brief Divide un bloque de memoria en dos, si el tamaño solicitado es menor que el bloque disponible.
 *
 * @param b Bloque a dividir.
 * @param s Tamaño del nuevo bloque.
 */
void split_block(t_block b, size_t s);

/**
 * @brief Fusiona un bloque libre con su siguiente bloque si también está libre.
 *
 * @param b Bloque a fusionar.
 * @return t_block Puntero al bloque fusionado.
 */
t_block fusion(t_block b);

/**
 * @brief Copia el contenido de un bloque de origen a un bloque de destino.
 *
 * @param src Bloque de origen.
 * @param dst Bloque de destino.
 */
void copy_block(t_block src, t_block dst);

/**
 * @brief Asigna un bloque de memoria del tamaño solicitado.
 *
 * @param size Tamaño en bytes del bloque a asignar.
 * @return void* Puntero al área de datos asignada.
 */
void *my_malloc(size_t size);

/**
 * @brief Libera un bloque de memoria previamente asignado.
 *
 * @param p Puntero al área de datos a liberar.
 */
void my_free(void *p);

/**
 * @brief Asigna un bloque de memoria para un número de elementos, inicializándolo a cero.
 *
 * @param number Número de elementos.
 * @param size Tamaño de cada elemento.
 * @return void* Puntero al área de datos asignada e inicializada.
 */
void *my_calloc(size_t number, size_t size);

/**
 * @brief Cambia el tamaño de un bloque de memoria previamente asignado.
 *
 * @param p Puntero al área de datos a redimensionar.
 * @param size Nuevo tamaño en bytes.
 * @return void* Puntero al área de datos redimensionada.
 */
void *my_realloc(void *p, size_t size);

/**
 * @brief Verifica el estado del heap y detecta bloques libres consecutivos.
 *
 * @param data Información adicional para la verificación.
 */
void check_heap(void *data);

/**
 * @brief Configura el modo de asignación de memoria (First Fit o Best Fit).
 *
 * @param mode Modo de asignación (0 para First Fit, 1 para Best Fit).
 */
void malloc_control(int mode);

/**
 * @brief Establece el método de asignación de memoria.
 *
 * Esta función permite seleccionar el método de asignación de memoria
 * que se utilizará al buscar bloques libres. Los métodos disponibles son:
 * - 0: Primer ajuste (First Fit)
 * - 1: Mejor ajuste (Best Fit)
 *
 * @param m Un entero que representa el método de asignación deseado.
 *          Debe ser 0 o 1. Cualquier otro valor es inválido.
 */
void set_method(int m);

/**
 * @brief Reporta el uso de memoria actual.
 *
 * Esta función recorre todos los bloques de memoria gestionados por el
 * administrador de memoria personalizado y calcula el tamaño total de
 * memoria asignada y la cantidad de memoria libre. Luego, imprime un
 * informe detallado del uso de memoria.
 *
 * El informe incluye:
 * - Total de memoria asignada en bytes.
 * - Total de memoria libre en bytes.
 *
 * Esta función es útil para monitorear la eficiencia del uso de memoria
 * y detectar posibles problemas de fragmentación o fugas de memoria.
 */
void memory_usage();

/**
 * @brief Logs the name of a function call.
 *
 * This function logs the name of the function that is being called.
 *
 * @param func_name The name of the function to log.
 * @param type The type of allocation.
 * @param ptr The pointer to the allocated memory.
 * @param size The size of the allocated memory.
 */
void log_function_call(const char *func_name, alloc_type type, void *ptr, size_t size);
/**
 * @brief Allocates a block of memory of the specified size.
 *
 * This function allocates a block of memory of at least the specified size
 * and returns a pointer to the beginning of the block. The contents of the
 * allocated memory are uninitialized.
 *
 * @param size The size of the memory block to allocate, in bytes.
 * @return A pointer to the allocated memory block, or NULL if the allocation fails.
 */
void *custom_malloc(size_t size);
/**
 * @brief Allocates memory for an array of elements and initializes it to zero.
 *
 * This function allocates memory for an array of `number` elements, each of
 * `size` bytes, and initializes all bytes in the allocated memory to zero.
 *
 * @param number The number of elements to allocate.
 * @param size The size of each element in bytes.
 * @return A pointer to the allocated memory, or NULL if the allocation fails.
 */
void *custom_calloc(size_t number, size_t size);
/**
 * @brief Reallocates memory block to a new size.
 *
 * This function changes the size of the memory block pointed to by `ptr` to `size` bytes.
 * The contents will be unchanged to the minimum of the old and new sizes; newly allocated memory will be uninitialized.
 * If `ptr` is NULL, the function behaves like `malloc(size)`.
 * If `size` is zero and `ptr` is not NULL, the function behaves like `free(ptr)` and returns NULL.
 *
 * @param ptr Pointer to the memory block to be reallocated.
 * @param size New size for the memory block in bytes.
 * @return Pointer to the reallocated memory block, or NULL if the reallocation fails.
 */
void *custom_realloc(void *ptr, size_t size);
/**
 * @brief Frees the memory space pointed to by ptr.
 *
 * This function deallocates the memory previously allocated by a call to
 * custom_malloc, custom_calloc, or custom_realloc. The pointer ptr must
 * have been returned by one of these functions, and must not have been
 * freed yet. If ptr is NULL, no operation is performed.
 *
 * @param ptr Pointer to the memory to be freed.
 */
void custom_free(void *ptr);
/**
 * @brief Calculates the external fragmentation of the memory.
 *
 * This function computes the external fragmentation, which is the total amount
 * of free memory that is not usable due to being divided into small, non-contiguous
 * blocks. It returns the fragmentation as a floating-point value.
 *
 * @return A float representing the external fragmentation.
 */
double external_frag();
