#ifndef SOCKETS_KEYS_H
#define SOCKETS_KEYS_H

#include "../commons.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>

#define SERVER_PORT_ENV "PORT_TUPLES"
#define SERVER_ADDRESS_ENV "IP_TUPLES"


/*
    Inicializa el servicio destruyendo todas las tuplas que guardadas previamente.
    @Devuelve:
        0    Éxito
        -1   Error
 */
int init();

int set_value(int key, char * value1, int value2, double value3);

int get_value(int key, char * value1, int * value2, double * value3);

int modify_value(int key, char * value1, int value2, double value3);

int delete_key(int key);

/*
    @Devuelve:
        0       No existe la node
        1       Existe la node
        -1      Error
 */
int exist(int key);

int copy_key(int key1, int key2);


#endif //SOCKETS_KEYS_H
