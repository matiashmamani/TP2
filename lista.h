#ifndef _LISTA_H
#define _LISTA_H

#include <stdbool.h>
#include <stddef.h>

struct lista;
typedef struct lista lista_t;

struct lista_iter;
typedef struct lista_iter lista_iter_t;

/* *****************************************************************
 *                    PRIMITIVAS DE LA LISTA ENLAZADA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t* lista_crear(void);

// Devuelve verdadero si la lista no tiene elementos encolados, falso en caso contrario.
// Pre: la cola fue creada.
bool lista_esta_vacia(const lista_t* lista);

// Inserta un nuevo elemento al principio. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se insertó un nuevo elemento a la lista, el valor se encuentra al principio
// de la misma.
bool lista_insertar_primero(lista_t* lista, void* dato);

// Inserta un nuevo elemento al final. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se insertó un nuevo elemento a la lista, el valor se encuentra al final
// de la misma.
bool lista_insertar_ultimo(lista_t* lista, void* dato);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos si la lista no estaba vacía.
void* lista_borrar_primero(lista_t* lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void* lista_ver_primero(const lista_t* lista);

// Obtiene el valor del último elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del último, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el último elemento de la lista, cuando no está vacía.
void* lista_ver_ultimo(const lista_t* lista);

// Devuelve el largo de la lista
// Pre: la lista fue creada
size_t lista_largo(const lista_t* lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t* lista, void (*destruir_dato)(void*));

/* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.

// Itera cada elemento de la lista. Si visitar devuelve false, la lista detiene su iteración.
// Pre: la lista fue creada. visitar se llamará para cada elemento de la lista y debe ser no NULL. 
// El párametro extra se utilizará en la función visitar para pasar valores entre cada iteración.
void lista_iterar(lista_t* lista, bool visitar(void* dato, void* extra), void* extra);

/* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

// Crea un iterador externo para una lista.
// Pre: la lista fue creada.
// Post: Si la lista tiene elementos se posiciona en el primero, 
// sino (si la lista esta vacia) se posiciona en NULL.
lista_iter_t* lista_iter_crear(lista_t* lista);

// Devuelve verdadero si se pudo avanzar al próximo lugar, 
// o falso en caso contrario (si ya se encuentra al final de la lista).
// Pre: el iterador externo fue creado.
// Post: si no estaba al final, se avanzó una posición en la lista.
bool lista_iter_avanzar(lista_iter_t* iter);

// Si la lista tiene elementos, se devuelve el elemento donde se encuentra posicionado 
// el iterador externo. En caso contrario (lista vacia o iterador posicionado al final),
// se devuelve NULL.
// Pre: el iterador externo fue creado.
void* lista_iter_ver_actual(const lista_iter_t* iter);

// Devuelve verdadero si el iterador externo se encuentra posicionado
// al final de la lista, o falso en caso contrario.
// Pre: el iterador externo fue creado.
bool lista_iter_al_final(const lista_iter_t* iter);

// Destruye el iterador externo.
// Pre: el iterador externo fue creado.
// Post: se destruyó el iterador externo.
void lista_iter_destruir(lista_iter_t* iter);

// Agrega un nuevo elemento donde se encuentra posicionado el iterador externo.
// Pre: el iterador externo fue creado.
// Post: se insertó un nuevo elemento a la lista.
bool lista_iter_insertar(lista_iter_t* iter, void* dato);

// Si la lista tiene elementos, se remueve el elemento donde se encuentra posicionado 
// el iterador externo y se devuelve su valor. En caso contrario (lista vacia o iterador 
// posicionado al final), se devuelve NULL.
// Pre: el iterador externo fue creado.
// Post: si la lista tenía elementos, se eliminó un elemento de la lista.
void* lista_iter_borrar(lista_iter_t* iter);

#endif  // _LISTA_H