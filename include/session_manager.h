#ifndef _S_INET_
#define _S_INET_

#define _DEBUG_

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib") // Link the Windows Sockets library

#include "structs.h"
#include "md5.h"

// -----------------------------------------------

int sicInitWSA(struct _sic_server_data*);
int sicDestroyWSA();

int createSocketTCPWin(struct _sic_server_data*);
//int createSocketUDPWin();
int bindSocketWin(struct _sic_server_data*);
int listenSocketWin(struct _sic_server_data*);
int acceptSocketWin(struct _sic_server_data*, struct _sic_session_data*);
int closeServerSocketWin(struct _sic_server_data*); // called in freeServerData()
int closeSessionSocketWin(struct _sic_session_data*); // called in freeSessionData() and freeSessionsData()

struct _sic_session_data* searchEmptySession(struct _sic_server_data*, struct _sic_session_data*);
struct _sic_session_data* createSession(struct _sic_server_data*, struct _sic_session_data*);
struct _sic_session_data* initSessionStack(struct _sic_server_data*, struct _sic_session_data*);
void freeServerData(struct _sic_server_data*);
struct _sic_session_data* freeSessionData(struct _sic_session_data*);
int freeSessionsData(struct _sic_session_data*);

void generateSessionID(struct _sic_session_data*);
// Create threads for client/session
// Destroy threads for client/session

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


int acceptSocketWin(struct _sic_server_data* _server, struct _sic_session_data* _session){
        createSession(_server, _session);

        printf("Session established | connected: %s:%d -> server | session id %s\n", inet_ntoa(_session->client_addr.sin_addr), ntohs(_session->client_addr.sin_port), _session->id);


        while ((_session->bytes_received = recv(_session->client_socket, _session->in_buffer, 1024, 0)) > 0) {
            _session->in_buffer[_session->bytes_received] = '\0';
            printf("Received data from session | session id: %s:\t%s\n", _session->id, _session->in_buffer);
        }

        if (_session->bytes_received == 0) {
            printf("Connection closed by the client | session id: %s\n", _session->id);
        } else {
            printf("Failed to receive data from the client | session id: %s\n", _session->id);
        }

    return 0;
}

struct _sic_session_data* initSessionStack(struct _sic_server_data* _server, struct _sic_session_data* _session){
    struct _sic_session_data* _start;
    struct _sic_session_data* temp;
    for(size_t iter = 0; iter < _server->conn_count; ++iter){
        _session = calloc(1, sizeof(struct _sic_session_data));
        if(iter == 0){
            _session->prev = NULL;
            _session->next = NULL;
            _session->start = _session;
            _start = _session;
            temp = _session;
            _session = _session->next;
        }else{
            _session->prev = temp;
            _session->next = NULL;
            _session->prev->next = _session;
            _session->start = _start;
            temp = _session;
            _session = _session->next;
        }
    }

    _session = _start;
    for(int iter = 0; iter < _server->conn_count; ++iter){
        #ifdef _DEBUG_
        printf("[%d] Session %p | start %p | next %p | prev %p\n", iter, _session, _session->start, _session->next, _session->prev);
        #endif
        _session = _session->next;
    }
    _session = _start;
    return _session;
}


struct _sic_session_data* searchEmptySession(struct _sic_server_data* _server, struct _sic_session_data* _session){
    _session = _session->start;
    for(int iter = 0; iter < _server->conn_count; ++iter){
        if(_session->client_socket == 0) return _session;
        _session = _session->next;
    }

    #ifdef _DEBUG_
        printf("Session stack is full\n");
    #endif
    return NULL;
}


struct _sic_session_data* createSession(struct _sic_server_data* _server, struct _sic_session_data* _session){
    _session = searchEmptySession(_server, _session);
    if(_session == NULL) return NULL;

    if(_session->client_socket == 0){
        unsigned int caddr_size = sizeof(_session->client_addr);
        _session->client_socket = accept(_server->server_socket, (struct sockaddr*)&(_session->client_addr), &caddr_size);
        if (_session->client_socket == INVALID_SOCKET) {
            #ifdef _DEBUG_
                printf("Failed to accept incoming connection\n");
            #endif
            return _session;
        }
        generateSessionID(_session);
        #ifdef _DEBUG_
            printf("Session created with id: %s\n", _session->id);
        #endif
    }

    return _session;
}

void generateSessionID(struct _sic_session_data* _session){
    // Concatenate the previous ID with the additional data
    char combinedInput[33]; // Make sure it's large enough to hold both strings
    if(_session->prev != NULL){
       snprintf(combinedInput, sizeof(combinedInput), "%s", _session->prev->id); 
    }else{
       snprintf(combinedInput, sizeof(combinedInput), "%s", "empty");  
    }

    // Generate an MD5 hash of the combined input to get the new ID
    char md5Output[MD5_DIGEST_LENGTH]; // 16 bytes for MD5 hash
    generateMD5(combinedInput, sizeof(combinedInput), md5Output);

    // Convert the binary MD5 hash to a human-readable string format for the new ID
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        snprintf(&(_session->id[i * 2]), 3, "%02x", (unsigned int)md5Output[i]);
    }
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

int freeSessionsData(struct _sic_session_data* _session){
    struct _sic_session_data* temp = _session;
    while(temp != NULL){
        temp = freeSessionData(temp);
    }
    return 0;
}

int sicDestroyWSA(){
    WSACleanup();
    return 0;
}

#endif