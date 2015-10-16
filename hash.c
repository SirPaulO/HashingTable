
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "lista.h"
#define LARGO_INICIAL 30

// Los structs deben llamarse "hash" y "hash_iter".
// HASH ABIERTO
// Las listas van a almacenar Nodo_hash con par Clave/Valor
// Reemplazo de vector_t a vector[] porque la redimension de un vector dinamico rompe con el Hash

// Tipo de función para destruir dato
typedef void (*hash_destruir_dato_t)(void *);

typedef struct hash {
    size_t tam; /* Cantidad de elementos en el vector*/
    size_t largo; /* Cantidad memoria del vector*/
    hash_destruir_dato_t destruir_dato; /* Funcion para destruir los datos */
    void** vector; /* Arreglo (HashTable) para guardar las listas */
} hash_t ;

/* Nodo para guardar en la Lista */
typedef struct nodo_hash {
    const char* clave;
    void* dato;
} nodo_hash_t;

/* ... */
typedef struct hash_iter {
    lista_t* actual;
    lista_iter_t* lista_iter;
    size_t posicion_actual;
    size_t numero_lista_actual;
    const hash_t* hash;
} hash_iter_t;

/* Nodo para pasar como Extra en el iterador de la lista y comparar pertenencia o actualizar dato */
typedef struct nodo_auxiliar {
    const char* clave;
    void* dato;
    bool encontrado;
    bool actualizar;
    hash_destruir_dato_t destruir_dato;
} nodo_auxiliar_t;

/* Inicializar todas las posiciones en NULL */
void vector_limpiar(void* vector[], size_t largo) {
    for(int i=0;i<largo;i++)
        vector[i] = NULL;
}

/*
  Algoritmo de Hash extraido de la libreria SDBM
  Fuente: http://stackoverflow.com/a/14409947
  Post: Devuelve un entero dentro del rango de 0 a largo-1.
*/
size_t hashear(const char *key, size_t largo) {
	size_t hashAddress = 0;

	for (int counter = 0; key[counter]!='\0'; counter++)
    {
        hashAddress = (size_t) counter + (hashAddress << 6) + (hashAddress << 16) - hashAddress;
    }

	hashAddress = hashAddress%largo;
	return hashAddress;
}

/* Crea el Hash */
hash_t *hash_crear(hash_destruir_dato_t destruir_dato) {
    hash_t *hash = malloc(sizeof(hash_t));
    if(!hash) return NULL;

    hash->destruir_dato = destruir_dato;
    hash->tam = 0;
    hash->largo = LARGO_INICIAL;
    hash->vector = malloc(sizeof(void*) * LARGO_INICIAL);

    if(!hash->vector)
    {
    	free(hash);
    	return NULL;
    }

    vector_limpiar(hash->vector, LARGO_INICIAL);

    return hash;
}

/* Copia la clave en memoria para evitar que el usuario la cambie */
char* copiar_clave(const char *clave) {
    char* clave_copiada = malloc(sizeof(clave));
    strcpy(clave_copiada, clave);
    return clave_copiada;
}

/* Compara si la clave almacenada en el nodo_hash es igual a la que estamos buscando */
bool comparar_claves(void* nodo_hash, void* extra) {
    // No valido porque tengo confianza en mi (:
    nodo_hash_t* nodo = nodo_hash;
    nodo_auxiliar_t* auxiliar = extra;

    if( strcmp(auxiliar->clave, nodo->clave) == 0)
    {
        auxiliar->encontrado = true;

        if(auxiliar->actualizar)
        {
            if(auxiliar->destruir_dato != NULL)
                auxiliar->destruir_dato(nodo->dato);
            nodo->dato = auxiliar->dato;
        }

        return false;
    }
    return true;
}

/* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
bool hash_pertenece(const hash_t *hash, const char *clave) {
    if(!hash || !clave)
        return NULL;

    size_t clave_hasheada = hashear(clave,hash->largo);

    lista_t* lista = hash->vector[clave_hasheada];

    if(!lista) return false;

    nodo_auxiliar_t* auxiliar = malloc(sizeof(nodo_auxiliar_t));
    if(!auxiliar) return NULL;

    auxiliar->actualizar = false;
    auxiliar->encontrado = false;
    auxiliar->clave = clave;

    // Por si acaso.
    auxiliar->destruir_dato = NULL;
    auxiliar->dato = NULL;

    lista_iterar(lista, comparar_claves, auxiliar);

    return auxiliar->encontrado;
}

/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 IMPORTANTE: (a) COPIAR CLAVE (para que no te la modifique el usuario) (b) Destruir dato si hay que actualizar
 */
bool hash_guardar(hash_t *hash, const char *clave, void *dato) {

    if(!hash || !clave || !dato)
        return NULL;

    size_t clave_hasheada = hashear(clave, hash->largo);

    lista_t* lista = hash->vector[clave_hasheada];

    // Si la lista no existe o la clave no se encuentra en el Hash
    if( !lista || !hash_pertenece(hash, clave) )
    {
        bool creeLista = false;
        // Verificar si hay una lista e insertar ultimo o crear una.
        if(!lista)
        {
            lista = lista_crear();
            if(lista == NULL)
                return false;
            creeLista = true;
        }

        nodo_hash_t* nodo = malloc(sizeof(nodo_hash_t));

        if(!nodo)
        {
            if(creeLista)
                free(lista);
            return false;
        }

        char* clave_copiada = copiar_clave(clave);

        nodo->clave = clave_copiada;
        nodo->dato = dato;

        bool insertado = lista_insertar_ultimo(lista, nodo);

        if(!insertado)
        {
            free(clave_copiada);
            free(nodo);

            if(creeLista)
                free(lista);

            return false;
        }

        // Si la cree, la apunto, sino apunto a lo apuntado (nada)
        hash->vector[clave_hasheada] = lista;

        hash->tam++;
    }
    else // Si la clave Pertenece, actualizar el valor usando nodo auxliar y lista iterar
    {
        // Redundante
        if(!lista) return false;

        nodo_auxiliar_t* auxiliar = malloc(sizeof(nodo_auxiliar_t));
        if(!auxiliar) return NULL;

        auxiliar->clave = clave;
        auxiliar->dato = dato;
        auxiliar->actualizar = true;
        auxiliar->encontrado = false;
        auxiliar->destruir_dato = hash->destruir_dato;

        lista_iterar(lista, comparar_claves, auxiliar);

        return auxiliar->encontrado;
    }
	return false;
}

