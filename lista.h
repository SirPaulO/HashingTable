#ifndef LISTA_H
#define LISTA_H

#include <stdlib.h>
#include <stdbool.h>


/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* La lista está planteada como una lista de punteros genéricos. */

//struct lista;
typedef struct lista lista_t;

typedef struct lista_iter lista_iter_t;


/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t* lista_crear(void);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void*));

// Devuelve verdadero o falso, según si la lista tiene o no elementos enlistados.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo elemento a la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al principio
// de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo elemento a la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al final
// de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void* lista_ver_primero(const lista_t *lista);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista no estaba vacía.
void* lista_borrar_primero(lista_t *lista);

// Devuelve el largo de la lista
// Pre: la lista fue creada.
// Post: se devolvió el largo de la lista
size_t lista_largo(const lista_t *lista);

// Se crea un iterador de la lista
// Pre: la lista fue creada.
// Post: se devolvió un iterador posicionado en el primer elemento
lista_iter_t *lista_iter_crear(const lista_t *lista);

// Devuelve si el iterador se encuentra despues del ultimo elemento en la lista
// Pre: el iterador fue creado
// Post: se devolvio NULL si el iter no fue creado
bool lista_iter_al_final(const lista_iter_t *iter);

// Avanza el iterador al siguiente nodo en la lista
// Pre: el iterador fue creado
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el puntero al dato alacenado en la posicion que se encuentra el iterador
// Pre: el iterador fue creado
// Post: Se devolvio un puntero al dato o NULL
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Destruye el iterador de una lista
// Pre: el iterador fue creado
void lista_iter_destruir(lista_iter_t *iter);

// Inserta en una lista en la posicion actual del iterador
// Pre: el iterador y la lista fueron creados
// Post: devuelve NULL si algun parametro no es correcto
bool lista_insertar(lista_t *lista, lista_iter_t *iter, void *dato);

// Elimina un elemento de la lista en la posicion actual del iterador
// Pre: el iterador y la lista fueron creados
// Post: devuelve un puntero al dato del nodo que fue borrado o NULL si algun parametro no es correcto
void *lista_borrar(lista_t *lista, lista_iter_t *iter);

// Itera la lista aplicandole la funcion visitar a cada dato almacenado, pasandole el parametro extra para que esta lo utilice
// Pre: la lista fue creada
void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra);


/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en lista.h
// (esto es, las pruebas no pueden acceder a los miembros del struct lista).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.
void pruebas_lista_alumno(void);

#endif // LISTA_H
