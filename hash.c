#define _POSIX_C_SOURCE 200809L
#define CAPACIDAD_INICIAL 100
#define FACTOR_CARGA_LIMITE 2.5
#define FACTOR_CARGA_DESEADO 0.8

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include "hash.h"
#include "lista.h"

typedef struct campo_hash {
	char* clave;
	void* dato;
} campo_hash_t;

struct hash {
	lista_t** tabla;
	size_t capacidad;
	size_t cantidad;
	hash_destruir_dato_t destruir_dato;
};

struct hash_iter {
	lista_iter_t* lista_iter;
	size_t pos;
	const hash_t* hash;
};

// Funcion de Hashing
// https://www.programmingalgorithms.com/algorithm/dek-hash/c/
unsigned int funcion_hash(const char* clave) {
	
	unsigned int length = (unsigned int)strlen(clave);
	unsigned int hash = length;
	unsigned int i = 0;

	for (i = 0; i < length; clave++, i++){
		hash = ((hash << 5) ^ (hash >> 27)) ^ (unsigned int)(*clave);
	}

	return hash;
}

// Función para uso interno
campo_hash_t* __hash_crear_campo(const char* clave, void* dato);

void __hash_destruir_campo(campo_hash_t* campo_hash);

void __hash_destruir_tabla(lista_t** tabla, size_t capacidad, bool destruir_campo, hash_destruir_dato_t destruir_dato);

campo_hash_t* __hash_buscar_campo(const hash_t* hash, const char* clave, bool borrar);

bool __hash_redimensionar(hash_t* hash_viejo, size_t capacidad_nueva);

bool __hash_evaluar_redimension(hash_t* hash);

bool __hash_iter_buscar_pos_valida(hash_iter_t* iter, size_t pos_inicial, bool crear_iter);

/* *****************************************************************
 *                    PRIMITIVAS DEL HASH
 * *****************************************************************/

hash_t* hash_crear(hash_destruir_dato_t destruir_dato){
	    
	hash_t* hash = malloc(sizeof(hash_t));
	
	if(!hash) return NULL;

	lista_t** tabla = malloc(sizeof(lista_t*)*CAPACIDAD_INICIAL);

	if(!tabla){
		free(hash);
		return NULL;
	}
	
	hash->tabla = tabla;
	hash->capacidad = CAPACIDAD_INICIAL;
	hash->cantidad = 0;
	hash->destruir_dato = destruir_dato;

	for(size_t i = 0; i < hash->capacidad; i++){
		hash->tabla[i] = NULL;
	}
    
	return hash;
}

bool hash_guardar(hash_t* hash, const char *clave, void *dato){

	campo_hash_t* campo_hash = __hash_buscar_campo(hash, clave, false);

	if(campo_hash){

		if(hash->destruir_dato){
			hash->destruir_dato(campo_hash->dato);
		}
		
		campo_hash->dato = dato;

		return true;
	}

	campo_hash_t* campo_hash_nuevo = __hash_crear_campo(clave, dato);
	if(!campo_hash_nuevo){
		return false;
	}

	size_t i = funcion_hash(clave) % hash->capacidad;

	lista_t* lista_nueva = NULL;

	if(!hash->tabla[i]){

		lista_nueva = lista_crear();
		
		if(!lista_nueva){
			__hash_destruir_campo(campo_hash_nuevo);
			return false;
		}

		hash->tabla[i] = lista_nueva;
	}

	if(!lista_insertar_ultimo(hash->tabla[i], campo_hash_nuevo)){

		if(lista_nueva) lista_destruir(lista_nueva, NULL);
		__hash_destruir_campo(campo_hash_nuevo);
		return false;
	}

	hash->cantidad++;

	__hash_evaluar_redimension(hash);

	return true;
}

void* hash_borrar(hash_t* hash, const char* clave){

	campo_hash_t* campo_hash = __hash_buscar_campo(hash, clave, true);
	if(!campo_hash) return NULL;

	void* dato = campo_hash->dato;

	__hash_destruir_campo(campo_hash);
	hash->cantidad--;

	return dato;
}

void* hash_obtener(const hash_t* hash, const char* clave){

	campo_hash_t* campo_hash = __hash_buscar_campo(hash, clave, false);
	return (!campo_hash)? NULL : campo_hash->dato;
}

bool hash_pertenece(const hash_t* hash, const char* clave){

	return __hash_buscar_campo(hash, clave, false);
}

void hash_destruir(hash_t* hash){

	__hash_destruir_tabla(hash->tabla, hash->capacidad, true, hash->destruir_dato);
	free(hash);
}

size_t hash_cantidad(const hash_t* hash){

	return hash->cantidad;
}

/* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/
hash_iter_t* hash_iter_crear(const hash_t* hash){

	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if(!iter) return NULL;

	iter->hash = hash;

	if(!__hash_iter_buscar_pos_valida(iter, 0, true)) return NULL;

	return iter;
}

bool hash_iter_avanzar(hash_iter_t* iter){

	if(iter->pos == iter->hash->capacidad - 1 && !iter->lista_iter) return false;

	lista_iter_avanzar(iter->lista_iter);

	if(lista_iter_al_final(iter->lista_iter)){

		lista_iter_destruir(iter->lista_iter);
		return __hash_iter_buscar_pos_valida(iter, iter->pos + 1, false);
	}

	return true;
}	

bool hash_iter_al_final(const hash_iter_t* iter){
	
	return (iter->pos == iter->hash->capacidad - 1) && (!iter->lista_iter);
}

const char* hash_iter_ver_actual(const hash_iter_t* iter){

	if(hash_iter_al_final(iter)) return NULL;

	campo_hash_t* campo_hash = lista_iter_ver_actual(iter->lista_iter);
	return campo_hash->clave;
}

void hash_iter_destruir(hash_iter_t* iter){
	
	if(iter->lista_iter) lista_iter_destruir(iter->lista_iter);

	free(iter);
}

/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

campo_hash_t* __hash_crear_campo(const char* clave, void* dato){

    campo_hash_t *campo_hash_nuevo = malloc(sizeof(campo_hash_t));
    if(!campo_hash_nuevo) return NULL;

	char* clave_duplicada = strdup(clave);
	if(!clave_duplicada){
		free(campo_hash_nuevo);
		return NULL;
	}

	campo_hash_nuevo->clave = clave_duplicada;
    campo_hash_nuevo->dato = dato;

    return campo_hash_nuevo;
}

void __hash_destruir_campo(campo_hash_t* campo_hash){

	free(campo_hash->clave);		// Liberar clave duplicada con strdup(...)
	free(campo_hash);
}

void __hash_destruir_tabla(lista_t** tabla, size_t capacidad, bool destruir_campo, hash_destruir_dato_t destruir_dato){
	
	for(size_t i = 0; i < capacidad; i++){
		
		if(tabla[i]){

			while(!lista_esta_vacia(tabla[i])){

				campo_hash_t* campo_hash = lista_borrar_primero(tabla[i]);

				if(destruir_campo){
					if(destruir_dato) destruir_dato(campo_hash->dato);
					__hash_destruir_campo(campo_hash);
				}			
			}

			lista_destruir(tabla[i], NULL);
		}
	}

	free(tabla);
}

campo_hash_t* __hash_buscar_campo(const hash_t* hash, const char* clave, bool borrar){
	
	size_t i = funcion_hash(clave) % hash->capacidad;
	
	if(!hash->tabla[i]) return NULL;

	lista_iter_t* iter = lista_iter_crear(hash->tabla[i]);
	if(!iter) return NULL;

	campo_hash_t* campo_hash_buscado;
	bool clave_encontrada = false;

	while(!lista_iter_al_final(iter)){

		campo_hash_buscado = (campo_hash_t*) lista_iter_ver_actual(iter);
		clave_encontrada = !strcmp(clave, campo_hash_buscado->clave)? true : false;
		
		if(clave_encontrada) break;
		lista_iter_avanzar(iter);
	}

	if(!clave_encontrada){

		lista_iter_destruir(iter);
		return NULL;
	}

	if(borrar && clave_encontrada){

		lista_iter_borrar(iter);
		lista_iter_destruir(iter);

		if(lista_esta_vacia(hash->tabla[i])){
			// Si borré el ultimo nodo, entonces borro lista
			lista_destruir(hash->tabla[i], NULL);
			hash->tabla[i] = NULL;
		}

	} else {

		lista_iter_destruir(iter);
	}	

	return campo_hash_buscado;
}

bool __hash_redimensionar(hash_t* hash, size_t capacidad_nueva){
	
	lista_t** tabla_nueva = malloc(sizeof(lista_t*)*capacidad_nueva);
	if(!tabla_nueva) return false;

	for(size_t i = 0; i < capacidad_nueva; i++){
		tabla_nueva[i] = NULL;
	}

	hash_iter_t* hash_iter = hash_iter_crear(hash);
	if(!hash_iter){
		free(tabla_nueva);
		return false;
	}

	campo_hash_t* campo_hash;

	while (!hash_iter_al_final(hash_iter)){

		campo_hash = lista_iter_ver_actual(hash_iter->lista_iter);

		size_t i = funcion_hash(campo_hash->clave) % capacidad_nueva;

		if (!tabla_nueva[i]){

			tabla_nueva[i] = lista_crear();
			
			if (!tabla_nueva[i]){
				__hash_destruir_tabla(tabla_nueva, capacidad_nueva, false, NULL);
				return false;	
			}
		}

		lista_insertar_ultimo(tabla_nueva[i], campo_hash);

		hash_iter_avanzar(hash_iter);
	}

	__hash_destruir_tabla(hash->tabla, hash->capacidad, false, NULL);
	hash_iter_destruir(hash_iter);

	hash->tabla = tabla_nueva;
	hash->capacidad = capacidad_nueva;

	return true;
}

bool __hash_evaluar_redimension(hash_t* hash){
	
	if((float)hash->cantidad / (float)hash->capacidad >= FACTOR_CARGA_LIMITE){

		double cap_nueva_double = floor((double)hash->cantidad / FACTOR_CARGA_DESEADO);
		size_t capacidad_nueva = (size_t)cap_nueva_double;

		return __hash_redimensionar(hash, capacidad_nueva);
	}

	return false;
}

bool __hash_iter_buscar_pos_valida(hash_iter_t* iter, size_t pos_inicial, bool crear_iter){

	for(size_t i = pos_inicial; i < iter->hash->capacidad; i++){

		if(iter->hash->tabla[i]){

			iter->pos = i;
			iter->lista_iter = lista_iter_crear(iter->hash->tabla[i]);

			if(!iter->lista_iter){

				if(crear_iter) free(iter);
				return false;
			}
			
			return true;
		}
	}

    iter->pos = iter->hash->capacidad - 1;
    iter->lista_iter = NULL;

    return crear_iter;
}