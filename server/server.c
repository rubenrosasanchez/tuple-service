//
// Created by ruben on 28/03/23.
//

#include <pthread.h>
#include <signal.h>
#include <stdbool.h>

#include "tuple_manager.h"
#include "server_messaging.h"

pthread_attr_t thread_attributes;
pthread_mutex_t single_list_lock;
int socket_descriptor;

// To make sure file descriptor is properly passed
bool busy = true;
pthread_mutex_t thread_fd_lock;


void init(int client) {
    message ret;
    ret.op = 0;
    //printf("Initiating database after calling init()...\n");

    pthread_mutex_lock(&single_list_lock);
    empty_list();
    pthread_mutex_unlock(&single_list_lock);

    send_response(ret, client, INIT_CODE);
}

void setValue(message msg, int client) {
    message ret;
    ret.op = 0;

    pthread_mutex_lock(&single_list_lock);
    if(insert_tuple(msg.key, msg.value1, msg.value2, msg.value3)) { ret.op = -1;}
    pthread_mutex_unlock(&single_list_lock);

    send_response(ret, client, SET_VALUE_CODE);
}

void getValue(message msg, int client) {
    message ret;
    ret.op = 0;
    ret.key = msg.key;

    pthread_mutex_lock(&single_list_lock);
    struct node *tp = find_tuple(msg.key);
    if(tp != NULL) {
        ret.value3 = tp->valor3;
        ret.value2 = tp->valor2;
        strcpy(ret.value1, tp->valor1);
    } else {
        ret.op = -1;
    }
    pthread_mutex_unlock(&single_list_lock);

    send_response(ret, client, GET_VALUE_CODE);
}

void modifyValue(message msg, int client) {
    message ret;
    ret.op = 0;

    pthread_mutex_lock(&single_list_lock);
    struct node *tp = find_tuple(msg.key);
    if(tp == NULL) {
        ret.op = -1;
    } else {
        strcpy(tp->valor1, msg.value1);
        tp->valor2 = msg.value2;
        tp->valor3 = msg.value3;
    }
    pthread_mutex_unlock(&single_list_lock);

    send_response(ret, client, MODIFY_VALUE_CODE);
}

void deleteKey(message msg, int client) {
    message ret;
    ret.op = 0;
    msg.value3 = 0;
    msg.value2 = 0;
    strcpy(msg.value1, "");

    pthread_mutex_lock(&single_list_lock);
    ret.op = delete_tuple(msg.key);
    pthread_mutex_unlock(&single_list_lock);

    send_response(ret, client, DELETE_KEY_CODE);
}

void exist(message msg, int client) {
    message ret;
    ret.op = 0;

    pthread_mutex_lock(&single_list_lock);
    struct node *tp = find_tuple(msg.key);
    if(tp != NULL) {
        ret.op = 1;
    }
    pthread_mutex_unlock(&single_list_lock);

    send_response(ret, client, EXIST_CODE);
}

void copyKey(message msg, int client) {
    message ret;
    ret.op = 0;
    msg.value3 = 0;
    strcpy(msg.value1, "");

    pthread_mutex_lock(&single_list_lock);

    struct node *tuple_1 = find_tuple(msg.key);
    struct node *tuple_2 = find_tuple(msg.value2);

    // If tuple 1 does not exist we exit with error code
    if (tuple_1 == NULL) {
        ret.op = -1;
    } else {
        // If tuple with key 2 does not exist it is created
        if(tuple_2 == NULL) {
            if(insert_tuple(msg.value2, tuple_1->valor1, tuple_1->valor2, tuple_1->valor3)) {
                ret.op = -1;
            }
        } else { // And if it exists it is modified
            if(delete_tuple(tuple_2->clave)) {
                ret.op = -1;
            } else {
                ret.op = insert_tuple(tuple_2->clave, tuple_1->valor1, tuple_1->valor2, tuple_1->valor3);
            }
        }
    }
    pthread_mutex_unlock(&single_list_lock);

    send_response(ret, client, COPY_KEY_CODE);
}

