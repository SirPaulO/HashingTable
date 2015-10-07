#include "pila.h"
#include <stdlib.h>
#include <stdbool.h>

#define LIBRE_MINIMO 0.25  // Un margen de la cuarta parte libre para almacenar
#define LIBRE_MAXIMO 0.5   // Un maximo de la mitad libre para almacenar

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void **datos;
    size_t tam;     // Cantidad usada
    size_t largo;   // Largo total
};

const size_t tamanio_inicial = 10;

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

// Crea una pila.
// Post: devuelve una nueva pila vacía.
pila_t* pila_crear()
{
    pila_t* pila = malloc(sizeof(pila_t));

    if (pila == NULL) {
    return NULL;
    }
    pila->datos = malloc( tamanio_inicial * sizeof( void* ) );

    if (tamanio_inicial > 0 && pila->datos == NULL) {
        free(pila);
        return NULL;
    }
    pila->tam = 0;
    pila->largo = tamanio_inicial;
    return pila;
}

// Destruye la pila.
// Pre: la pila fue creada.
// Post: se eliminaron todos los elementos de la pila.
void pila_destruir(pila_t *pila)
{
    if(pila)
    {
        free(pila->datos);
        free(pila);
        pila = NULL;
    }
}

// Devuelve verdadero o falso, según si la pila tiene o no elementos apilados.
// Pre: la pila fue creada.
bool pila_esta_vacia(const pila_t *pila)
{
   return pila->tam > 0 ? false : true;
}

// Obtiene el valor del tope de la pila. Si la pila tiene elementos,
// se devuelve el valor del tope. Si está vacía devuelve NULL.
// Pre: la pila fue creada.
// Post: se devolvió el valor del tope de la pila, cuando la pila no está
// vacía, NULL en caso contrario.
void* pila_ver_tope(const pila_t *pila)
{
    return pila_esta_vacia(pila) ? NULL : pila->datos[pila->tam-1];
}

// Incrementa/Decrementa el largo de los datos de la pila en caso de ser necesario
// Pre: la pila fue creada.
// Post: se devolvió false si ocurrió un error, de lo contrario devuelve true.
bool redimensionar_pila(pila_t *pila)
{

    // Si el tamanio de la pila es igual (o mayor) al lago, O el tamanio es menor a la mitad de la longitud Y esta es mayor que TAMANIO_INICIAL.
    // Redimensiona solo si el largo > TAMANIO_INICIAL y necesita o sobra espacio.
    if( (pila->tam >= pila->largo) ||
      ( (pila->tam < (size_t) ((long double) pila->largo * (1 - LIBRE_MAXIMO)) && pila->largo > tamanio_inicial)) ) {

        size_t largo_nuevo = (size_t) ( pila->tam + ( (long double) pila->tam * LIBRE_MINIMO) );

        void* datos_nuevos = realloc(pila->datos, largo_nuevo * sizeof(void *));

        if(datos_nuevos == NULL)
            return false;

        pila->datos = datos_nuevos;
        pila->largo = largo_nuevo;
    }
    return true;
}

// Agrega un nuevo elemento a la pila. Devuelve falso en caso de error.
// Pre: la pila fue creada.
// Post: se agregó un nuevo elemento a la pila, valor es el nuevo tope.
bool pila_apilar(pila_t *pila, void* valor)
{
    return !redimensionar_pila(pila) ? false : (pila->datos[pila->tam++] = valor);
}

// Saca el elemento tope de la pila. Si la pila tiene elementos, se quita el
// tope de la pila, y se devuelve ese valor. Si la pila está vacía O NO PUEDE REDIMENSIONAR, devuelve
// NULL.
// Pre: la pila fue creada.
// Post: si la pila no estaba vacía, se devuelve el valor del tope anterior
// y la pila contiene un elemento menos.
void* pila_desapilar(pila_t *pila)
{
    if(pila_esta_vacia(pila) || !redimensionar_pila(pila))
        return NULL;
    void* dato = pila_ver_tope(pila);
    pila->tam--;

    return dato;
}