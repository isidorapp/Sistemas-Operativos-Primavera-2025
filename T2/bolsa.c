#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "bolsa.h"

// Declare aca sus variables globales
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

int hay_vendedor = 0;
int hay_comprador = 0;
int se_vendio = 0;

int mejor_precio = 0;

char vendedor_actual[100];
char comprador_actual[100];

int vendo(int precio, char *vendedor, char *comprador) {
    pthread_mutex_lock(&m);
// Si llego y no hay vendedor o soy mejor que el mejor precio actual y no se ha vendido, me copio y reemplazo
    if (hay_vendedor == 0 || (precio < mejor_precio && se_vendio == 0)) {
        strcpy(vendedor_actual, vendedor);
        mejor_precio = precio;
        pthread_cond_broadcast(&c);
        hay_vendedor = 1;
    }
    else {
        // Soy mayor al mejor precio
        pthread_mutex_unlock(&m);
        return 0;
    }
  
    // Mientras soy el mejor precio
    while (hay_comprador != 1 && (precio == mejor_precio)) {
        pthread_cond_wait(&c,&m);
    }
  
    // Si ya no soy el mejor precio
    if (precio != mejor_precio) {
        pthread_mutex_unlock(&m);
        return 0;
    }
    else {
        strcpy(comprador, comprador_actual);
        strcpy(comprador_actual,"");
        strcpy(vendedor_actual,"");
        mejor_precio = 0;
        hay_vendedor = 0;
        hay_comprador = 0;
        pthread_mutex_unlock(&m);
        return 1;
    }
  
}

int compro(char *comprador, char *vendedor) {
    pthread_mutex_lock(&m);
    int res;
  
    if (hay_vendedor == 0 || hay_comprador == 1) {
        pthread_mutex_unlock(&m);
        return 0;
    }
    else {
        hay_comprador = 1;
        se_vendio = 1;
        res = mejor_precio;
        strcpy(comprador_actual, comprador);
        strcpy(vendedor, vendedor_actual);
        pthread_cond_broadcast(&c);
        pthread_mutex_unlock(&m);
    }
    return res;
}

