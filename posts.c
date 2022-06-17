#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include "hash.h"
#include "lista.h"
#include "abb.h"
#include "posts.h"

struct posts{
	size_t generador_id;
	hash_t * hash;
};

typedef struct dato_hash{
	char* texto;
	char* usuario;
	size_t cant_likes;
	abb_t * abb;
} dato_hash_t;

//Funciones auxiliares
dato_hash_t* _dato_hash_crear(const char* texto, const char * usuario);
void _dato_hash_destruir(dato_hash_t *dato_hash);
void _wrapper_dato_hash_destruir(void *dato_hash);
bool _enlistar_usuarios(const char* clave, void* dato, void* extra);
char *_id_a_string(size_t id);

/* *****************************************************************
 *                    PRIMITIVAS DE TDA USUARIOS
 * *****************************************************************/
posts_t* posts_crear(void){

	posts_t *posts = malloc(sizeof(posts_t));
	if(!posts) return NULL;
	
	hash_t *hash = hash_crear(_wrapper_dato_hash_destruir);
    	if(!hash){
		free(posts);
        	return NULL;
    	}

	posts->generador_id = 0;
	posts->hash = hash;
	return posts;
}

bool posts_pertenece(posts_t* posts,const char* clave){
	return hash_pertenece(posts->hash,clave);
}

ssize_t posts_publicar(posts_t *posts, const char* texto, const char *usuario){
	
	size_t id_actual = posts->generador_id;
	char *clave_id = _id_a_string(id_actual);
	if(!clave_id) return false;

	dato_hash_t* dato_hash = _dato_hash_crear(texto, usuario);
	if(!dato_hash){
		free(clave_id);
		return -1;
	}
	
	if(!hash_guardar(posts->hash, clave_id, dato_hash)){
		free(clave_id);
		_dato_hash_destruir(dato_hash);
		return -1;
	}

	posts->generador_id++;
	free(clave_id);
	return (ssize_t)id_actual;
}

bool posts_ver_siguiente_feed(posts_t* posts, ssize_t id, char** usuario, size_t* cant_likes, char** texto){
	
	char *clave_id = _id_a_string((size_t)id);
	if(!clave_id) return false;

	if(!posts_pertenece(posts, clave_id)){
		free(clave_id);
		return false;
	}

	dato_hash_t* dato_hash = hash_obtener(posts->hash,clave_id);	
	
	*usuario = dato_hash->usuario;		
	*cant_likes = dato_hash->cant_likes;
	*texto = dato_hash->texto;
	free(clave_id);
	return true;
}

bool posts_likear(posts_t* posts, const char *clave_id, const char* usuario){

	if(!posts_pertenece(posts, clave_id)) return false;
	
	dato_hash_t* dato_hash = hash_obtener(posts->hash, clave_id);
	
	if(!abb_pertenece(dato_hash->abb, usuario)){
		if(!abb_guardar(dato_hash->abb, usuario, NULL)) return false;
		dato_hash->cant_likes++;
	}
	
	return true;
}

lista_t* posts_mostrar_likes(posts_t* posts, const char *clave_id, size_t* cant_likes){
	
	if(!posts_pertenece(posts, clave_id)) return false;
	
	dato_hash_t* dato_hash = hash_obtener(posts->hash, clave_id);
	
	lista_t* lista = lista_crear();
	if(!lista) return NULL;

	*cant_likes = dato_hash->cant_likes;
	abb_in_order(dato_hash->abb, _enlistar_usuarios, lista); // Devolvemos lista con usuarios que dieron like.
	return lista; // Borrar lista desde la invocación
}

void posts_destruir(posts_t* posts){
    hash_destruir(posts->hash);
    free(posts);
}

/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/
dato_hash_t* _dato_hash_crear(const char* texto, const char * usuario){

	dato_hash_t* dato_hash = malloc(sizeof(dato_hash_t));
	if(!dato_hash) return NULL;
	
	abb_t* abb = abb_crear(strcmp,NULL);
	if(!abb){
		free(dato_hash);
		return NULL;
	}
	
	dato_hash->texto = strdup(texto);
	dato_hash->usuario = strdup(usuario);
	dato_hash->cant_likes = 0;
	dato_hash->abb = abb;
	return dato_hash;		
}

void _dato_hash_destruir(dato_hash_t* dato_hash){

	abb_destruir(dato_hash->abb);
	free(dato_hash->texto);
	free(dato_hash->usuario);
	free(dato_hash);
}

void _wrapper_dato_hash_destruir(void *dato_hash){
    _dato_hash_destruir((dato_hash_t*)dato_hash);
}

bool _enlistar_usuarios(const char* clave, void* dato, void* extra) {	
	
	lista_insertar_ultimo((lista_t*)extra, (void*)clave);
    return true;
}

char *_id_a_string(size_t id){
	
	size_t largo_clave;

	if(id > 0){
		double digitos = log10((double)id);
		largo_clave = (size_t)floor(digitos) + 1;
	} else {
		largo_clave = 1;
	}
	
	char *clave = malloc(sizeof(char)*(largo_clave + 1));
	if(!clave) return NULL;

	sprintf(clave, "%zu", id);
	return clave;
}