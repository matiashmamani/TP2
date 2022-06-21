#define _POSIX_C_SOURCE 200809L
#define MIN_ARGS 2
#define MODO_LECTURA "r"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "usuarios.h"
#include "sesion.h"
#include "posts.h"

// Indices de Mensajes
enum status{
    OK_HOLA = 0,
    ERROR_USUARIO_YA_LOGGEADO,
    ERROR_USUARIO_NO_EXISTE,
    OK_ADIOS,
    ERROR_USUARIO_NO_LOGGEADO,
    OK_POST,
    OK_LIKEAR,
    ERROR_LIKEAR,
    ERROR_MOSTRAR_LIKES,
    ERROR_SIG_FEED    
};

// Mensajes
const char *status_msj[] = {
    "Hola",
    "Error: Ya habia un usuario loggeado",
    "Error: usuario no existente",
    "Adios",
    "Error: no habia usuario loggeado",
    "Post publicado",
    "Post likeado",
    "Error: Usuario no loggeado o Post inexistente",
    "Error: Post inexistente o sin likes",
    "Usuario no loggeado o no hay mas posts para ver"
};

// Funciones Principales
bool crear_TDAs(usuarios_t** usuarios, sesion_t** sesion, posts_t** posts);
bool cargar_archivo(usuarios_t *usuarios, const char *nombre_archivo);
bool algogram(usuarios_t *usuarios, sesion_t *sesion, posts_t *posts);
void destruir_TDAs(usuarios_t *usuarios, sesion_t *sesion, posts_t *posts);

// Funciones Secundarias
void chomp(char *str);
void login(usuarios_t *usuarios, sesion_t *sesion);
void logout(sesion_t *sesion);
void publicar(posts_t* posts, sesion_t *sesion,usuarios_t *usuarios);
void ver_siguiente_feed(posts_t * posts, sesion_t * sesion,usuarios_t *usuarios);
void likear_post(posts_t * posts, sesion_t * sesion,usuarios_t *usuarios);
void mostrar_likes(posts_t * posts, sesion_t * sesion,usuarios_t *usuarios);

/* *****************************************************************
 *                    PROGRAMA PRINCIPAL
 * *****************************************************************/
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

/* *****************************************************************
 *                    FUNCIONES PRINCIPALES
 * *****************************************************************/
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
    
    while(getline(&linea, &tam, stdin) != EOF){
        
        chomp(linea);

        if(!strcmp(linea, "login")){
            login(usuarios, sesion);
        }
        if(!strcmp(linea, "logout")){
            logout(sesion);
        }
        if(!strcmp(linea, "publicar")){
            publicar(posts, sesion, usuarios);
        }
        if(!strcmp(linea, "ver_siguiente_feed")){
            ver_siguiente_feed(posts, sesion, usuarios);
        }
        if(!strcmp(linea, "likear_post")){
            likear_post(posts, sesion, usuarios);
        }
        if(!strcmp(linea, "mostrar_likes")){
            mostrar_likes(posts, sesion, usuarios);
        }
    }

    free(linea);
    return true;
}

void destruir_TDAs(usuarios_t *usuarios, sesion_t *sesion, posts_t *posts){

    usuarios_destruir(usuarios);
    sesion_destruir(sesion);
    posts_destruir(posts);
}

