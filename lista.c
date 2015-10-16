#include <stdlib.h>
#include "lista.h"
#include <stdbool.h>

typedef struct nodo {
	void* dato;
	struct nodo* siguiente;
} nodo_t;

struct lista {
	nodo_t* primero;
	nodo_t* ultimo;
	size_t largo;
};

struct lista_iter
{
	nodo_t* nodo_ant;
	nodo_t* nodo_act;
};

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t* lista_crear(void)
{
    lista_t* lista = malloc(sizeof(lista_t));

    if(!lista)
    	return NULL;

    lista->primero = NULL;
    lista->ultimo = NULL;
    lista->largo = 0;

    return lista;
}

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void*))
{
	while(!lista_esta_vacia(lista))
        	if(destruir_dato!=NULL)
				destruir_dato(lista_borrar_primero(lista));
		else
			lista_borrar_primero(lista);
	free(lista);
}

// Devuelve verdadero o falso, según si la lista tiene o no elementos enlistados.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista)
{
	return (lista->primero == NULL);
}

// Agrega un nuevo elemento al principio de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al principio
// de la lista.
bool lista_insertar_primero(lista_t *lista, void* valor)
{
	nodo_t* nodo = malloc(sizeof(nodo_t));

	if(nodo == NULL)
		return false;

	lista->largo++;
	nodo->siguiente = lista->primero;
	nodo->dato = valor;

	if(lista_esta_vacia(lista))
		lista->ultimo = nodo;

	lista->primero = nodo;
	return true;
}

// Agrega un nuevo elemento al final de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al final
// de la lista.
bool lista_insertar_ultimo(lista_t *lista, void* valor)
{
    if(lista_esta_vacia(lista))
        return lista_insertar_primero(lista, valor);

	nodo_t* nodo = malloc(sizeof(nodo_t));
	nodo->dato = valor;
    nodo->siguiente = NULL;

	if(nodo == NULL)
		return false;

    lista->ultimo->siguiente = nodo;
	lista->ultimo = nodo;
	lista->largo++;

	return true;
}

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void* lista_ver_primero(const lista_t *lista)
{
	return (!lista_esta_vacia(lista)) ? lista->primero->dato : NULL;
}

// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista no estaba vacía.
void* lista_borrar_primero(lista_t *lista)
{
	if(lista_esta_vacia(lista))
		return NULL;

	void* dato = lista_ver_primero(lista);

	nodo_t* nuevo_primero = lista->primero->siguiente;
	free(lista->primero);
	lista->largo--;
	lista->primero = nuevo_primero;
	return dato;
}

// Devuelve el largo de la lista
// Pre: la lista fue creada.
// Post: se devolvió el largo de la lista
size_t lista_largo(const lista_t *lista)
{
	return lista->largo;
}

// Se crea un iterador de la lista
// Pre: la lista fue creada.
// Post: se devolvió un iterador posicionado en el primer elemento
lista_iter_t *lista_iter_crear(const lista_t *lista)
{
    lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if(!iter) return NULL;

	iter->nodo_ant = NULL;
	iter->nodo_act = lista->primero;

	return iter;
}

// Devuelve si el iterador se encuentra despues del ultimo elemento en la lista
// Pre: el iterador fue creado
// Post: se devolvio NULL si el iter no fue creado
bool lista_iter_al_final(const lista_iter_t *iter)
{
	if(!iter) return NULL;
	if(!iter->nodo_act) return true;
	return false;
}

// Avanza el iterador al siguiente nodo en la lista
// Pre: el iterador fue creado
bool lista_iter_avanzar(lista_iter_t *iter)
{
	if(!iter || lista_iter_al_final(iter))
		return false;

	iter->nodo_ant = iter->nodo_act;
	iter->nodo_act = iter->nodo_act->siguiente;
	return true;
}

// Devuelve el puntero al dato alacenado en la posicion que se encuentra el iterador
// Pre: el iterador fue creado
// Post: Se devolvio un puntero al dato o NULL
void* lista_iter_ver_actual(const lista_iter_t *iter)
{
	if(!iter || lista_iter_al_final(iter))
		return NULL;

	return iter->nodo_act->dato;
}

// Destruye el iterador de una lista
// Pre: el iterador fue creado
void lista_iter_destruir(lista_iter_t *iter)
{
	if(!iter) return;
	free(iter);
}

// Inserta en una lista en la posicion actual del iterador
// Pre: el iterador y la lista fueron creados
// Post: devuelve NULL si algun parametro no es correcto
bool lista_insertar(lista_t *lista, lista_iter_t *iter, void *dato)
{
	if(!lista || !iter || !dato)
		return false;

    // Caso 1: Iter al principio de la lista
	if(!iter->nodo_ant)
	{
		if(!lista_insertar_primero(lista, dato))
			return false;
		iter->nodo_act = lista->primero;
		iter->nodo_ant = NULL;
		return true;
	}

	nodo_t* nodo = malloc(sizeof(nodo_t));
    if(nodo == NULL) return false;

    nodo->dato = dato;

	// Caso 2: Iter al final
	if(lista_iter_al_final(iter))
    {
        nodo->siguiente = NULL;
        lista->ultimo = nodo;
    }
    else // Caso 3: Iter entre dos nodos
        nodo->siguiente = iter->nodo_act;

    iter->nodo_ant->siguiente = nodo;
    iter->nodo_act = nodo;

	lista->largo++;
	return true;
}

// Elimina un elemento de la lista en la posicion actual del iterador
// Pre: el iterador y la lista fueron creados
// Post: devuelve un puntero al dato del nodo que fue borrado o NULL si algun parametro no es correcto
void* lista_borrar(lista_t *lista, lista_iter_t *iter)
{
    if(!lista || !iter || lista_iter_al_final(iter) || lista_esta_vacia(lista))
		return NULL;

    nodo_t *nodo = iter->nodo_act;
    void *dato = nodo->dato;

    // Caso 1: Iter al principio de la lista
    if(iter->nodo_ant == NULL)
    {

        lista->primero = iter->nodo_act->siguiente;
        iter->nodo_act = lista->primero;
    }
    else
    {
        iter->nodo_ant->siguiente = iter->nodo_act->siguiente;
        iter->nodo_act = iter->nodo_ant->siguiente;
    }

    free(nodo);
    lista->largo--;

    if(lista_largo(lista)==1)
        lista->ultimo = lista->primero;

    return dato;
}

// Itera la lista aplicandole la funcion visitar a cada dato almacenado, pasandole el parametro extra para que esta lo utilice
// Pre: la lista fue creada
void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra)
{
    if(!lista || !visitar || !extra)
		return;

    lista_iter_t *iter = lista_iter_crear(lista);

    if(!iter)
        return;

    while( !lista_iter_al_final(iter) && visitar(lista_iter_ver_actual(iter), extra) )
       lista_iter_avanzar(iter);

    lista_iter_destruir(iter);

}








