#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef LOCAL
#include "Lista/lista.h"
#else
#include "lista.h"
#endif

#include "lookup3.h" /* lookup3.c, by Bob Jenkins, May 2006, Public Domain. */

#define LARGO_INICIAL 773
#define FACTOR_CARGA_MAXIMO 2.5
#define FACTOR_CARGA_MINIMO 0.5
#define AUMENTO_LIBRE 2             // Factor para aumentar largo del arreglo
#define REDUCCION_LIBRE 0.25        // Factor para reducir largo del arreglo

/*
 * HASH ABIERTO
 * Los structs deben llamarse "hash" y "hash_iter".
 * El arreglo del HASH SE INICIALIZA EN NULL, LAS LISTAS SE CREAN CUANDO SE NECESITAN (AHORRO DE MEMORIA).
 * Las listas almacenan Nodo_hash con par Clave/Valor
 */

/* Standar documentation: GIGO. */

/* Tipo de función para destruir dato */
typedef void (*hash_destruir_dato_t)(void *);

/* Estructura principal del Hash */
typedef struct hash {
    size_t tam;                             /* Cantidad de elementos en el vector */
    size_t largo;                           /* Cantidad memoria del vector */
    hash_destruir_dato_t destruir_dato;     /* Funcion para destruir los datos */
    void** vector;                          /* Arreglo (HashTable) para guardar las listas */
    bool redimensionando;                   /* Evita que redimensione cuando esta en proceso de redimension */
} hash_t ;

/* Nodo para guardar en la Lista */
typedef struct nodo_hash {
    char* clave;
    void* dato;
} nodo_hash_t;

/* Iterador del hash */
typedef struct hash_iter {
    lista_t* actual;
    lista_iter_t* lista_iter;
    size_t posicion_actual;
    size_t numero_lista_actual;
    size_t items_recorridos;
    const hash_t* hash;
} hash_iter_t;

/************* PROTOTIPOS *************/
bool hash_redimensionar(hash_t* hash);
/**************************************/

/* Funcion para inicializar todas las posiciones de un arreglo en NULL */
void vector_limpiar(void* vector[], size_t largo) {
    for(size_t i=0;i<largo;i++)
        vector[i] = NULL;
}

/* Algoritmo de Hash by Bob Jenkins.
 * Post: Devuelve un entero dentro del rango de 0 a largo-1.
 */
size_t hashear(const char *key, size_t largo) {
    if(strlen(key)==0) return 0;

	unsigned int initval;
    unsigned int hashAddress;
    initval = 5381;
    hashAddress = lookup3(key, strlen(key), initval);
    return (hashAddress % largo);
    //return (hashAddress & (largo-1)); SOLO PARA LARGOS DE 2^n
}

/* Crea el Hash */
hash_t *hash_crear(hash_destruir_dato_t destruir_dato) {
    hash_t *hash = malloc(sizeof(hash_t));
    if(!hash) return NULL;

    hash->destruir_dato = destruir_dato;
    hash->redimensionando = false;
    hash->tam = 0;
    hash->largo = LARGO_INICIAL;
    hash->vector = malloc(sizeof(void*) * hash->largo);

    if(!hash->vector)
    {
    	free(hash);
    	return NULL;
    }

    vector_limpiar(hash->vector, hash->largo);

    return hash;
}

/* Copia la clave en memoria para evitar que el usuario la cambie */
char* copiar_clave(const char *clave) {
    char* clave_copiada = (char *) calloc(strlen(clave)+1, sizeof(char));
    strcpy(clave_copiada, clave);
    return clave_copiada;
}

/* Destruir el iterador una vez usado.
 * WARN: Si puntero_lista != NULL se crea una lista nueva cuando no existe.
 */
lista_iter_t* obtener_iterador_lista_por_clave(const hash_t *hash, const char *clave, lista_t** puntero_lista ) {
    if(!hash || !clave) return NULL;

    size_t clave_hasheada = hashear(clave, hash->largo);

    lista_t* lista = hash->vector[clave_hasheada];
    if(!lista)
    {
        if(puntero_lista)
        {
            hash->vector[clave_hasheada] = lista_crear();
            *puntero_lista = hash->vector[clave_hasheada];
        }
        return NULL;
    }

    lista_iter_t* lista_iter = lista_iter_crear(lista);
    if(!lista_iter) return NULL;

    if(puntero_lista) *puntero_lista = lista;

    while(!lista_iter_al_final(lista_iter) && strcmp(clave, ((nodo_hash_t*) lista_iter_ver_actual(lista_iter))->clave) != 0 )
        lista_iter_avanzar(lista_iter);

    return lista_iter;
}

/* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
bool hash_pertenece(const hash_t *hash, const char *clave) {
    lista_iter_t* lista_iter = obtener_iterador_lista_por_clave(hash, clave, NULL);
    bool pertenece = (lista_iter && !lista_iter_al_final(lista_iter));
    lista_iter_destruir(lista_iter);
    return pertenece;
}

// TODO: Documentar.
nodo_hash_t* crear_nodo(const char *clave, void* dato) {
    if(!clave) return NULL;

    nodo_hash_t* nodo = malloc(sizeof(nodo_hash_t));
    if(!nodo) return NULL;

    char* clave_copiada = copiar_clave(clave);
    nodo->clave = clave_copiada;
    nodo->dato = dato;
    return nodo;
}

/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 IMPORTANTE: (a) COPIAR CLAVE (para que no te la modifique el usuario) (b) Destruir dato si hay que actualizar
 */
bool hash_guardar(hash_t *hash, const char *clave, void *dato) {
    if(!hash || !clave || !hash_redimensionar(hash)) return false;

    lista_t* lista = NULL;
    lista_iter_t* lista_iter = obtener_iterador_lista_por_clave(hash, clave, &lista);
    bool pertenece = (lista_iter && !lista_iter_al_final(lista_iter));

    if(pertenece)
    {
        nodo_hash_t* nodo = lista_iter_ver_actual(lista_iter);
        if(hash->destruir_dato) hash->destruir_dato(nodo->dato);
        nodo->dato = dato;
        lista_iter_destruir(lista_iter);
        return true;
    }
    lista_iter_destruir(lista_iter);

    nodo_hash_t* nodo = crear_nodo(clave, dato);
    if(!nodo) return false;

    bool insertado = lista_insertar_ultimo(lista, nodo);
    if(!insertado)
    {
        free(nodo->clave);
        free(nodo);
        return false;
    }

    hash->tam++;
    return insertado;

}

/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 IMPORTANTE: DSTRUIR DATO SI NO ES NULL
 */
void* hash_borrar(hash_t *hash, const char *clave) {
    if(!hash || !clave || !hash_redimensionar(hash)) return NULL;

    lista_t* lista = NULL;
    lista_iter_t* lista_iter = obtener_iterador_lista_por_clave(hash, clave, &lista);
    bool pertenece = (lista_iter && !lista_iter_al_final(lista_iter));

    if(!pertenece)
    {
        if(lista) lista_destruir(lista, NULL);
        hash->vector[hashear(clave, hash->largo)] = NULL;   // TODO: No me gusta, pero es necesario dejarlo en NULL.
        lista_iter_destruir(lista_iter);
        return NULL;
    }

    nodo_hash_t* nodo = lista_borrar(lista, lista_iter);
    lista_iter_destruir(lista_iter);

    if(!nodo) return NULL;

    void* dato = nodo->dato;

    free(nodo->clave);
    free(nodo);

    if(lista_esta_vacia(lista))
    {
        lista_destruir(lista, NULL);
        hash->vector[hashear(clave, hash->largo)] = NULL;    // TODO: No me gusta, pero es necesario dejarlo en NULL.
    }

    hash->tam--;

    return dato;
}

/* Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void* hash_obtener(const hash_t *hash, const char *clave) {
    lista_iter_t* lista_iter = obtener_iterador_lista_por_clave(hash, clave, NULL);
    bool pertenece = (lista_iter && !lista_iter_al_final(lista_iter));
    void* dato = pertenece ? ((nodo_hash_t*)lista_iter_ver_actual(lista_iter))->dato : NULL;
    lista_iter_destruir(lista_iter);
    return dato;
}

/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
 */
size_t hash_cantidad(const hash_t *hash) {
	return hash ? hash->tam : 0;
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash) {
    if(!hash) return;

    for(size_t i=0;i<hash->largo;i++)
    {
        lista_t* lista = hash->vector[i];
        if(!lista) continue;

        while(!lista_esta_vacia(lista))
        {
            nodo_hash_t* nodo = lista_borrar_primero(lista);
            if(hash->destruir_dato != NULL)
                hash->destruir_dato(nodo->dato);

            free(nodo->clave);
            free(nodo);
        }
        free(lista);
    }
    free(hash->vector);
    free(hash);
}

/* Iterador del hash */

/* Itera el vector del Hash desde la posicion actual del iterador hasta la proxima posicion != NULL */
bool buscar_proxima_lista(hash_iter_t *hash_iter) {
    hash_iter->actual = hash_iter->hash->vector[hash_iter->posicion_actual];

    while(!hash_iter->actual && hash_iter->posicion_actual < hash_iter->hash->largo-1)
        hash_iter->actual = hash_iter->hash->vector[++hash_iter->posicion_actual];

    if(!hash_iter->actual || hash_iter->posicion_actual == hash_iter->hash->largo)
        return false;
    return true;
}

/* Crea un iterador del Hash */
hash_iter_t *hash_iter_crear(const hash_t *hash) {
    if(!hash) return NULL;

	hash_iter_t *hash_iter = malloc(sizeof(hash_iter_t));
	hash_iter->posicion_actual = 0;
	hash_iter->actual = NULL;
	hash_iter->hash = hash;
	hash_iter->numero_lista_actual = 1;
    hash_iter->items_recorridos = 1;

    if(!hash_iter) return NULL;

    if(hash_cantidad(hash) != 0)
    {
        if(!buscar_proxima_lista(hash_iter)) return NULL;

        hash_iter->lista_iter = lista_iter_crear(hash_iter->actual);

        if(!hash_iter->lista_iter)
        {
            free(hash_iter);
            return NULL;
        }
    }
    else
        hash_iter->lista_iter = NULL;

    return hash_iter;
}

