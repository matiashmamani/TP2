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

enum status{
    OK_HOLA = 0,
    ERROR_USUARIO_YA_LOGGEADO,
    ERROR_USUARIO_NO_EXISTE,
    OK_ADIOS,
    ERROR_USUARIO_NO_LOGGEADO,
    OK_POST,
    OK_LIKEAR,
    ERROR_LIKEAR,
    ERROR_MOSTRAR_LIKES
};

const char *status_msj[] = {
    "Hola", // "Hola <usuario>"
    "Error: Ya habia un usuario loggeado",
    "Error: usuario no existente",
    "Adios",
    "Error: no habia usuario loggeado",
    "Post Publicado",
    "Post likeado",
    "Error: Usuario no loggeado o Post inexistente",
    "Error: Post inexistente o sin likes"
};

bool crear_TDAs(usuarios_t** usuarios, sesion_t** sesion, posts_t** posts);
void destruir_TDAs(usuarios_t *usuarios, sesion_t *sesion, posts_t *posts);
bool cargar_archivo(usuarios_t *usuarios, const char *nombre_archivo);
bool algogram(usuarios_t *usuarios, sesion_t *sesion, posts_t *posts);
void chomp(char *str);
void login(usuarios_t *usuarios, sesion_t *sesion);
void logout(sesion_t *sesion);
void publicar(posts_t* posts, sesion_t *sesion,usuarios_t *usuarios);
void ver_siguiente_feed(posts_t * posts, sesion_t * sesion,usuarios_t *usuarios);
void likear_post(posts_t * posts, sesion_t * sesion,usuarios_t *usuarios);
bool extraer_id(char * str , int *id);
void mostrar_likes(posts_t * posts, sesion_t * sesion,usuarios_t *usuarios);

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


void publicar(posts_t* posts, sesion_t *sesion, usuarios_t * usuarios){

	char *texto = NULL;
	size_t tam = 0;
	ssize_t id_posts = 0;
	char *usuario = NULL;
	
	usuario = sesion_obtener_usuario(sesion);
	
    	if(!sesion_esta_loggeado(sesion)){
        	fprintf(stdout, "%s\n", status_msj[ERROR_USUARIO_NO_LOGGEADO]);
        	return;
    	}	
	
	getline(&texto, &tam, stdin);   
	chomp(texto);
	
	id_posts = posts_publicar(posts, texto, usuario); //Devuelve -1 si no pudo realizar el dato de hash
		
	fprintf(stdout, "%zu\n", id_posts); //BORRAR
	
//	if(id_posts > -1)
	if(usuarios_publicar(usuarios, id_posts, usuario))
		fprintf(stdout, "%s\n", status_msj[OK_POST]);
	//No dice nada el enunciado pero si no puede generar bien el hash debería decir indicar error de alguna forma.	
	
	free(texto);
	
	posts_ver(posts,id_posts);//Esto es de prueba para ver que genero bien. Hay que borrarlo.
}

void ver_siguiente_feed(posts_t * posts, sesion_t * sesion, usuarios_t *usuarios){

	size_t * cant_likes = 0;
	char * texto = NULL;
	ssize_t id = 0;

	char *usuario_actual = NULL; //Usuario que esta viendo los feeds
	char *usuario = NULL; //Guardo el nombre del usuario que posteo el feed

	usuario_actual = sesion_obtener_usuario(sesion);
	
	id = usuarios_ver_sig_feed(usuarios, usuario_actual);
	
	fprintf(stdout, "%ld\n", id); //BORRAR

	if(posts_ver_siguiente_feed(posts, id, &usuario, cant_likes, &texto))
		fprintf(stdout, "Post ID %ld\n %s dijo: %s\n %n\n", id, usuario, texto, (int*)cant_likes);

}

void likear_post(posts_t * posts, sesion_t * sesion,usuarios_t *usuarios){
	char *usuario_actual = NULL;
	char *texto = NULL;
	size_t tam = 0;
	int id;
	
	getline(&texto, &tam, stdin);   
	chomp(texto);
		
	usuario_actual = sesion_obtener_usuario(sesion);
	
	if(!sesion_esta_loggeado(sesion)){
        	fprintf(stdout, "%s\n", status_msj[ERROR_LIKEAR]);
        	return;
    	}
    	
	//Valida existencia del post
/*	char clave[5];
	sprintf(clave,"%u",id);
	if(!hash_pertenece(posts->hash,clave)){ 		
		fprintf(stdout, "%s\n", status_msj[ERROR_LIKEAR]);
		return;
	}
*/	    	
    	if(!extraer_id(texto, &id)){
		fprintf(stdout, "%s\n", status_msj[ERROR_LIKEAR]);
		return;
	}
			
	if(!posts_likear(posts,(ssize_t)id,usuario_actual)){
		fprintf(stdout, "%s\n", status_msj[ERROR_LIKEAR]);
		return;
	}
		
	fprintf(stdout, "%s\n", status_msj[OK_LIKEAR]);
	posts_ver(posts,(ssize_t)id);//BORRAR Elu
}

//Auxiliar
bool extraer_id(char * str , int * id){
	if(str==NULL)return false;
	
	for (int i = 0; i < strlen(str); i++)
    	{
        	if(!isdigit(str[i]) ) {
            		return false;
            	}
    	}	

	*id = atoi(str);
    	return true;
}

void mostrar_likes(posts_t * posts, sesion_t * sesion,usuarios_t *usuarios){
	size_t cant_likes = 0;
	char *texto = NULL;
	size_t tam = 0;
	int id;
	char* usuario;
		
	getline(&texto, &tam, stdin);   
	chomp(texto);

  	if(!extraer_id(texto, &id)){
		fprintf(stdout, "%s\n", status_msj[ERROR_MOSTRAR_LIKES]);
		return;
	}
/*
	char clave[5];
	sprintf(clave,"%u",id);

	//Valida existencia del post
	if(!hash_pertenece(posts->hash,clave)){ 		
		fprintf(stdout, "%s\n", status_msj[ERROR_MOSTRAR_LIKES]);
		return;
	}
*/	
	lista_t* lista;
	
	lista = posts_mostrar_likes(posts, (size_t)id, &cant_likes);

	fprintf(stdout, "El post tiene %zu likes:\n", cant_likes); //BORRAR Elu

	lista_iter_t* iter;
	iter = lista_iter_crear(lista);

	while(!lista_iter_al_final(iter)){
		usuario = lista_iter_ver_actual(iter);
		fprintf(stdout, "\t%s\n", usuario);
        	lista_iter_avanzar(iter);
    	}
    	
	lista_iter_destruir(iter);
	lista_destruir(lista,NULL);
}


