#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "hash.h"
#include "lista.h"
#include "abb.h"
#include "posts.h"

struct posts{
	ssize_t generador_id;
	hash_t * hash;
};

//-1 no hay posteo

typedef struct dato_hash{
	char* texto;
	char* usuario;
	size_t cant_likes;
	abb_t * abb;
} dato_hash_t;


void _dato_hash_destruir(dato_hash_t *dato_hash);
void _wrapper_dato_hash_destruir(void *dato_hash);


//Funciones auxiliares
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

void  _dato_hash_destruir(dato_hash_t* dato_hash){

	abb_destruir(dato_hash->abb);
	free(dato_hash->texto);
	free(dato_hash->usuario);
	free(dato_hash);
}

bool concatenar_datos(const char* clave, void* dato, void* extra) {	
	
	lista_insertar_ultimo((lista_t*)extra, (void*)clave);
	
    	return true;
}

void _wrapper_dato_hash_destruir(void *dato_hash){
    _dato_hash_destruir((dato_hash_t*)dato_hash);
}

////
posts_t* posts_crear(){

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

ssize_t posts_publicar(posts_t* posts,const char* texto,const char * usuario){
	
	char clave[5];// = malloc(sizeof(char*5);
	
	sprintf(clave,"%zu",posts->generador_id);

	dato_hash_t* dato_hash = _dato_hash_crear(texto,usuario);
	
	if(!dato_hash) return -1; //Fallo publicar. 
	
	if(!hash_guardar(posts->hash,clave,dato_hash)){
		_dato_hash_destruir(dato_hash);
		return -1;
	}
	ssize_t id_actual = posts->generador_id;
	posts->generador_id++;	
	return id_actual;
}

bool posts_ver_siguiente_feed(posts_t* posts,ssize_t id,char** usuario, size_t* cant_likes,char** texto){
	
	char clave[5];
	sprintf(clave,"%zu",id);

	if(!hash_pertenece(posts->hash,clave)) return false;

	dato_hash_t* dato_hash = hash_obtener(posts->hash,clave);	
	
	*usuario = dato_hash->usuario;		
	*cant_likes = dato_hash->cant_likes;

	*texto = dato_hash->texto;
	
	return true;
}

bool posts_likear(posts_t* posts,ssize_t id, const char* usuario){
	
	char clave[5];
	sprintf(clave,"%zu",id);

	if(!hash_pertenece(posts->hash,clave)) return false;
	
	dato_hash_t* dato_hash = hash_obtener(posts->hash,clave);
	
	if(!abb_pertenece(dato_hash->abb,usuario)){
		
		if(!abb_guardar(dato_hash->abb,usuario,NULL)) return false;
		dato_hash->cant_likes++;
	}
	
	return true;
}

lista_t* posts_mostrar_likes(posts_t* posts,size_t id,size_t* cant_likes){

	char clave[5];
	sprintf(clave,"%zu",id);
	
	if(!hash_pertenece(posts->hash,clave)) return NULL;
	
	dato_hash_t* dato_hash = hash_obtener(posts->hash,clave);
	
	lista_t* lista = lista_crear();
	
	if(!lista) return NULL;

	*cant_likes = dato_hash->cant_likes;
	
	abb_in_order(dato_hash->abb, concatenar_datos, lista); //Devolvemos lista con usuarios que dieron like.
	
	return lista; //acordarse borra lista en el main 
}

void posts_destruir(posts_t* posts){
    hash_destruir(posts->hash);
    free(posts);
}

/*PRAR PROBAR - BORRAR*/
bool posts_ver(posts_t* posts,ssize_t id){
	
	char clave[5];
	sprintf(clave,"%zu",id);
	char* usuario;
	char* texto;
	size_t cant_likes;
	
	if(!hash_pertenece(posts->hash,clave)) return false;

	dato_hash_t* dato_hash = hash_obtener(posts->hash,clave);	
	
	usuario = dato_hash->usuario;		
	cant_likes = dato_hash->cant_likes;

	texto = dato_hash->texto;
	
	fprintf(stdout, "usuario %s cant_likes %zu  texto %s \n", usuario, cant_likes, texto);
	
	return true;
}



