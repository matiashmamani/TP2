#include <stdlib.h>
#include <stddef.h>
#include "lista.h"

typedef struct nodo {
	void* dato;
	struct nodo* prox;
} nodo_t;

struct lista {
	size_t largo;
	nodo_t* primero;
	nodo_t* ultimo;
};

struct lista_iter {
	lista_t* lista;
    nodo_t* actual;
    nodo_t* anterior;
};

// Función para uso interno
nodo_t *__crear_nodo(void *valor);

/* *****************************************************************
 *                    PRIMITIVAS DE LA LISTA ENLAZADA
 * *****************************************************************/
lista_t* lista_crear(void){
    
    lista_t *lista = malloc(sizeof(lista_t));
	
	if (!lista) return NULL;
	
	lista->largo = 0;
	lista->primero = NULL;
	lista->ultimo = NULL;
    
	return lista;
}

bool lista_esta_vacia(const lista_t* lista){
    
	return !lista->largo;
}

bool lista_insertar_primero(lista_t* lista, void* dato){
	
	nodo_t * nodo = malloc(sizeof(nodo_t));
	
	if(!nodo) return false;
	
	nodo->dato = dato;
	nodo->prox = NULL;
	
	if(!lista_esta_vacia(lista)){
		nodo->prox = lista->primero;
	} else {
		lista->ultimo = nodo;
	}
	
	lista->primero = nodo;
	lista->largo++;
	
	return true;
}

bool lista_insertar_ultimo(lista_t* lista, void* dato){
	
	nodo_t * nodo= malloc(sizeof(nodo_t));
	
	if(!nodo) return false;
	
	nodo->dato = dato;
	nodo->prox = NULL;
	
	if(!lista_esta_vacia(lista)){
		lista->ultimo->prox = nodo;
	} else {
		lista->primero = nodo;
	}
	
	lista->ultimo = nodo;
	lista->largo++;
	
	return true;
}

void* lista_borrar_primero(lista_t* lista){

	if(lista_esta_vacia(lista)){
		return NULL;
	}

	void * dato_aux = lista->primero->dato;
	nodo_t * nodo_aux = lista->primero;

	lista->primero = nodo_aux -> prox;
	free(nodo_aux);
	lista->largo--;

	if(lista_esta_vacia(lista)){
		lista->ultimo = NULL;
	}

	return dato_aux;
}

void* lista_ver_primero(const lista_t* lista){

    return (!lista_esta_vacia(lista)) ? lista->primero->dato : NULL;
}

void* lista_ver_ultimo(const lista_t* lista){

    return (!lista_esta_vacia(lista)) ? lista->ultimo->dato : NULL;
}

size_t lista_largo(const lista_t* lista){
    
	return lista->largo;
}

void lista_destruir(lista_t* lista, void (*destruir_dato)(void*)){
	
	void* dato_aux;

	while(!lista_esta_vacia(lista)){
	
		dato_aux = lista_borrar_primero(lista);

		if (destruir_dato != NULL){	
			destruir_dato(dato_aux);	
		}		
	}

	free(lista);
}

/* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/
void lista_iterar(lista_t* lista, bool visitar(void* dato, void* extra), void* extra){
		
	nodo_t *nodo = lista->primero;

	while(nodo != NULL && visitar(nodo->dato, extra)){
		nodo = nodo->prox;
	}
	
	return;
}

/* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/
lista_iter_t* lista_iter_crear(lista_t* lista){

    lista_iter_t* iter = (lista_iter_t*)malloc(sizeof(lista_iter_t));
    
    if(!iter) return NULL;

    iter->lista = lista;
    iter->actual = lista->primero;
    iter->anterior = NULL;
    
    return iter;
}

bool lista_iter_avanzar(lista_iter_t* iter){

    if(lista_iter_al_final(iter)) return false;

    iter->anterior = iter->actual;
    iter->actual = iter->actual->prox;

    return true;
}

void* lista_iter_ver_actual(const lista_iter_t* iter){

    if(lista_iter_al_final(iter)) return NULL;
    
    return iter->actual->dato;
}

bool lista_iter_al_final(const lista_iter_t* iter){

    return (!iter->actual);
}

void lista_iter_destruir(lista_iter_t* iter){
    
    free(iter);
}

bool lista_iter_insertar(lista_iter_t* iter, void* dato){
    
    nodo_t* nodo_nuevo = __crear_nodo(dato);

    if(!nodo_nuevo) return false;

    if(lista_iter_al_final(iter)){
        // Si inserto al final de la lista
        iter->lista->ultimo = nodo_nuevo;
    }

    nodo_nuevo->prox = iter->actual;
    iter->actual = nodo_nuevo;

    if(!iter->anterior){
        // Si inserto al principio de la lista
        iter->lista->primero = nodo_nuevo;
    } else {
        // Si inserto en cualquier otra posición
        iter->anterior->prox = nodo_nuevo;
    }

    iter->lista->largo++;

    return true;
}

void* lista_iter_borrar(lista_iter_t* iter){
 
    if(lista_iter_al_final(iter)) return NULL;

    void* dato = lista_iter_ver_actual(iter);
    nodo_t* nodo_a_destruir = iter->actual;

    iter->actual = nodo_a_destruir->prox;

    if(nodo_a_destruir == iter->lista->ultimo){
        // Si destruyo al final de la lista
        iter->lista->ultimo = iter->anterior;
    }

    if(nodo_a_destruir == iter->lista->primero){
        // Si destruyo al principio de la lista
        iter->lista->primero = nodo_a_destruir->prox;
    } else {
        // Si destruyo en cual otra posición
        iter->anterior->prox = iter->actual;    // al nuevo actual
    }

    iter->lista->largo--;
    free(nodo_a_destruir);

    return dato;
}

nodo_t* __crear_nodo(void* valor){

    nodo_t *nodo_nuevo = malloc(sizeof(nodo_t));
    
    if(!nodo_nuevo) return NULL;

    nodo_nuevo->dato = valor;
    nodo_nuevo->prox = NULL;

    return nodo_nuevo;
}