/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 IMPORTANTE: DSTRUIR DATO SINO ES NULL
 */
void *hash_borrar(hash_t *hash, const char *clave) {
    // TODO
    return NULL;
}

/* Compara si la clave almacenada en el nodo_hash es igual a la que estamos buscando y guarda su dato*/
bool obtener_dato(void* nodo_hash, void* extra) {
    // No valido porque tengo confianza en mi (:
    nodo_hash_t* nodo = nodo_hash;
    nodo_hash_t* auxiliar = extra;

    if( strcmp(auxiliar->clave, nodo->clave) == 0)
    {
        auxiliar->dato = nodo->dato;
        return false;
    }
    return true;
}

/* Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *hash_obtener(const hash_t *hash, const char *clave) {
    if(!hash || !clave) return NULL;
    if(!hash_pertenece(hash, clave)) return NULL;

    size_t clave_hasheada = hashear(clave,hash->largo);

    lista_t* lista = hash->vector[clave_hasheada];

    nodo_hash_t* auxiliar = malloc(sizeof(nodo_hash_t));
    if(!auxiliar) return NULL;

    auxiliar->clave = clave;
    auxiliar->dato = NULL;

    lista_iterar(lista, obtener_dato, auxiliar);

    return auxiliar->dato;
}

/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
 */
size_t hash_cantidad(const hash_t *hash) {
    if(!hash) return NULL;
	return hash->tam;
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash) {
    if(!hash) return;

    for(int i=0;i<hash->largo;i++)
    {
        lista_t* lista = hash->vector[i];
        if(!lista) continue;

        while(!lista_esta_vacia(lista))
        {
            nodo_hash_t* nodo = lista_borrar_primero(lista);

            if(hash->destruir_dato != NULL)
                hash->destruir_dato(nodo->dato);

            free(nodo);
        }
    }
}

/* Iterador del hash */

// Crea iterador
hash_iter_t *hash_iter_crear(const hash_t *hash) {
    if(!hash) return NULL;

	hash_iter_t *hash_iter = malloc(sizeof(hash_iter_t));
	size_t posicion_primer_lista = 0;
	lista_t* lista = NULL;

    if(!hash_iter) return NULL;

    if(hash_cantidad(hash) == 0)
    {
        hash_iter->actual = NULL;
        return hash_iter;
    }

    lista = hash->vector[posicion_primer_lista];

    while(lista == NULL && posicion_primer_lista < hash->largo)
    {
        posicion_primer_lista++;
        lista = hash->vector[posicion_primer_lista];
    }

    hash_iter->posicion_actual = posicion_primer_lista;
    hash_iter->numero_lista_actual = 1;
    hash_iter->actual = lista;
    hash_iter->hash = hash;

    // WARN: Puede ser NULL
    hash_iter->lista_iter = lista_iter_crear(hash_iter->actual);

    if(!hash_iter->lista_iter)
    {
        free(hash_iter);
        return NULL;
    }

    return hash_iter;
}

// Comprueba si terminó la iteración
bool hash_iter_al_final(const hash_iter_t *hash_iter) {
    if(!hash_iter) return NULL;
    return hash_iter->numero_lista_actual >= hash_iter->hash->tam;
}

// Avanza iterador (no me digas..)
bool hash_iter_avanzar(hash_iter_t *hash_iter) {
    if(!hash_iter) return NULL;

    // 1 - Iterador del hash al final del mismo.
    if(hash_iter_al_final(hash_iter)) return false;

    bool iter_lista_al_final = lista_iter_al_final(hash_iter->lista_iter);

    // 2 - Iterador del hash y de la lista al final.
    if(iter_lista_al_final && hash_iter_al_final(hash_iter))
        return false;

    if(!iter_lista_al_final)
        return lista_iter_avanzar(hash_iter->lista_iter);
    else
    {
        lista_iter_destruir(hash_iter->lista_iter);
        hash_iter->actual = hash_iter->hash->vector[++hash_iter->posicion_actual];

        while(hash_iter->actual == NULL && hash_iter->posicion_actual < hash_iter->hash->largo)
        {
            hash_iter->posicion_actual++;
            hash_iter->actual = hash_iter->hash->vector[hash_iter->posicion_actual];
        }

        hash_iter->numero_lista_actual++;

        return true;
    }

    return false;
}

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
const char *hash_iter_ver_actual(const hash_iter_t *hash_iter) {
    if(!hash_iter || hash_iter_al_final(hash_iter))
        return NULL;

    const nodo_hash_t* nodo = lista_iter_ver_actual(hash_iter->lista_iter);
    return nodo->clave;
}

// Destruye iterador
void hash_iter_destruir(hash_iter_t* hash_iter) {
    if(!hash_iter) return;
    lista_iter_destruir(hash_iter->lista_iter);
    free(hash_iter);
}
