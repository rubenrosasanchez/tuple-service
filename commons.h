//
// Created by ruben on 26/03/23.
//

#ifndef SOCKETS_COMMONS_H
#define SOCKETS_COMMONS_H

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define MSG_CHAR_SIZE 255

#define INIT_CODE 1
#define SET_VALUE_CODE 2
#define GET_VALUE_CODE 3
#define MODIFY_VALUE_CODE 4
#define DELETE_KEY_CODE 5
#define EXIST_CODE 6
#define COPY_KEY_CODE 7

typedef struct {
    char op;

    int32_t key;
    char value1[MSG_CHAR_SIZE + 1];
    int32_t value2;
    double value3;
} message;

void displayTuple(message m);

int receive_attribute(int socket, char * buffer, int len);

int send_attribute(int socket, char * buffer, int len);

#endif //SOCKETS_COMMONS_H