/* *****************************************************************
 *                    FUNCIONES SECUNDARIAS
 * *****************************************************************/
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
    if(!getline(&usuario, &tam, stdin)) return;
    chomp(usuario);

    if(!sesion_login(sesion, usuario)){
        fprintf(stdout, "%s\n", status_msj[ERROR_USUARIO_YA_LOGGEADO]);
        free(usuario);
        return;
    }
    
    if(!usuarios_pertenece(usuarios, usuario)){
        fprintf(stdout, "%s\n", status_msj[ERROR_USUARIO_NO_EXISTE]);
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


void publicar(posts_t* posts, sesion_t *sesion, usuarios_t * usuarios){

	char *texto = NULL;
	size_t tam = 0;	
	if(!getline(&texto, &tam, stdin)) return;   
	chomp(texto);

    if(!sesion_esta_loggeado(sesion)){
        fprintf(stdout, "%s\n", status_msj[ERROR_USUARIO_NO_LOGGEADO]);
        free(texto);
        return;
    }

    char *usuario = sesion_obtener_usuario(sesion);
	ssize_t id = posts_publicar(posts, texto, usuario);

    // Devuelve -1 si no pudo publicar
    if(id == -1){
        free(texto);
        return;
    }	
	
	if(usuarios_publicar(usuarios, id, usuario)){
		fprintf(stdout, "%s\n", status_msj[OK_POST]);
    }
	//No dice nada el enunciado pero si no puede generar bien el hash debería decir indicar error de alguna forma.	
	
	free(texto);
}

void ver_siguiente_feed(posts_t * posts, sesion_t * sesion, usuarios_t *usuarios){

    if(!sesion_esta_loggeado(sesion)){
        fprintf(stdout, "%s\n", status_msj[ERROR_SIG_FEED]);
        return;
    }	
	
	char *usuario_actual = NULL; // Usuario que esta viendo los feeds
	char *usuario = NULL;        // Guardo el nombre del usuario que posteo el feed
    ssize_t id = 0;

	usuario_actual = sesion_obtener_usuario(sesion);
	id = usuarios_ver_sig_feed(usuarios, usuario_actual);

	if(id == -1){
        fprintf(stdout, "%s\n", status_msj[ERROR_SIG_FEED]);
        return;
    }	

    char *texto = NULL;
    size_t cant_likes = 0;

	if(posts_ver_siguiente_feed(posts, id, &usuario, &cant_likes, &texto)){
	    fprintf(stdout, "Post ID %ld\n%s dijo: %s\nLikes: %zu\n", id, usuario, texto, cant_likes);
    }
}

void likear_post(posts_t * posts, sesion_t * sesion, usuarios_t *usuarios){
	
	if(!sesion_esta_loggeado(sesion)){
        fprintf(stdout, "%s\n", status_msj[ERROR_LIKEAR]);
        return;
    }

	char *clave_id = NULL;
	size_t tam = 0;
	if(!getline(&clave_id, &tam, stdin)) return;   
	chomp(clave_id);

	if(!posts_pertenece(posts, clave_id)){
		fprintf(stdout, "%s\n", status_msj[ERROR_LIKEAR]);
        free(clave_id);
		return;
	}

    char *usuario = sesion_obtener_usuario(sesion);
			
	if(!posts_likear(posts, clave_id, usuario)){
		fprintf(stdout, "%s\n", status_msj[ERROR_LIKEAR]);
        free(clave_id);
		return;
	}

	fprintf(stdout, "%s\n", status_msj[OK_LIKEAR]);		
	free(clave_id);
}

void mostrar_likes(posts_t *posts, sesion_t *sesion, usuarios_t *usuarios){
	
	char *clave_id = NULL;
	size_t tam = 0;
	if(!getline(&clave_id, &tam, stdin)) return;   
	chomp(clave_id);

	if(!posts_pertenece(posts, clave_id)){
		fprintf(stdout, "%s\n", status_msj[ERROR_MOSTRAR_LIKES]);
        free(clave_id);
		return;
	}

    size_t cant_likes = 0;
	lista_t* lista = posts_mostrar_likes(posts, clave_id, &cant_likes);
    if(!cant_likes){
        fprintf(stdout, "%s\n", status_msj[ERROR_MOSTRAR_LIKES]);
        free(clave_id);
        lista_destruir(lista, NULL);
		return;
    }
	
    fprintf(stdout, "El post tiene %zu likes:\n", cant_likes);

    char* usuario;
	lista_iter_t* iter = lista_iter_crear(lista);

	while(!lista_iter_al_final(iter)){
		usuario = lista_iter_ver_actual(iter);
		fprintf(stdout, "\t%s\n", usuario);
        lista_iter_avanzar(iter);
    }
    	
    free(clave_id);
	lista_iter_destruir(iter);
	lista_destruir(lista, NULL);
}