/////////////////////////////////////////////////////////////////////

void * execute_operation(const int * sock_des) {
    int client_descriptor;

    pthread_mutex_lock(&thread_fd_lock);
    client_descriptor = *sock_des;
    busy = false;
    pthread_mutex_unlock(&thread_fd_lock);

    // Copy data just in case it's lost in the tuple servise loop before operating
    message msg;
    if(receive_request(&msg, client_descriptor)){
        fprintf(stderr, "Error receiving operation message");
    } else {
        switch (msg.op) {
            case INIT_CODE:
                init(client_descriptor);
                break;
            case SET_VALUE_CODE:
                setValue(msg, client_descriptor);
                break;
            case GET_VALUE_CODE:
                getValue(msg, client_descriptor);
                break;
            case MODIFY_VALUE_CODE:
                modifyValue(msg, client_descriptor);
                break;
            case DELETE_KEY_CODE:
                deleteKey(msg, client_descriptor);
                break;
            case EXIST_CODE:
                exist(msg, client_descriptor);
                break;
            case COPY_KEY_CODE:
                copyKey(msg, client_descriptor);
                break;
            default:
                break;
        }
    }
    pthread_exit(0);
}

void operate_tuple_service() {
    int client_descriptor;
    struct sockaddr_in client_addr;
    socklen_t size = sizeof(client_addr);

    pthread_t thread;
    pthread_attr_init(&thread_attributes);
    pthread_attr_setdetachstate(&thread_attributes, PTHREAD_CREATE_DETACHED);

    while(true) {
        client_descriptor = accept(socket_descriptor, (struct sockaddr *) &client_addr, &size);

        if(client_descriptor == -1) {
            fprintf(stderr, "Error accepting client connection");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&thread_fd_lock);
        busy = true;
        pthread_mutex_unlock(&thread_fd_lock);

        pthread_create(&thread, &thread_attributes, (void *(*)(void *)) execute_operation, &client_descriptor);

        while(busy){
            // Stop execution until the detached tread has copied the file descriptor.
        }
    }
}

void initiate_service(unsigned short port) {
    struct sockaddr_in server_addr;
    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_descriptor < 0){
        fprintf(stderr, "Error in socket creation.");
        exit(EXIT_FAILURE);
    }

    int val = 1;
    setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

    // Set struct to zero
    bzero((char *) &server_addr, sizeof(server_addr));

    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(port);

    if(bind(socket_descriptor, (const struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr, "Error in bind");
        exit(EXIT_FAILURE);
    }
    if(listen(socket_descriptor, SOMAXCONN) == -1){
        fprintf(stderr, "Error in listen");
        exit(EXIT_FAILURE);
    }

    operate_tuple_service();
}

/////////////////////////////////////////////////////////////////////

void stop_program_handler(){
    printf("\nStopping execution...\n");
    pthread_attr_destroy(&thread_attributes);
    pthread_mutex_destroy(&single_list_lock);
    close(socket_descriptor);
    exit(EXIT_SUCCESS);
}

int main(int argc, char ** argv) {

    signal(SIGINT, (__sighandler_t) stop_program_handler);

    if(argc != 2) {
        fprintf(stderr, "Invalid arguments.\n\tUsage: ./client <PORT_NUMBER>\n");
        return -1;
    }
    long int initial_port_input;
    unsigned short port_number = 0;
    // check proper number input
    initial_port_input = strtol(argv[1], NULL, 10);
    if ((initial_port_input > USHRT_MAX) || (initial_port_input < 1)) {
        fprintf(stderr, "Invalid port number");
        return -1;
    }
    port_number = (unsigned short) initial_port_input;

    if (pthread_mutex_init(&single_list_lock, NULL) != 0) {
        perror("\nError initializing mutex");
        return -1;
    }

    initiate_service(port_number);

    pthread_mutex_destroy(&single_list_lock);

    return 0;
}