
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

struct hash
{
    size_t tam; /* Cantidad de elementos en el vector*/
    size_t largo; /* Cantidad memoria del vector*/
    hash_destruir_dato_t destruir_dato; /* Funcion para destruir los datos */
    vector_t *vector; /* Arreglo (HashTable) para guardar las listas */
};

/* Nodo para guardar en la Lista */
typedef struct nodo_hash
{
    char* clave;
    void* dato;
} nodo_hash_t;

/* ... */
struct hash_iter
{
    lista_t* anterior;
    lista_t* actual;
    lista_iter_t* lista_iter;
    size_t posicion_actual;
    size_t tam_hash; // TODO: Innecesario.
    size_t numero_lista_actual;

};


/* Nodo para pasar como Extra en el iterador de la lista y comparar pertenencia o actualizar dato */
typedef struct nodo_auxiliar
{
    char* clave;
    void* dato;
    bool encontrado = false;
    bool actualizar = false;
    hash_destruir_dato_t destruir_dato;
} nodo_auxiliar_t;

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
    if(!vector)
    {
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
vector_t *crear_vector_dinamico()
{
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

    lista_t* lista = NULL;
    bool obtenido = vector_obtener(hash->vector, clave_hasheada, lista);

    // Si no puedo obtener que hay en esa posicion, no vale la pena seguir.
    if(!obtenido) return false;

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
        bool guardado = hash->vector_guardar(hash->vector, clave_hasheada, lista);

        if(!insertado || !guardado)
        {
            free(clave_copiada);
            free(nodo);

            if(creeLista)
                free(lista);

            return false;
        }

        hash->tam++;
    }
    else // Si la clave Pertenece, actualizar el valor usando nodo auxliar y lista iterar
    {
        // Redundante
        if(!lista) return false;

        nodo_auxiliar_t* auxiliar;
        auxiliar->clave = clave;
        auxiliar->dato = dato;
        auxiliar->actualizar = true;
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
void *hash_borrar(hash_t *hash, const char *clave);

/* Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *hash_obtener(const hash_t *hash, const char *clave);

/* Compara si la clave almacenada en el nodo_hash es igual a la que estamos buscando */
bool comparar_claves(void* nodo_hash, void* extra)
{
    nodo_hash_t* nodo = nodo_hash;
    nodo_auxiliar_t* auxliar = extra;

    if( strcmp(auxiliar->clave, nodo->clave) == 0)
    {
        auxiliar->encontrado = true;

        if(auxiliar->actualizar)
        {
            // TODO Destruir Dato
            auxliar->destruir_dato(nodo->dato);
            nodo->dato = auxiliar->dato;
        }

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

    // Si no logro obtener, no puedo asegurar que no pertenece!
    if(!obtenido) return NULL;

    if(!lista) return false;

    nodo_auxiliar_t* busqueda;
    busqueda->clave = clave;

    lista_iterar(lista, comparar_claves, busqueda);

    return busqueda->encontrado;
}

/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
 */
size_t hash_cantidad(const hash_t *hash)
{
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
hash_iter_t *hash_iter_crear(const hash_t *hash)
{
	hash_iter_t *hash_iter = malloc(sizeof(hash_iter_t));

    if(!hash_iter) return NULL;

    hash_iter->anterior = NULL;

    if(hash_cantidad(hash) == 0)
    {
        hash_iter->actual = NULL;
        return hash_iter;
    }

    /*
    caso inicial, posicionar el iter actual en la primer lista
    => Recorrer hash (vector con while) hasta la primer posicion.
    */

    size_t posicion_primer_lista = 0;
    lista_t* lista = NULL;
    bool obtenido;

    while(lista == NULL)
    {
        obtenido = vector_obtener(hash->vector, posicion_primer_lista, lista);
        posicion_primer_lista++;
        if(!obtenido) return NULL;
        // Posible loop infinito: Si hash_cantidad() deberia ser 0 y no lo es.
    }

    hash_iter->actual = lista;
    hash_iter->lista_iter = lista_iter_crear(hash_iter->actual);
    hash_iter->tam_hash = hash_cantidad(hash);
    return hash_iter;
}

// Avanza iterador
bool hash_iter_avanzar(hash_iter_t *hash_iter)
{
    // 1 - Iterador del hash al final.
    if(hash_iter_al_final(hash_iter)) return false;

    bool iter_lista_al_final = lista_iter_al_final(hash_iter->lista_iter);

    // 2 - Iterador del hash y de la lista al final.
    if(iter_lista_al_final && hash_iter->numero_lista_actual == hash->tam_hash-1)
        return false;


    if(!iter_lista_al_final && lista_iter->actual->prox != NULL)
        return lista_iter_avanzar(hash_iter->lista_iter);

    else
    {
        lista_t* lista = NULL;
        bool obtenido = vector_obtener(hash->vector, hash_iter->posicion_actual+1, lista);
        if(lista == NULL)
        {
            while(!lista)
            {
                bool obtenido = vector_obtener(hash->vector, primer_lista, lista);
                if(!obtenido)
                {
                    hash_iter->actual = NULL;
                    return hash_iter;
                }
                siguiente_posicion++;
            }
            hash_iter->numero_lista_actual++;
            hash_iter->anterior = hash_iter->actual;
            hash_iter->actual = hash->vector[siguiente_posicion];
            hash_iter->posicion_actual = siguiente_posicion;
            return true;
        }
    }
}

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
const char *hash_iter_ver_actual(const hash_iter_t *hash_iter)
{
    return hash_iter->lista_iter->actual->clave;
}

// Comprueba si terminó la iteración
bool hash_iter_al_final(const hash_iter_t *hash_iter)
{
    return hash_iter->actual == NULL;
}

// Destruye iterador
void hash_iter_destruir(hash_iter_t* hash_iter)
{
    free(hash_iter->lista_iter);
    free(iter)
}

size_t hashear(void *key, size_t largo)
{
	size_t hashAddress = 0;
	for (int counter = 0; word[counter]!='\0'; counter++)
    {
    	hashAddress = key[counter] + (hashAddress << 6) + (hashAddress << 16) - hashAddress;
	}
	hashAddress = hashAddress%largo < 0 ? hashAddress%largo * -1 : hashAddress%largo;
	return hashAddress;
}
