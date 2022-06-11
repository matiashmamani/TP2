#include "pila.h"
#include <stdlib.h>
#include <stdbool.h>

#define INITIAL_SIZE 10
#define RESIZE_FACTOR 2 
#define DECREMENTAL_FACTOR 4


//Redimensiona la capacidad de la pila según lo que indique el parametro nueva_capacidad. Devuelve false si no se pudo redimensionar o true si se pudo.
bool pila_redimensionar(pila_t *pila, size_t nueva_capacidad) ;

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void **datos;
    size_t cantidad;   // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

pila_t *pila_crear(void){
	pila_t *pila = malloc(sizeof(pila_t));
	
	if (pila ==NULL) return NULL;
	
	pila->datos = malloc(INITIAL_SIZE * sizeof(void *));
	
	if (pila->datos==NULL) 
	{
		free(pila);
		return NULL;
	}
	
	pila->capacidad = INITIAL_SIZE;
	pila->cantidad = 0;
    
	return pila;
}

void pila_destruir(pila_t *pila){
    free(pila->datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t *pila){
	return pila->cantidad ==0;
}

bool pila_apilar(pila_t *pila, void *valor){
	
	if(pila->cantidad == pila->capacidad)
	{
		if(!pila_redimensionar(pila,(pila->capacidad)*RESIZE_FACTOR)) return false;
	}
	
	pila->datos[pila->cantidad] = valor;
	pila->cantidad++;
	return true;
}

void *pila_ver_tope(const pila_t *pila){

	if(pila_esta_vacia(pila)) return NULL;

	return pila->datos[pila->cantidad-1];
}

void *pila_desapilar(pila_t *pila){
	if(pila_esta_vacia(pila)) return NULL;
	
	void *tope = pila_ver_tope(pila);

    	if(pila->cantidad*DECREMENTAL_FACTOR < pila->capacidad)
    	{
		pila_redimensionar(pila,(pila->capacidad)/RESIZE_FACTOR);
	}  
	
	pila->cantidad--;
	
	return tope;
	}

bool pila_redimensionar(pila_t *pila, size_t nueva_capacidad) {
	void ** aux = (void **) realloc (pila->datos, nueva_capacidad *sizeof (void *));
	
	if(aux == NULL) return false;	

	pila->datos = aux;
	pila->capacidad = nueva_capacidad;

	return true;
}



