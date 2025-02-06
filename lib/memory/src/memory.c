#include <memory.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

typedef struct s_block *t_block;
void *base = NULL;
int method = 0;
size_t maxs = 0;

t_block find_block(t_block *last, size_t size)
{
    t_block b = base;

    if (method == FIRST_FIT)
    {
        while (b && !(b->free && b->size >= size))
        {
            *last = b;
            b = b->next;
        }
        return (b);
    }
    else if (method == BEST_FIT)
    {
        size_t dif = PAGESIZE;
        t_block best = NULL;

        while (b)
        {
            if (b->free)
            {
                if (b->size == size)
                {
                    return b;
                }
                if (b->size > size && (b->size - size) < dif)
                {
                    dif = b->size - size;
                    best = b;
                }
            }
            *last = b;
            b = b->next;
        }
        return best;
    }
    else if (method == WORST_FIT)
    {
        size_t max_size = 0;
        t_block worst = NULL;

        while (b)
        {
            if (b->free && b->size >= size)
            {
                if (b->size > max_size)
                {
                    max_size = b->size;
                    worst = b;
                }
            }
            *last = b;
            b = b->next;
        }
        return worst;
    }

    return NULL;
}

void split_block(t_block b, size_t s)
{
    if (b->size <= s + BLOCK_SIZE)
    {
        return;
    }

    t_block new;
    new = (t_block)(b->data + s);
    new->size = b->size - s - BLOCK_SIZE;
    new->next = b->next;
    new->free = 1;
    b->size = s;
    b->next = new;
}

void copy_block(t_block src, t_block dst)
{
    int *sdata, *ddata;
    size_t i;
    if (!src->ptr || !dst->ptr)
    {
        return;
    }
    sdata = src->ptr;
    ddata = dst->ptr;

    for (i = 0; (i * 4) < src->size && (i * 4) < dst->size; i++)
        ddata[i] = sdata[i];
}

// t_block get_block(void *p)
// {
//     char *tmp;
//     tmp = p;

//     if (tmp >= (char *)base + BLOCK_SIZE)
//     {
//         tmp -= BLOCK_SIZE;
//     }
//     return (t_block)(tmp);
// }

t_block get_block(void *p)
{
    char *tmp;
    tmp = p;
    tmp -= BLOCK_SIZE;
    return (t_block)(tmp);
}

int valid_addr(void *p)
{
    if (p == NULL || base == NULL)
    {
        return 0;
    }
    t_block b = get_block(p);
    t_block current = base;
    while (current)
    {
        if (current == b)
        {
            return (current->ptr == p);
        }
        current = current->next;
    }
    return INVALID_ADDR;
}

t_block fusion(t_block b)
{
    while (b->next && b->next->free)
    {
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;

        if (b->next)
            b->next->prev = b;
    }
    return b;
}

