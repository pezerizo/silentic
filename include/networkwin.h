#ifndef _S_INET_
#define _S_INET_

#define _DEBUG_

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

int closeServerSocketWin(struct _sic_server_data*);
int closeSessionSocketWin(struct _sic_session_data*);
void freeServerData(struct _sic_server_data*);
struct _sic_session_data* freeSessionData(struct _sic_session_data*);
int freeSessionsData(struct _sic_session_data*);

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
        unsigned int caddr_size = sizeof(_sessions->client_addr);
        _sessions->client_socket = accept(_server->server_socket, (struct sockaddr*)&(_sessions->client_addr), &caddr_size);
        if (_sessions->client_socket == INVALID_SOCKET) {
            #ifdef _DEBUG_
                printf("Failed to accept incoming connection\n");
            #endif
            return 1;
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

    return 0;
}


int closeServerSocketWin(struct _sic_server_data* _server){
    if(closesocket(_server->server_socket) == SOCKET_ERROR){
        #ifdef _DEBUG_
            printf("Failed to close server socket\n");
        #endif
        return 1;
    }
    return 0;
}


int closeSessionSocketWin(struct _sic_session_data* _session){
        if(closesocket(_session->client_socket) == SOCKET_ERROR){
        #ifdef _DEBUG_
            printf("Failed to close client socket\n");
        #endif
        return 1;
    }
    return 0;
}


void freeServerData(struct _sic_server_data* _server){
    closeServerSocketWin(_server);
    free(_server);
}


struct _sic_session_data* freeSessionData(struct _sic_session_data* _session){
    struct _sic_session_data* temp;
    if(_session->next == NULL && _session->prev == NULL){
        closeSessionSocketWin(_session);
        free(_session);
        #ifdef _DEBUG_
            printf("Single session deleted\n");
        #endif
        return NULL;
    }else if(_session->next != NULL && _session->prev != NULL){
        struct _sic_session_data* temp = _session->next;
        _session->prev->next = temp;
        _session->next->prev = _session->prev;
        temp = _session->prev;
        #ifdef _DEBUG_
            printf("Middle session deleted\n");
        #endif
        closeSessionSocketWin(_session);
        free(_session);
    }else if(_session->next == NULL && _session->prev != NULL){
        _session->prev->next = NULL;
        temp = _session->prev;
        #ifdef _DEBUG_
            printf("First session deleted\n");
        #endif
        closeSessionSocketWin(_session);
        free(_session);
    }else if(_session->prev == NULL && _session->next != NULL){
        _session->next->prev = NULL;
        temp = _session->next;
        #ifdef _DEBUG_
            printf("Last session deleted\n");
        #endif
        closeSessionSocketWin(_session);
        free(_session);
    }
    return temp;
}

int freeSessionsData(struct _sic_session_data* _sessions){
    struct _sic_session_data* temp;
    while(temp != NULL){
        temp = freeSessionData(_sessions);
    }
    return 0;
}

int sicDestroyWSA(){
    WSACleanup();
    return 0;
}

#endif