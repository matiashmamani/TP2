#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>
#include "sesion.h"

struct sesion {
    char* usuario;
};

sesion_t *sesion_crear(void){
    sesion_t *sesion = malloc(sizeof(sesion_t));
    if(!sesion) return NULL;
    sesion->usuario = NULL;

    return sesion;
}

bool sesion_login(sesion_t *sesion, const char *usuario){
    if(sesion_esta_loggeado(sesion)) return false;

    char* usuario_dup = strdup(usuario);
    if(!usuario_dup) return false;
    sesion->usuario = usuario_dup;

    return true;
}

void sesion_logout(sesion_t *sesion){
    free(sesion->usuario);
    sesion->usuario = NULL;
}

bool sesion_esta_loggeado(sesion_t *sesion){
    return sesion->usuario;
}

char* sesion_obtener_usuario(sesion_t *sesion){
    if(!sesion_esta_loggeado(sesion)) return NULL;
    return sesion->usuario;
}

void sesion_destruir(sesion_t *sesion){
    if(sesion_esta_loggeado(sesion)){
        sesion_logout(sesion);
    }
    free(sesion);
}