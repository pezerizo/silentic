#ifndef _S_INET
#define _S_INET

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") // Link the Windows Sockets library

#include "structs.h"

// -----------------------------------------------

int sicInitWSA(struct _sic_server_data*);
int createSocketTCPWin(struct _sic_server_data*);
//int createSocketUDPWin();
int bindSocketWin(struct _sic_server_data*);
int listenSocketWin(struct _sic_server_data*);
int acceptSocketWin(struct _sic_server_data*, struct _sic_session_data*);

int closeSocketWin(struct _sic_server_data*);
void freeServerData(struct _sic_server_data*);
void freeSessionData(struct _sic_session_data*);

int sicDestroyWSA();

// -----------------------------------------------

int sicInitWSA(struct _sic_server_data* _server){
    if(WSAStartup(MAKEWORD(2, 2), &(_server->wsa)) != 0){
        #ifdef _DEBUG_
            printf("\tWSA init errorr\n");
        #endif
        sicDestroyWSA();
        return 1;
    }
    return 0;
}

int createSocketTCPWin(struct _sic_server_data* _server){
    _server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_server->server_socket == INVALID_SOCKET){
        #ifdef _DEBUG_
            printf("\t_server->server_socket creating error\n");
        #endif
        sicDestroyWSA();
        return 1;
    }
    return 0;
}

int bindSocketWin(struct _sic_server_data* _server){
    _server->server_addr.sin_family = AF_INET; 
    _server->server_addr.sin_addr.s_addr = INADDR_ANY;
    _server->server_addr.sin_port = htons(_server->port);

    if(bind(_server->server_socket, (const struct sockaddr*)&(_server->server_addr), sizeof(_server->server_addr)) == SOCKET_ERROR){
        #ifdef _DEBUG_
            printf("\t_server->server_socket bind error\n");
        #endif
        sicDestroyWSA();
        return 1;
    }
    return 0;
}

int listenSocketWin(struct _sic_server_data* _server){
    if (listen(_server->server_socket, _server->conn_count) == SOCKET_ERROR) {
        #ifdef _DEBUG_
            printf("\t_server->server_socket listen error\n");
        #endif
        sicDestroyWSA();
        return 1;
    }
    return 0;
}

int acceptSocketWin(struct _sic_server_data* _server, struct _sic_session_data* _sessions){
    while (1) {
        unsigned int caddr_size = sizeof(_sessions->client_addr);
        _sessions->client_socket = accept(_server->server_socket, (struct sockaddr*)&(_sessions->client_addr), &caddr_size);
        if (_sessions->client_socket == INVALID_SOCKET) {
            #ifdef _DEBUG_
                printf("Failed to accept incoming connection\n");
            #endif
            continue;
        }

        printf("Client connected: %s:%d\n", inet_ntoa(_sessions->client_addr.sin_addr), ntohs(_sessions->client_addr.sin_port));


        while ((_sessions->bytes_received = recv(_sessions->client_socket, _sessions->in_buffer, 1024, 0)) > 0) {
            _sessions->in_buffer[_sessions->bytes_received] = '\0';
            printf("Received data from client: %s\n", _sessions->in_buffer);

        }

        if (_sessions->bytes_received == 0) {
            printf("Connection closed by the client\n");
        } else {
            printf("Failed to receive data from the client\n");
        }

        closesocket(_sessions->client_socket);
    }
    closesocket(_server->server_socket);
    sicDestroyWSA();
    return 0;
}

int sicDestroyWSA(){
    WSACleanup();
    return 0;
}

#endif