
#include <stdbool.h>
#include <stddef.h>
#include "vector_dinamico.h"
#include "lista.h"
#include "pila.h"
#define LARGO_INICIAL 30


vector_dinamico_t *crear_vector_dinamico();

// Los structs deben llamarse "hash" y "hash_iter".
struct hash {
    size_t tam; /*cantidad de elementos*/
    size_t largo; /*cantidad memoria*/
    void (*destruir_dato);
    vector_t *vector;
};

struct hash_iter{

};
// tipo de funci�n para destruir dato
typedef void (*hash_destruir_dato_t)(void *);

/* Crea el hash
 */
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

/*Crea un vector dinamico*/
vector_dinamico_t *crear_vector_dinamico(){
	vector_t *vector = vector_crear(LARGO_INICIAL);
	if(!vector)
		return NULL;
	return vector_dinamico;
}

/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacen� el par (clave, dato)
 IMPORTANTE: COPIAR CLAVE (para que no te la modifique el usuario)
 */
bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	size_t clave_hasheada = hashear(clave,hash->largo);
	hash->vector[clave_hasheada] = dato;
	return true;
}

/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvi�,
 * en el caso de que estuviera guardado.
 IMPORTANTE: DSTRUIR DATO SINO ES NULL
 */
void *hash_borrar(hash_t *hash, const char *clave);

/* Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *hash_obtener(const hash_t *hash, const char *clave);

/* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
bool hash_pertenece(const hash_t *hash, const char *clave);

/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
 */
size_t hash_cantidad(const hash_t *hash){
	return hash->tam;
}

/* Destruye la estructura liberando la memoria pedida y llamando a la funci�n
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash);

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

// Comprueba si termin� la iteraci�n
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