//
// Created by ruben on 16/04/23.
//
#include "commons.h"

void displayTuple(message m) {
    printf("op: %d, key: %d,\t value1: %s,\t value2: %d,\t value3: %f\n", m.op, m.key, m.value1, m.value2, m.value3);
}

int receive_attribute(int socket, char * buffer, int len) {
    long r;
    long l = (long) len;

    do{
        r = read(socket, buffer, l);
        l = l-r;
        buffer = buffer + r;
    } while ( (l > 0) && (r >= 0) );

    if(r < 0){
        return -1;
    }
    return 0;
}

int send_attribute(int socket, char * buffer, int len) {
    long r;
    long l = (long) len;

    do{
        r = write(socket, buffer, l);
        l = l-r;
        buffer = buffer + r;
    } while ( (l > 0) && (r >= 0));

    if( r < 0){
        return -1;
    }
    return 0;
}