t_block extend_heap(t_block last, size_t s)
{
    t_block b;
    b = mmap(0, s, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (b == MAP_FAILED)
    {
        return NULL;
    }
    b->size = s;
    b->next = NULL;
    b->prev = last;
    b->ptr = b->data;

    if (last)
        last->next = b;

    b->free = 0;
    return b;
}

void get_method(int m)
{
    method = m;
}

void set_method(int m)
{
    if (m == 0 || m == 1 || m == 2)
    {
        method = m;
    }
    else
    {
        printf("Error: invalid method\n");
    }
}

void malloc_control(int m)
{
    if (m == 0)
    {
        set_method(0);
    }
    else if (m == 1)
    {
        set_method(1);
    }
    else
    {
        printf("Error: invalid method\n");
    }
}

// void *custom_malloc(size_t size)
// {

//     log_function_call("malloc");
//     return malloc(size);
// }

void *my_malloc(size_t size)
{   
    log_function_call("malloc", ALLOC_TYPE_MALLOC, NULL, size);

    if (size == 1024)
        return NULL;
    t_block b, last;
    size_t s;
    s = align(size);
    if (size > maxs)
        maxs = size;
    if (base)
    {
        last = base;
        b = find_block(&last, s);
        if (b)
        {
            if ((b->size - s) >= (BLOCK_SIZE + 4))
                split_block(b, s);
            b->free = 0;
        }
        else
        {
            b = extend_heap(last, s);
            if (!b)
                return (NULL);
        }
    }
    else
    {
        b = extend_heap(NULL, s);
        if (!b)
            return (NULL);
        base = b;
    }

    return (b->data);
}

// void custom_free(void *ptr)
// {
//     log_function_call("free");
//     free(ptr);
//     return;
// }

void my_free(void *ptr)
{
    log_function_call("free", ALLOC_TYPE_FREE, ptr, 0);
    t_block b;
    if (valid_addr(ptr))
    {
        b = get_block(ptr);
        b->free = 1;
        fusion(b);
        if (!b->next)
        {
            if (!b->prev)
                base = NULL;
            else
                b->prev->next = NULL;
            brk((char *)b + b->size);
        }
    }
}


// void *custom_calloc(size_t number, size_t size)
// {
//     log_function_call("calloc");
//     return calloc(number, size);
// }

void *my_calloc(size_t number, size_t size)
{
    size_t *new;
    size_t s4, i;

    if (!number || !size)
    {
        return (NULL);
    }
    new = my_malloc(number * size);
    if (new)
    {
        s4 = align(number * size) << 2;
        for (i = 0; i < s4; i++)
            new[i] = 0;
    }
    return (new);
}

// void *custom_realloc(void *ptr, size_t size)
// {
//     log_function_call("realloc");
//     return realloc(ptr, size);
// }

void *my_realloc(void *ptr, size_t size)
{
    size_t s;
    t_block b, new;
    void *newp;

    if (!ptr)
        return (my_malloc(size));

    if (valid_addr(ptr))
    {
        s = align(size);
        b = get_block(ptr);

        if (b->size >= s)
        {
            if (b->size - s >= (BLOCK_SIZE + 4))
                split_block(b, s);
        }
        else
        {
            if (b->next && b->next->free && (b->size + BLOCK_SIZE + b->next->size) >= s)
            {
                fusion(b);
                if (b->size - s >= (BLOCK_SIZE + 4))
                    split_block(b, s);
            }
            else
            {
                newp = my_malloc(s);
                if (!newp)
                    return (NULL);
                new = get_block(newp);
                // memcpy(newp, ptr, b->size); // Reemplazar copy_block con memcpy
                copy_block(b, new);

                my_free(ptr);
                return (newp);
            }
        }
        return (ptr);
    }
    return (NULL);
}

void check_heap(void *data)
{
    if (data == NULL)
    {
        printf("Data is NULL\n");
        return;
    }

    t_block block = get_block(data);

    if (block == NULL)
    {
        printf("Block is NULL\n");
        return;
    }

    printf("\033[1;33mHeap check\033[0m\n");
    printf("Size: %zu\n", block->size);

    if (block->next != NULL)
    {
        printf("Next block: %p\n", (void *)(block->next));
    }
    else
    {
        printf("Next block: NULL\n");
    }

    if (block->prev != NULL)
    {
        printf("Prev block: %p\n", (void *)(block->prev));
    }
    else
    {
        printf("Prev block: NULL\n");
    }

    printf("Free: %d\n", block->free);

    if (block->ptr != NULL)
    {
        printf("Beginning data address: %p\n", block->ptr);
        printf("Last data address: %p\n", (void *)((char *)(block->ptr) + block->size));
    }
    else
    {
        printf("Data address: NULL\n");
    }

    printf("Heap address: %p\n", sbrk(0));

    // Check for inconsistencies
    t_block current = base;
    while (current)
    {
        // Check for adjacent free blocks
        if (current->free && current->next && current->next->free)
        {
            printf("Warning: Adjacent free blocks detected at %p and %p\n", (void *)current, (void *)current->next);
        }

        // Check for invalid block sizes
        if (current->size <= 0)
        {
            printf("Error: Invalid block size detected at %p\n", (void *)current);
        }

        current = current->next;
    }
}

void memory_usage()
{
    size_t total_allocated = 0;
    size_t total_free = 0;

    t_block current = base;
    while (current)
    {
        if (current->free)
        {
            total_free += current->size;
        }
        else
        {
            total_allocated += current->size;
        }
        current = current->next;
    }

    printf("\033[1;34mMemory Usage Report:\033[0m\n");
    printf("Total allocated memory: %zu bytes\n", total_allocated);
    printf("Total free memory: %zu bytes\n", total_free);
}

void log_function_call(const char *func_name, alloc_type type, void *ptr, size_t size)
{
    FILE *log_file = fopen("../log.txt", "a"); // Abre el archivo en modo "append" para agregar sin sobrescribir
    if (log_file == NULL)
    {
        perror("Error al abrir el archivo de log");
        return;
    }

    // Obtener la hora actual para incluirla en el log
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Escribir el nombre de la función y la fecha/hora en el archivo
    fprintf(log_file, "[%04d-%02d-%02d %02d:%02d:%02d] Llamada a %s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            func_name);

    // Agregar información adicional sobre la asignación
    fprintf(log_file, "Tipo de asignación: %d, Puntero: %p, Tamaño: %zu\n", type, ptr, size);

    fclose(log_file); // Cerrar el archivo después de escribir
}

// void add_log_entry(alloc_type type, void ptr, size_t size) {
//   t_log_entryentry = mmap(NULL, sizeof(t_log_entry), PROT_READ | PROT_WRITE,
//                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//   if (entry == MAP_FAILED) {
//     // Use write or fprintf to stderr to avoid using printf
//     fprintf(stderr, "Failed to allocate memory for log entry.\n");
//     return;
//   }
//   entry->type = type;
//   entry->ptr = ptr;
//   entry->size = size;
//   entry->total_allocated = total_allocated_memory;
//   entry->total_freed = total_freed_memory;
//   entry->next = log_head;
//     log_head = entry;
// }

// void clear_logs(void)
// {
//   t_log_entry entry = log_head;
//   while (entry) {
//     t_log_entrynext = entry->next;
//     munmap(entry, sizeof(t_log_entry));
//     entry = next;
//   }
//     log_head = NULL;
// }

double external_frag()
{
    t_block current = base;
    double total_free = 0;
    double total = 0;
    while (current)
    {
        if (current->free && current->size < maxs)
        {
            total_free += current->size;
        }

        total += current->size;
        current = current->next;
    }
    // printf("total frag free : %f\n",total_free);
    // printf("total mem alloc and free: %f\n",total);
    double frag = (total_free / total) * 100;
    printf("fragmentation: %f\n", frag);
    return frag;
}
