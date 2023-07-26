#ifndef _S_INET
#define _S_INET

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") // Link the Windows Sockets library

#include "structs.h"


int initWSA(struct _sic_session_data*);
struct _sic_session_data* createSocketTCPWin(struct _sic_session_data*);
struct _sic_session_data* createSocketUDPWin(struct _sic_session_data*);

int closeSocketWin(struct _sic_session_data*);
void freeSessionData();

int destroyWSA(struct _sic_session_data*);

int initWSA(struct _sic_session_data* _session){
    if(WSAStartup(MAKEWORD(2, 2), &(_session->wsa)) != 0){
        return 1;
    }
    return 0;
}

int createSocketTCPWin(struct _sic_session_data* _session);

/*

struct _s_session_data* createSocket(){
    struct _s_session_data* _session = malloc(sizeof(struct _s_session_data));
    if(_session == NULL){
        printf("Falied to allocate memory for _s_session_data");
        return NULL;
    }

    if(WSAStartup(MAKEWORD(2, 2), &(_session->wsa)) != 0){
        printf("Failed to initialize Winsock\n");
        return NULL;
    }

    _session->server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_session->server_socket == INVALID_SOCKET){
        printf("Failed to create socket\n");
        return NULL;
    }

    _session->server_addr.sin_family = AF_INET;
    _session->server_addr.sin_addr.s_addr = INADDR_ANY;
    _session->server_addr.sin_port = htons(55555);

    if(bind(_session->server_socket, (struct sockaddr*)&(_session->server_addr), sizeof(_session->server_addr)) == SOCKET_ERROR){
        printf("Failed to bind socket\n");
        return NULL;
    }

    if (listen(_session->server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Failed to listen for incoming connections\n");
        closesocket(_session->server_socket);
        WSACleanup();
        return NULL;
    }

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    SOCKET client_socket;
    int bytes_received;

    while (1) {
        client_socket = accept(_session->server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Failed to accept incoming connection\n");
            continue;
        }

        printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        while ((bytes_received = recv(client_socket, _session->in_buffer, 1024, 0)) > 0) {
            _session->in_buffer[bytes_received] = '\0';
            printf("Received data from client: %s\n", _session->in_buffer);

        }

        if (bytes_received == 0) {
            printf("Connection closed by the client\n");
        } else {
            printf("Failed to receive data from the client\n");
        }

        closesocket(client_socket);
    }
    return _session;
}

int closeSocket(struct _s_session_data* _session){
    closesocket(_session->server_socket);
    WSACleanup();
    free(_session);
    return 0;
}
*/

#endif