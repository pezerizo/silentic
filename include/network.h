#ifndef _S_INET
#define _S_INET

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") // Link the Windows Sockets library

#include "structs.h"

int createSocket(){
    struct _s_session_data* _session = malloc(sizeof(struct _s_session_data));
    if(_session == NULL){
        printf("Falied to allocate memory for _s_session_data");
        return 1;
    }

    if(WSAStartup(MAKEWORD(2, 2), &(_session->wsa)) != 0){
        printf("Failed to initialize Winsock\n");
        return 1;
    }

    _session->server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_session->server_socket == INVALID_SOCKET){
        printf("Failed to create socket\n");
        return 1;
    }

    _session->server_addr.sin_family = AF_INET;
    _session->server_addr.sin_addr.s_addr = INADDR_ANY;
    _session->server_addr.sin_port = htons(8080);

    if(bind(_session->server_socket, (struct sockaddr*)&(_session->server_addr), sizeof(_session->server_addr)) == SOCKET_ERROR){
        printf("Failed to bind socket\n");
        return 1;
    }

    closesocket(_session->server_socket);
    WSACleanup();
    return 0;
}

#endif