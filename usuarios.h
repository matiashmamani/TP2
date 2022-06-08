#ifndef _USUARIOS_H
#define _USUARIOS_H

#include <stdbool.h>
#include <stddef.h>

struct usuarios;

typedef struct usuarios usuarios_t;

usuarios_t *usuarios_crear(void);

bool usuarios_cargar_archivo(usuarios_t *usuarios, const char *nombre_archivo);

bool usuarios_pertenece(usuarios_t *usuarios, const char *usuario);

bool usuarios_publicar(usuarios_t *usuarios, ssize_t id, const char *usuario);

ssize_t usuarios_ver_sig_feed(usuarios_t *usuarios, const char *usuario);

void usuarios_destruir(usuarios_t *usuarios);

#endif