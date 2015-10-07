
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "vector_dinamico.h"
#include "lista.h"
#define LARGO_INICIAL 30


vector_dinamico_t *crear_vector_dinamico();

// Los structs deben llamarse "hash" y "hash_iter".
// HASH ABIERTO

// Las listas van a almacenar Nodo_hash con par Clave/Valor

struct hash {
    size_t tam; /* Cantidad de elementos en el vector*/
    size_t largo; /* Cantidad memoria del vector*/
    void (*destruir_dato); /* Funcion para destruir los datos */
    vector_t *vector; /* Arreglo (HashTable) para guardar las listas */
};

/* Nodo para guardar en la Lista */
typedef struct nodo_hash
{
    char* clave;
    void* dato;
} nodo_hash_t;

/* ... */
struct hash_iter{
};


/* Nodo para pasar como Extra en el iterador de la lista y comparar pertenencia */
typedef struct nodo_busqueda
{
    char* clave;
    bool encontrado = false;
} nodo_busqueda_t;

// Tipo de función para destruir dato
typedef void (*hash_destruir_dato_t)(void *);

/* Crea el Hash */
hash_t *hash_crear(hash_destruir_dato_t destruir_dato)
{
    hash_t *hash = malloc(sizeof(hash_t));
    if(!hash)
        return NULL;
    hash->destruir_dato = destruir_dato;
    hash->tam = 0;
    hash->largo = LARGO_INICIAL;
    hash->vector = crear_vector_dinamico();
    if(!vector){
    	free(hash);
    	return NULL;
    }
    return hash;
}

/* Inicializar todas las posiciones en NULL */
void vector_limpiar(vector_t* vector, size_t largo)
{
    for(int i=0;i<largo;i++)
        vector_guardar(vector, i, NULL);
}

/* Crea un vector dinamico (HashTable) */
vector_t *crear_vector_dinamico(){
	vector_t *vector = vector_crear(LARGO_INICIAL);
	if(!vector)
		return NULL;

    vector_limpiar(vector, LARGO_INICIAL);

	return vector_dinamico;
}

/* Copia la clave en memoria para evitar que el usuario la cambie */
char* copiar_clave(const char *clave)
{
    char* clave_copiada = malloc(sizeof(clave));
    strcpy(clave_copiada, clave);
    return clave_copiada;
}

/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 IMPORTANTE: COPIAR CLAVE (para que no te la modifique el usuario)
 */
bool hash_guardar(hash_t *hash, const char *clave, void *dato){

    size_t clave_hasheada = hashear(clave, hash->largo);

    // Si la posicion en el HashTable es NULL
    if( !hash_pertenece(hash, clave) )
    {
        // Verificar si hay una lista e insertar ultimo o crear una.

        lista_t* lista = NULL;
        bool obtenido = vector_obtener(hash->vector, clave_hasheada, lista);

        if(!obtenido) return false;

        if(!lista)
        {
            lista = lista_crear();
            if(lista != NULL)
                return false;
        }

        nodo_hash_t* nodo = malloc(sizeof(nodo_hash_t));
        
        if(!nodo)
        {
            free(lista);
            return false;
        }
        
        char* clave_copiada = copiar_clave(clave);

        nodo->clave = clave_copiada;
        nodo->dato = dato;

        bool insertado = lista_insertar_ultimo(lista, nodo);
        bool guardado = hash->vector_guardar(hash->vector, clave_hasheada, lista);

        if(!insertado || !guardado)
        {
            free(clave_copiada);
            free(nodo);
            free(lista);
            return false;
        }
    }
    else // Si la clave Pertenece, actualizar el valor
    {

    }
    hash->tam++;
	return true;
}

/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 IMPORTANTE: DSTRUIR DATO SINO ES NULL
 */
void *hash_borrar(hash_t *hash, const char *clave);

/* Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *hash_obtener(const hash_t *hash, const char *clave);

/* Compara si la clave almacenada en el nodo_hash es igual a la que estamos buscando */
bool comparar_claves(void* dato, void* extra)
{
    nodo_hash_t* nodo = dato;
    nodo_busqueda_t* busqueda = extra;

    if( strcmp(busqueda->clave, nodo->clave) == 0)
    {
        busqueda->encontrado = true;
        return false;
    }
    return true;
}

/* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
bool hash_pertenece(const hash_t *hash, const char *clave)
{
    size_t clave_hasheada = hashear(clave,hash->largo);

    lista_t* lista = NULL;
    bool obtenido = vector_obtener(hash->vector, clave_hasheada, lista);

    if(!obtenido || !lista) return false;

    nodo_busqueda_t* busqueda;
    busqueda->clave = clave;

    lista_iterar(lista, comparar_claves, busqueda);

    return busqueda->encontrado;
}

/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
 */
size_t hash_cantidad(const hash_t *hash){
	return hash->tam;
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash)
{
    for(int i=0;i<hash->largo;i++)
    {
        lista_t* lista = NULL;
        bool obtenido = vector_obtener(hash->vector, i, lista);
        if(!obtenido || !lista) continue;

        while(!lista_esta_vacia(lista))
        {
            nodo_hash_t* nodo = lista_borrar_primero(lista);

            if(hash->destruir_dato != NULL)
                destruir_dato(nodo->dato);

            free(nodo);
        }
    }
}

/* Iterador del hash */

// Crea iterador
hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t *hash_iter = malloc(sizeof(hash_iter_t));
    if(!hash_iter)
        return NULL;
}

// Avanza iterador
bool hash_iter_avanzar(hash_iter_t *iter);

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
const char *hash_iter_ver_actual(const hash_iter_t *iter);

// Comprueba si terminó la iteración
bool hash_iter_al_final(const hash_iter_t *iter);

// Destruye iterador
void hash_iter_destruir(hash_iter_t* iter);

size_t hashear(void *key, size_t largo){
	size_t hashAddress = 0;
	for (int counter = 0; word[counter]!='\0'; counter++){
    	hashAddress = key[counter] + (hashAddress << 6) + (hashAddress << 16) - hashAddress;
	}
	hashAddress = hashAddress%largo < 0 ? hashAddress%largo * -1 : hashAddress%largo;
	return hashAddress;
}