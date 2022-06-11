#ifndef _ABB_H
#define _ABB_H

#include <stddef.h>
#include <stdbool.h>

struct abb;
typedef struct abb abb_t;
typedef struct abb_iter abb_iter_t;

// Tipo de función para comparar dentro del ABB
typedef int (*abb_comparar_clave_t) (const char *, const char *);
// Tipo de función para destruir dato
typedef void (*abb_destruir_dato_t) (void *);

/* Crea el ABB
 */
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

/* Guarda un elemento en el ABB, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura ABB fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

/* Borra un elemento del ABB y devuelve el dato asociado. Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura ABB fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *abb_borrar(abb_t *arbol, const char *clave);

/* Obtiene el valor de un elemento del ABB, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura ABB fue inicializada
 */
void *abb_obtener(const abb_t *arbol, const char *clave);

/* Determina si clave pertenece o no al ABB.
 * Pre: La estructura ABB fue inicializada
 */
bool abb_pertenece(const abb_t *arbol, const char *clave);

/* Devuelve la cantidad de elementos del ABB.
 * Pre: La estructura ABB fue inicializada
 */
size_t abb_cantidad(const abb_t *arbol);

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura ABB fue inicializada
 * Post: La estructura ABB fue destruida
 */
void abb_destruir(abb_t *arbol);

//Iterador interno
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

//Iterador Externo
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);
bool abb_iter_in_avanzar(abb_iter_t *iter);
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);
bool abb_iter_in_al_final(const abb_iter_t *iter);
void abb_iter_in_destruir(abb_iter_t* iter);


#endif  // _ABB_H
