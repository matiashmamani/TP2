#ifndef _USUARIOS_H
#define _USUARIOS_H

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdbool.h>

struct usuarios;

typedef struct usuarios usuarios_t;

usuarios_t *usuarios_crear(void);

bool usuarios_guardar(usuarios_t *usuarios, const char *usuario, size_t pos);

bool usuarios_pertenece(usuarios_t *usuarios, const char *usuario);

bool usuarios_publicar(usuarios_t *usuarios, ssize_t id, const char *usuario);

ssize_t usuarios_ver_sig_feed(usuarios_t *usuarios, const char *usuario);

void usuarios_destruir(usuarios_t *usuarios);

#endif