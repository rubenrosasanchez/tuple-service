//
// Created by ruben on 8/04/23.
//

#ifndef SOCKETS_SERVER_MESSAGING_H
#define SOCKETS_SERVER_MESSAGING_H

#include "../commons.h"

void send_response(message msg, int client_descriptor, int msg_code);

int receive_request(message * msg, int client_descriptor);

#endif //SOCKETS_SERVER_MESSAGING_H
