//
// Created by ruben on 8/04/23.
//

#include <errno.h>
#include "server_messaging.h"

void send_response(message msg, int client_descriptor, int msg_code) {
    // Send feedback code
    if(send(client_descriptor, &msg.op, sizeof(msg.op), 0) == -1) {
        fprintf(stderr, "Error sending feedback code: %s\n", strerror(errno));
        return;
    }

    if(msg_code == GET_VALUE_CODE) {
        // Send value 1
        if(send(client_descriptor, msg.value1, MSG_CHAR_SIZE + 1, 0) == -1) {
            fprintf(stderr, "Error sending value 1: %s\n", strerror(errno));
            return;
        }
        // Send value 2
        uint32_t v2 = htonl(msg.value2);
        if(send(client_descriptor, &v2, sizeof(v2), 0) == -1) {
            fprintf(stderr, "Error sending value 2: %s\n", strerror(errno));
            return;
        }
        // Send value 3
        char str_v3[MSG_CHAR_SIZE];
        sprintf(str_v3, "%lf", msg.value3);
        if(send(client_descriptor, str_v3, MSG_CHAR_SIZE + 1, 0) == -1) {
            fprintf(stderr, "Error sending value 3: %s\n", strerror(errno));
            return;
        }
    }
}

int receive_request(message * msg, int client_descriptor) {
    // receive operation code
    if(recv(client_descriptor, &msg->op, sizeof(msg->op), 0) == -1) {
        return -1;
    }

    if(msg->op != INIT_CODE) {
        // Receive key
        uint32_t k = htonl(msg->key);
        if(recv(client_descriptor, &k, sizeof(k), 0) == -1) {
            fprintf(stderr, "Error receiving key: %s\n", strerror(errno));
            return -1;
        }
        msg->key = (int32_t) ntohl(k);

        if(msg->op == SET_VALUE_CODE || msg->op == MODIFY_VALUE_CODE) {
            // Receive value 1
            if(recv(client_descriptor, &msg->value1, MSG_CHAR_SIZE + 1, 0) == -1) {
                fprintf(stderr, "Error receiving value 1: %s\n", strerror(errno));
                return -1;
            }

            // Receive value 2
            uint32_t v2 = htonl(msg->value2);
            if(recv(client_descriptor, &v2, sizeof(v2), 0) == -1) {
                fprintf(stderr, "Error receiving value 2: %s\n", strerror(errno));
                return -1;
            }
            msg->value2 = (int32_t) ntohl(v2);

            // Receive value 3
            char str_v3[MSG_CHAR_SIZE+1];
            if(recv(client_descriptor, str_v3, MSG_CHAR_SIZE+1, 0) == -1) {
                fprintf(stderr, "Error receiving value 3: %s\n", strerror(errno));
                return -1;
            }
            sscanf(str_v3, "%lf", &msg->value3);
        }

        if(msg->op == COPY_KEY_CODE) {
            // Receive key 2
            uint32_t k2 = 69;
            if(recv(client_descriptor, &k2, sizeof(k2), 0) == -1) {
                fprintf(stderr, "Error receiving key 2: %s\n", strerror(errno));
                return -1;
            }
            msg->value2 = (int32_t) ntohl(k2);
        }
    }

    return 0;
}






