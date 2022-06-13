#define _POSIX_C_SOURCE 200809L
#define MIN_ARGS 2
#define MODO_LECTURA "r"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usuarios.h"
#include "sesion.h"
#include "posts.h"

enum status{
    OK_HOLA = 0,
    ERROR_USUARIO_YA_LOGGEADO,
    ERROR_USUARIO_NO_EXISTE,
    OK_ADIOS,
    ERROR_USUARIO_NO_LOGGEADO
};

const char *status_msj[] = {
    "Hola", // "Hola <usuario>"
    "Error: Ya habia un usuario loggeado",
    "Error: usuario no existente",
    "Adios",
    "Error: no habia usuario loggeado"
};

bool crear_TDAs(usuarios_t** usuarios, sesion_t** sesion, posts_t** posts);
void destruir_TDAs(usuarios_t *usuarios, sesion_t *sesion, posts_t *posts);
bool cargar_archivo(usuarios_t *usuarios, const char *nombre_archivo);
bool algogram(usuarios_t *usuarios, sesion_t *sesion, posts_t *posts);
void chomp(char *str);
void login(usuarios_t *usuarios, sesion_t *sesion);
void logout(sesion_t *sesion);

int main(int argc, char *argv[]){

    if(argc < MIN_ARGS){
        return EXIT_FAILURE;
    }

    usuarios_t* usuarios;
    sesion_t* sesion;
    posts_t* posts;
    char* nombre_archivo = argv[1];

    if(!crear_TDAs(&usuarios, &sesion, &posts)){
        return EXIT_FAILURE;
    }
    
    if(!cargar_archivo(usuarios, nombre_archivo)){
        destruir_TDAs(usuarios, sesion, posts);
        return EXIT_FAILURE;
    }

    if(!algogram(usuarios, sesion, posts)){
        destruir_TDAs(usuarios, sesion, posts);
        return EXIT_FAILURE;
    }

    destruir_TDAs(usuarios, sesion, posts);
    
    return EXIT_SUCCESS;
}

bool crear_TDAs(usuarios_t** usuarios, sesion_t** sesion, posts_t** posts){
    *usuarios = usuarios_crear();
    if(!(*usuarios)) return false;

    *sesion = sesion_crear();
    if(!(*sesion)){
        usuarios_destruir(*usuarios);
        return false;
    }

    *posts = posts_crear();
    if(!(*posts)){
        sesion_destruir(*sesion);
        usuarios_destruir(*usuarios);
        return false;
    }

    return true;
}

void destruir_TDAs(usuarios_t *usuarios, sesion_t *sesion, posts_t *posts){

    usuarios_destruir(usuarios);
    sesion_destruir(sesion);
    posts_destruir(posts);
}

bool cargar_archivo(usuarios_t *usuarios, const char *nombre_archivo){

    FILE *archivo = fopen(nombre_archivo, MODO_LECTURA);
    if(!archivo) return false;

    char *usuario = NULL;
    size_t tam = 0;
    size_t pos = 0;
    
    while(getline(&usuario, &tam, archivo) != EOF){

        chomp(usuario);

        if(!usuarios_guardar(usuarios, usuario, pos)){
            free(usuario);
            fclose(archivo);
            return false;
        }
        pos++;
    }

    free(usuario);
    fclose(archivo);
    return true;
}

bool algogram(usuarios_t *usuarios, sesion_t *sesion, posts_t *posts){

    char *linea = NULL;
    size_t tam = 0;
    //status_t status;
    
    while(getline(&linea, &tam, stdin) != EOF){
        
        chomp(linea);

        if(!strcmp(linea, "login")){
            login(usuarios, sesion);
        }

        if(!strcmp(linea, "logout")){
            logout(sesion);
        }
    }

    free(linea);
    return true;
}

void chomp(char *str){
    size_t largo = strlen(str);
    if(!largo) return;

    if(str[largo - 1] == '\n'){
        str[largo - 1] = '\0';
    }
}

void login(usuarios_t *usuarios, sesion_t *sesion){

    char *usuario = NULL;
    size_t tam = 0;
    
    getline(&usuario, &tam, stdin);   
    chomp(usuario);
    
    if(!usuarios_pertenece(usuarios, usuario)){
        fprintf(stdout, "%s\n", status_msj[ERROR_USUARIO_NO_EXISTE]);
        free(usuario);
        return;
    }

    if(!sesion_login(sesion, usuario)){
        fprintf(stdout, "%s\n", status_msj[ERROR_USUARIO_YA_LOGGEADO]);
        free(usuario);
        return;
    }

    fprintf(stdout, "%s %s\n", status_msj[OK_HOLA], usuario);
    free(usuario);
}

void logout(sesion_t *sesion){

    if(!sesion_esta_loggeado(sesion)){
        fprintf(stdout, "%s\n", status_msj[ERROR_USUARIO_NO_LOGGEADO]);
        return;
    }

    sesion_logout(sesion);
    fprintf(stdout, "%s\n", status_msj[OK_ADIOS]);
}