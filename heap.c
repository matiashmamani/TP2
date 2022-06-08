#define CAPACIDAD_INICIAL 20
#define FACTOR 2
#define POS_HIJO_IZQ(i) (2*i)+1
#define POS_HIJO_DER(i) (2*i)+2
#define POS_PADRE(i) (i-1)/2

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "heap.h"

struct heap {
    void **datos;
    size_t cantidad;
    size_t capacidad;
    cmp_func_t cmp;
};

// Funciones para uso interno

bool __heap_redimensionar(heap_t *heap, size_t capacidad_nueva);

void __swap(void **x, void **y);

void __heapify(void **datos, size_t n, cmp_func_t cmp);

void __upheap(void **datos, size_t pos, cmp_func_t cmp);

void __downheap(void **datos, size_t pos, size_t n, cmp_func_t cmp);

/* *****************************************************************
 *                    PRIMITIVAS DEL HEAP
 * *****************************************************************/
heap_t *heap_crear(cmp_func_t cmp){

    heap_t *heap = malloc(sizeof(heap_t));
    if(!heap) return NULL;

    heap->datos = malloc(CAPACIDAD_INICIAL * sizeof(void*));
    if(!heap->datos){
        free(heap);
        return NULL;
    }

    heap->cantidad = 0;
    heap->capacidad = CAPACIDAD_INICIAL;
    heap->cmp = cmp;

    return heap;
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){

    if(!n) return NULL;

    heap_t *heap = malloc(sizeof(heap_t));
    if(!heap) return NULL;

    heap->datos = malloc(2 * n * sizeof(void*));
    if(!heap->datos){
        free(heap);
        return NULL;
    }

    for(size_t i = 0; i < n; i++){
        heap->datos[i] = arreglo[i];
    }

    __heapify(heap->datos, n, cmp);

    heap->cantidad = n;
    heap->capacidad = 2 * n;
    heap->cmp = cmp;

    return heap;
}

void heap_destruir(heap_t *heap, void (*destruir_elemento)(void *e)){

    if(destruir_elemento){
        for(size_t i = 0; i < heap->cantidad; i++){
            destruir_elemento(heap->datos[i]);
        }
    }

    free(heap->datos);
    free(heap);
}

size_t heap_cantidad(const heap_t *heap){

    return heap->cantidad;
}

bool heap_esta_vacio(const heap_t *heap){

    return !heap_cantidad(heap);
}

bool heap_encolar(heap_t *heap, void *elem){

    if(heap->cantidad == heap->capacidad){
        if(!__heap_redimensionar(heap, heap->capacidad * FACTOR)){
            return false;
        }
    }

    heap->datos[heap->cantidad] = elem;
    __upheap(heap->datos, heap->cantidad, heap->cmp);
    heap->cantidad++;

    return true;
}

void *heap_ver_max(const heap_t *heap){

	if(heap_esta_vacio(heap)) return NULL;
	
	return heap->datos[0];
}

void *heap_desencolar(heap_t *heap){

    if(heap_esta_vacio(heap)) return NULL;

    void *max = heap_ver_max(heap);

    __swap(&heap->datos[0], &heap->datos[heap->cantidad - 1]);
    heap->cantidad--;
    __downheap(heap->datos, 0, heap->cantidad, heap->cmp);

    if(heap->cantidad <= heap->capacidad / (FACTOR*FACTOR) && CAPACIDAD_INICIAL <= heap->capacidad / FACTOR){
        __heap_redimensionar(heap, heap->capacidad / FACTOR);
    }

    return max;
}

/* *****************************************************************
*                          HEAPSORT
 * *****************************************************************/

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){

    __heapify(elementos, cant, cmp);

    while(cant >= 1){
        __swap(&elementos[0], &elementos[cant - 1]);
        cant--;
        __downheap(elementos, 0, cant, cmp);
    }
}

/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

bool __heap_redimensionar(heap_t *heap, size_t capacidad_nueva){

    void **datos_nuevo = realloc(heap->datos, capacidad_nueva * sizeof(void*));

    // Cuando capacidad_nueva es 0, es correcto si se devuelve NULL.
    // En toda otra situación significa que falló el realloc.
    if (capacidad_nueva > 0 && !datos_nuevo) return false;

    heap->datos = datos_nuevo;
    heap->capacidad = capacidad_nueva;

    return true;
}

void __swap(void **x, void **y) {
    
    void *aux = *x;
    *x = *y;
    *y = aux;
}

void __heapify(void **datos, size_t n, cmp_func_t cmp){

    for(size_t i = n; i > 0; i--){
        __downheap(datos, i - 1, n, cmp);
    }
}

void __upheap(void **datos, size_t pos, cmp_func_t cmp){

    // Es el MAX, no tiene padre
    if(!pos) return;

    size_t pos_padre = POS_PADRE(pos);

    // Llegué a la pos correcta, corto el upheap
    if(cmp(datos[pos], datos[pos_padre]) <= 0) return;

    // Sino, hago swap y sigo haciendo upheap
    __swap(&datos[pos], &datos[pos_padre]);

    __upheap(datos, pos_padre, cmp);
}

void __downheap(void **datos, size_t pos, size_t n, cmp_func_t cmp){
    
    if(pos >= n) return;

    size_t pos_hijo_izq = POS_HIJO_IZQ(pos);
    size_t pos_hijo_der = POS_HIJO_DER(pos);

    size_t pos_max = pos;

    if(pos_hijo_izq < n && cmp(datos[pos_hijo_izq], datos[pos_max]) > 0){
        pos_max = pos_hijo_izq;
    }

    if(pos_hijo_der < n && cmp(datos[pos_hijo_der], datos[pos_max]) > 0){
        pos_max = pos_hijo_der;
    }

    // Si no hay cambio en la pos en el vector,
    // llegué a la pos correcta y corto el downheap
    if(pos == pos_max) return;

    // Sino, hago swap y sigo haciendo downheap
    __swap(&datos[pos], &datos[pos_max]);

    __downheap(datos, pos_max, n, cmp);
}