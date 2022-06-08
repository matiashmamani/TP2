#ifndef _SESION_H
#define _SESION_H

#include <stdbool.h>
#include <stddef.h>

struct sesion;

typedef struct sesion sesion_t;

sesion_t *sesion_crear(void);

bool sesion_login(sesion_t *sesion, const char *usuario);

void sesion_logout(sesion_t *sesion);

bool sesion_esta_loggeado(sesion_t *sesion);

char* sesion_obtener_usuario(sesion_t *sesion);

void sesion_destruir(sesion_t *sesion);

#endif