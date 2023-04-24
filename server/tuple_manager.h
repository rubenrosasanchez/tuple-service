//
// Created by ruben on 28/03/23.
//

#ifndef SOCKETS_TUPLE_MANAGER_H
#define SOCKETS_TUPLE_MANAGER_H


#include "../commons.h"
#include <stdlib.h>

struct node {
    int clave;
    char valor1[MSG_CHAR_SIZE];
    int valor2;
    double valor3;

    struct node *next;
};


void empty_list();
// Returns 0 on success & -1 on error
char insert_tuple(int clave, char * v1, int v2, double v3);
// Returns tuple on success and NULL on error
struct node * find_tuple(int clave);
// Returns 0 on success & -1 on error
char delete_tuple(int clave);

#endif //SOCKETS_TUPLE_MANAGER_H
