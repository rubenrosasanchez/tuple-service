//
// Created by ruben on 28/03/23.
//

#include "tuple_manager.h"
#include <string.h>
#include <stdbool.h>

struct node *head = NULL;

// Returns 0 on success & -1 on error
char insert_tuple(int clave, char * v1, int v2, double v3) {

    if(find_tuple(clave) != NULL){
        return -1;
    }

    struct node *link = (struct node *) malloc(sizeof(struct node));

    link->clave = clave;
    strcpy(link->valor1, v1);
    link->valor2 = v2;
    link->valor3 = v3;

    link->next = head;
    head = link;

    return 0;
}

bool isEmpty() {
    return head == NULL;
}

void empty_list(){
    struct node * current = head;
    struct node * prev = NULL;

    while(current != NULL) {
        prev = current;
        current = current->next;
        free(prev);
    }
}

int numTuplas(){
    int tam = 0;
    struct node * current;

    for(current = head; current != NULL; current = current->next){
        tam++;
    }
    return tam;
}

// Returns tuple on success and NULL on error
struct node * find_tuple(int clave) {
    struct node *current = head;

    if(head == NULL) {
        return NULL;
    }

    while(current->clave != clave) {
        // If we're on the last element
        if (current->next == NULL) {
            return NULL;
        } else {
            current = current->next;
        }
    }

    return current;
}

// Returns 0 on success & -1 on error
char delete_tuple(int clave) {
    struct node *current = head;
    struct node *previous = head;

    if (head == NULL) {
        return -1;
    }

    while(current->clave != clave) {
        // If we're on the last element
        if (current->next == NULL) {
            return -1;
        } else {
            previous = current;
            current = current->next;
        }
    }

    if(current == head) {
        head = head->next;
    } else {
        previous->next = current->next;
    }

    free(current);
    return 0;
}




