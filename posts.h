#ifndef _POSTS_H
#define _POSTS_H

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdbool.h>
#include "lista.h"

typedef struct posts posts_t;

//Primitivas 
posts_t* posts_crear();

ssize_t posts_publicar(posts_t* posts,const char* texto,const char * usuario);

bool posts_ver_siguiente_feed(posts_t* posts,ssize_t id,char** usuario, size_t* cant_likes,char** texto);

bool posts_likear(posts_t* posts,ssize_t id, const char* usuario);

lista_t* posts_mostrar_likes(posts_t* posts,size_t id,size_t* cant_likes);

void posts_destruir(posts_t* posts);

bool posts_ver(posts_t* posts,ssize_t id); //BORRAR funcion de prueba

#endif
