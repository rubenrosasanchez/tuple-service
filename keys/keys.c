
#include "keys.h"

// Return 0 if successful, -1 otherwise
int get_ip_address(struct in_addr * in, struct hostent * hp){
    char * ip;

    ip = getenv(SERVER_ADDRESS_ENV);
    if(ip == NULL){
        fprintf(stderr, "IP_TUPLAS not defined: %s\n", strerror(errno));
        return -1;
    }
    bzero((char *) in, sizeof(*in));
    hp = gethostbyname(ip);
    if(hp == NULL){
        fprintf(stderr, "gethostbyname error: %s\n", strerror(errno));
        return -1;
    }
    memcpy(&in->s_addr, *(hp->h_addr_list), sizeof(in->s_addr));
    return 0;
}

// Return 0 if successful, -1 otherwise
int get_port_number(unsigned short * portNum){
    char * port_input;
    port_input = getenv(SERVER_PORT_ENV);
    if(port_input == NULL){
        fprintf(stderr, "PORT_TUPLAS not defined: %s\n", strerror(errno));
        return -1;
    }

    long int port_number = strtol(port_input, NULL, 10);
    if((port_number > USHRT_MAX) || (port_number <= 0) || (errno == ERANGE) ){
        fprintf(stderr, "Wrong port input number: %s\n", strerror(errno));
        return -1;
    }
    *portNum = (unsigned short) port_number;
    return 0;
}

// Return 0 on success, -1 on error
int connect_to_server(int *socket_fd, struct sockaddr_in *server_address, unsigned short server_port) {

    *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*socket_fd == 1) {
        fprintf(stderr, "Error on socket creation\n");
        return -1;
    }
    struct sockaddr server;
    server.sa_family = server_address->sin_family;
    socklen_t size = sizeof(*server_address);
    memcpy(server.sa_data, server_address, size);

    if(connect(*socket_fd, (struct sockaddr *) server_address, size) < 0) {
        fprintf(stderr, "Error on connection to server\n");
        return -1;
    }
    return 0;
}

// Return 0 on success, -1 on error
int send_data_to_server(message msg, int socket_fd) {
    // send operation code
    if(send(socket_fd, &msg.op, sizeof(msg.op), 0) == -1){
        fprintf(stderr, "Error sending operation code: %s\n", strerror(errno));
        return -1;
    }

    // If it isn't the init function we have to send more data
    if(msg.op != INIT_CODE) {
        // Send key
        uint32_t  k = ntohl(msg.key);
        if(send(socket_fd, &k, sizeof(k), 0) == -1) {
            fprintf(stderr, "Error sending key: %s\n", strerror(errno));
            return -1;
        }

        if(msg.op == SET_VALUE_CODE || msg.op == MODIFY_VALUE_CODE) {
            // Send value 1
            if(send(socket_fd, &msg.value1, MSG_CHAR_SIZE + 1, 0) == -1) {
                fprintf(stderr, "Error sending value 1: %s\n", strerror(errno));
                return -1;
            }
            // Send value 2
            uint32_t v2 = htonl(msg.value2);
            if(send(socket_fd, &v2, sizeof(v2), 0) == -1) {
                fprintf(stderr, "Error sending value 2: %s\n", strerror(errno));
                return -1;
            }
            // Send value 3
            char str_v3[MSG_CHAR_SIZE];
            sprintf(str_v3, "%lf", msg.value3);
            if(send(socket_fd, str_v3, MSG_CHAR_SIZE+1, 0) == -1) {
                fprintf(stderr, "Error sending value 3: %s\n", strerror(errno));
                return -1;
            }
        }

        if(msg.op == COPY_KEY_CODE) {
            // Send key 2
            uint32_t k2 = htonl(msg.value2);
            if(send(socket_fd, &k2, sizeof(k2), 0) == -1) {
                fprintf(stderr, "Error sending key 2: %s\n", strerror(errno));
                return -1;
            }
        }
        //printf("Enviada la tupla con op = %d:\n\t", msg.op);
        //displayTuple(msg);
    }
    return 0;
}

