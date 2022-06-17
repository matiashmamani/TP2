#ifndef _POSTS_H
#define _POSTS_H

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdbool.h>
#include "lista.h"

typedef struct posts posts_t;

posts_t* posts_crear(void);

bool posts_pertenece(posts_t* posts, const char* clave);

ssize_t posts_publicar(posts_t* posts, const char* texto, const char * usuario);

bool posts_ver_siguiente_feed(posts_t* posts, ssize_t id, char** usuario, size_t* cant_likes, char** texto);

bool posts_likear(posts_t* posts, const char *clave_id, const char* usuario);

lista_t* posts_mostrar_likes(posts_t* posts, const char *clave_id, size_t* cant_likes);

void posts_destruir(posts_t* posts);

#endif