/* Comprueba si terminó la iteración */
bool hash_iter_al_final(const hash_iter_t *hash_iter) {
    if(!hash_iter) return NULL;
    return hash_iter->items_recorridos > hash_iter->hash->tam;
}

/* Avanza el iterador a la proxima posicion valida de ser posible */
bool hash_iter_avanzar(hash_iter_t *hash_iter) {
    if(!hash_iter) return false;

    // Iterador al final del hash, nada mas para iterar.
    if(hash_iter_al_final(hash_iter)) return false;

    // 1 - No hay iterador de lista, SOLO SI EL HASH NO TIENE ELEMENTOS.
    if(!hash_iter->lista_iter) return false;

    // Todas las acciones de aca en delante, incrementan en uno los items recorridos.
    hash_iter->items_recorridos++;

    // 2 - Iterador avanza en la lista, pero no esta al final
    if(lista_iter_avanzar(hash_iter->lista_iter) && !lista_iter_al_final(hash_iter->lista_iter))
        return true;

    // 3 - Se termino la lista que estaba iterando, buscar la proxima de ser posible.
    hash_iter->posicion_actual = hash_iter->posicion_actual + 1;

    if(!buscar_proxima_lista(hash_iter))
        return false; // No existe otra lista para iterar.

    // Si se encontro otra lista, destruir el iterador actual y crear uno nuevo.
    lista_iter_destruir(hash_iter->lista_iter);
    hash_iter->lista_iter = lista_iter_crear(hash_iter->actual);
    hash_iter->numero_lista_actual++;

    return true;
}

/* Devuelve clave actual, esa clave no se puede modificar ni liberada */
const char *hash_iter_ver_actual(const hash_iter_t *hash_iter) {
    if(!hash_iter || hash_iter_al_final(hash_iter))
        return NULL;

    const nodo_hash_t* nodo = lista_iter_ver_actual(hash_iter->lista_iter);
    if(!nodo) return NULL;
    return nodo->clave;
}

/* Destruye iterador */
void hash_iter_destruir(hash_iter_t* hash_iter) {
    if(!hash_iter) return;
    lista_iter_destruir(hash_iter->lista_iter);
    free(hash_iter);
}

/* Ajustar memoria necesaria para el vector del Hash */
bool hash_redimensionar(hash_t* hash) {

    // Esta variable evita la redimension cuando se estan guardando los nuevos elementos mientras se redimensiona
    if(hash->redimensionando) return true;

    size_t nuevo_largo = NULL;
    double factor_carga = (double)hash->tam / (double)hash->largo;

    if(factor_carga >= FACTOR_CARGA_MAXIMO)
        nuevo_largo = hash->tam + (size_t) ( (double)hash->tam * AUMENTO_LIBRE );

    else if(factor_carga < FACTOR_CARGA_MINIMO && hash->largo > LARGO_INICIAL)
        nuevo_largo = hash->tam - (size_t) ( (double)hash->tam * REDUCCION_LIBRE );

    if(!nuevo_largo) return true;

    void* nuevo_vector = malloc(sizeof(void*) * nuevo_largo);

    if (!nuevo_vector)
        return false;

    vector_limpiar(nuevo_vector, nuevo_largo);

    // Idea (1) de como redimensionar un Hash
    // Recorrer el VECTOR ACTUAL del HASH
    // Si la POSICION es != NLL pedir CLAVE del PRIMER dato de la LISTA (Del primer, segundo, o ultimo; total la funcion hashear aplicado a cualquiera deberia ser el mismo resultado)
    // Hashear esa CLAVE para ver la nueva POSICION dentro del VECTOR NUEVO
    // Colorcar el puntero a esa LISTA en la POSICION correspondiente en VECTOR NUEVO
    // Repetir VECTOR ACTUAL LARGO veces
    // Apuntar el VECTOR ACTUAL del HASH al VECTOR NUEVO

    lista_t* lista = lista_crear();

    for(int i = 0; i<hash->largo; i++){
        if(hash->vector[i] != NULL){
            lista_insertar_ultimo(lista,hash->vector[i]);
        }
    }

    free(hash->vector);

    hash->vector = nuevo_vector;
    hash->largo = nuevo_largo;
    hash->tam = 0;
    hash->redimensionando = true;


    while(!lista_esta_vacia(lista))
    {
        lista_t* lista_hash = lista_borrar_primero(lista);
        while(!lista_esta_vacia(lista_hash))
        {
            nodo_hash_t* nodo = lista_borrar_primero(lista_hash);
            hash_guardar(hash,nodo->clave,nodo->dato);
            free(nodo->clave);
            free(nodo);

        }
        free(lista_hash);
    }

    hash->redimensionando = false;
    free(lista);
    return true;
}