message receive_data_from_server(char code, int socket_fd) {
    message response;
    response.op = 0;
    response.key = 0;
    response.value2 = 0;
    response.value3 = 0;

    // First receive feedback code
    if(-1 == recv(socket_fd, &response.op, sizeof(response.op), 0)) {
        fprintf(stderr, "Error receiving data: %s\n", strerror(errno));
        response.op = -1;
        return response;
    }

    if (code == GET_VALUE_CODE) {
        // Receive value 1
        if(-1 == recv(socket_fd, response.value1, MSG_CHAR_SIZE + 1, 0)) {
            fprintf(stderr, "Error receiving data: %s\n", strerror(errno));
            response.op = -1;
            return response;
        }

        // Receive value 2
        uint32_t v2;
        if(-1 == recv(socket_fd, &v2, sizeof(v2), 0)) {
            fprintf(stderr, "Error receiving data: %s\n", strerror(errno));
            response.op = -1;
            return response;
        }
        response.value2 = (int32_t) ntohl(v2);

        // Receive value 3
        char str_v3[MSG_CHAR_SIZE+1];
        if(-1 == recv(socket_fd, str_v3, MSG_CHAR_SIZE+1, 0)) {
            fprintf(stderr, "Error receiving data: %s\n", strerror(errno));
            response.op = -1;
            return response;
        }
        sscanf(str_v3, "%lf", &response.value3);
        //printf("Recibida la tupla con code = %d:\n\t", code);
        //displayTuple(response);
    }
    return response;
}

// Sends the message argument to the server and returns the returned message  by server
message communicate_with_server(message msg) {
    // To communicate
    int socket_fd;
    struct sockaddr_in server_address;
    struct in_addr ip_server;
    struct hostent hp;
    unsigned short port;

    // Message to return
    message response;
    response.op = 0;
    response.key = 0;
    response.value2 = 0;
    response.value3 = 0;

    // Get data from environment variables
    if(get_ip_address(&ip_server, &hp)){
        response.op = -1;
        return response;
    }
    if(get_port_number(&port)){
        response.op = -1;
        return response;
    }
    // Set data for connection
    server_address.sin_addr.s_addr = ip_server.s_addr;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if(connect_to_server(&socket_fd, &server_address, port) == -1) {
        response.op = -1;
        close(socket_fd);
        return response;
    }

    if(send_data_to_server(msg, socket_fd)) {
        response.op = -1;
        close(socket_fd);
        return response;
    }

    response = receive_data_from_server(msg.op, socket_fd);

    close(socket_fd);

    return response;
}

//////////////////////////////////////////////////////////////////////////////////////////

int init() {
    message m;
    m.op = INIT_CODE;
    m.key = 0;
    m.value2 = 0;
    m.value3 = 0;

    for (int i = 0; i < strlen(m.value1); ++i) {
        m.value1[i] = '\0';
    }

    return (int) communicate_with_server(m).op;
}

int set_value(int key, char * value1, int value2, double value3) {
    if(strlen(value1) > MSG_CHAR_SIZE) {
        return -1;
    }

    message m;
    m.op = SET_VALUE_CODE;
    m.key = key;
    strcpy(m.value1, value1);
    m.value2 = value2;
    m.value3 = value3;

    return (int) communicate_with_server(m).op;
}

int get_value(int key, char * value1, int * value2, double * value3) {
    message m;
    m.op = GET_VALUE_CODE;
    m.key = key;

    message res = communicate_with_server(m);

    strcpy(value1, res.value1);
    *value2 = res.value2;
    *value3 = res.value3;

    return (int) res.op;
}

int modify_value(int key, char * value1, int value2, double value3) {
    if(strlen(value1) > MSG_CHAR_SIZE) {
        return -1;
    }

    message m;
    m.op = MODIFY_VALUE_CODE;
    m.key = key;
    strcpy(m.value1, value1);
    m.value2 = value2;
    m.value3 = value3;

    return (int) communicate_with_server(m).op;
}

int delete_key(int key) {
    message m;
    m.op = DELETE_KEY_CODE;
    m.key = key;

    return (int) communicate_with_server(m).op;
}

/*
    @Devuelve:
        0       No existe la node
        1       Existe la node
        -1      Error
 */
int exist(int key) {
    message m;
    m.op = EXIST_CODE;
    m.key = key;
    m.value2 = 0;
    m.value3 = 69.420;

    return (int) communicate_with_server(m).op;
}

int copy_key(int key1, int key2) {
    if(key2 == key1) {
        return -1;
    }

    message m;
    m.op = COPY_KEY_CODE;
    m.key = key1;
    m.value2 = key2;

    return (int) communicate_with_server(m).op;
}
