#ifndef _S_INET_
#define _S_INET_

#define _DEBUG_

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <pthread.h>
#pragma comment(lib, "ws2_32.lib") // Link the Windows Sockets library

#include "structs_manager.h"
#include "crypto_manager.h"

// -----------------------------------------------

int sicInitWSA(struct _sic_server_data*);
int sicDestroyWSA();

int createSocketTCPWin(struct _sic_server_data*);
//int createSocketUDPWin();
int bindSocketWin(struct _sic_server_data*);
int listenSocketWin(struct _sic_server_data*);
struct _sic_session_data* acceptSocketWin(struct _sic_server_data*, struct _sic_session_data*);
int closeServerSocketWin(struct _sic_server_data*); // called in freeServerData()
int closeSessionSocketWin(struct _sic_session_data*); // called in freeSessionData() and freeSessionsData()

//int initServer(struct _sic_server_data*);
int freeServer(struct _sic_server_data*);

struct _sic_session_data* initSession(struct _sic_server_data*, struct _sic_session_data*);
struct _sic_session_data* searchEmptySession(struct _sic_server_data*, struct _sic_session_data*);
struct _sic_session_data* establishSession(struct _sic_server_data*, struct _sic_session_data*);
int breakSession(struct _sic_server_data*, struct _sic_session_data*);
void freeSession(struct _sic_session_data* _session);
void freeSessionList(struct _sic_server_data*, struct _sic_session_data*);


void generateSessionID(struct _sic_session_data*);
void* start(void*);
//void stopSessionProcess(struct _sic_session_data*);
// Create threads for client/session
// Destroy threads for client/session

// -----------------------------------------------

void* start(void* _sess){
    struct _sic_session_data* _session = (struct _sic_session_data*)_sess;
    while ((_session->bytes_received = recv(_session->client_socket, _session->in_buffer, 1024, 0)) > 0) {
        _session->in_buffer[_session->bytes_received] = '\0';
        printf("Received data from session | session id: %s:\t%s\n", _session->id, _session->in_buffer);
    }

    if (_session->bytes_received == 0) {
        printf("Connection closed by the client | session id: %s\n", _session->id);
    } else {
        printf("Failed to receive data from the client | session id: %s\n", _session->id);
    }
    //_session->all_thread_count -= 1;
}

int sicInitWSA(struct _sic_server_data* _server){
    if (WSAStartup(MAKEWORD(2, 2), &(_server->wsa)) != 0){
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
    if (_server->server_socket == INVALID_SOCKET){
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

    if (bind(_server->server_socket, (const struct sockaddr*)&(_server->server_addr), sizeof(_server->server_addr)) == SOCKET_ERROR){
        #ifdef _DEBUG_
            printf("\t_server->server_socket bind error\n");
        #endif
        sicDestroyWSA();
        return 1;
    }

    return 0;
}

int listenSocketWin(struct _sic_server_data* _server){
    if (listen(_server->server_socket, _server->conn_allowed) == SOCKET_ERROR) {
        #ifdef _DEBUG_
            printf("\t_server->server_socket listen error\n");
        #endif
        sicDestroyWSA();
        return 1;
    }

    return 0;
}

struct _sic_session_data* acceptSocketWin(struct _sic_server_data* _server, struct _sic_session_data* _session){
    while(1){
        _session = establishSession(_server, _session);
        printf("Session established | connected: %s:%d -> server | session id %s\n", inet_ntoa(_session->client_addr.sin_addr), ntohs(_session->client_addr.sin_port), _session->id);

        //_session->all_thread_count += 1;
        pthread_create(&(_session->session_thread), NULL, start, _session);
    }

    for (struct _sic_session_data* iter = _session->begin; iter != NULL; iter = iter->next)
        pthread_join(iter->session_thread, NULL);
    return _session;
}

struct _sic_session_data* initSession(struct _sic_server_data* _server, struct _sic_session_data* _session){
    struct _sic_session_data* new_session;
    new_session = calloc(1, sizeof(struct _sic_session_data));
    if (_server->sess_items == 0){
        new_session->prev = NULL;
        new_session->next = NULL;
        new_session->begin = new_session;
        _session = new_session;
        _session->all_thread_count = 0;

        _server->sess_items += 1;
    }
    else if (_server->sess_items > 0 && _server->sess_items <= _server->conn_allowed){
        for (_session = _session->begin; _session->next != NULL; _session = _session->next);
        new_session->prev = _session;
        new_session->next = NULL;
        new_session->begin = _session->begin;
        _session->next = new_session;
        new_session->all_thread_count = _session->all_thread_count;
        _session = new_session;

        _server->sess_items += 1;
    }else{ 
        #ifdef _DEBUG_
        printf("List is full\n");
    #endif
    return NULL; }


    #ifdef _DEBUG_
    for (struct _sic_session_data* iter = _session->begin; iter != NULL; iter = iter->next){
        printf("Session %p | start %p | next %p | prev %p\n", iter, iter->begin, iter->next, iter->prev);
    }
    printf("--------------\n");
    #endif

    return _session;
}

void freeSessionList(struct _sic_server_data* _server, struct _sic_session_data* _session){
    struct _sic_session_data* temp;
    struct _sic_session_data* iter = _session;
    while(iter != NULL){
       if (iter->client_socket != 0){
            breakSession(_server, iter);
        }
        #ifdef _DEBUG_
            printf("Session free data %s\n", iter->id);
        #endif
        temp = iter->next;
        free(iter);
        iter = temp; 
    }
}

struct _sic_session_data* searchEmptySession(struct _sic_server_data* _server, struct _sic_session_data* _session){
    for (struct _sic_session_data* iter = _session->begin; iter != NULL; iter = iter->next){
        if (iter->client_socket == 0) 
        {
            return iter;
        }
    }

    #ifdef _DEBUG_
        printf("No free session\n");
    #endif
    return NULL;
}

struct _sic_session_data* establishSession(struct _sic_server_data* _server, struct _sic_session_data* _session){
    struct _sic_session_data* temp = _session;
    if ((_session = searchEmptySession(_server, _session)) == NULL) return NULL;

    if (_session->client_socket == 0 && _server->conn_established <= _server->conn_allowed && _server->sess_items <= _server->conn_allowed){
        unsigned int caddr_size = sizeof(_session->client_addr);
        _session->client_socket = accept(_server->server_socket, (struct sockaddr*)&(_session->client_addr), &caddr_size);
        if (_session->client_socket == INVALID_SOCKET) {
            #ifdef _DEBUG_
                printf("Failed to accept incoming connection\n");
            #endif
            return NULL;
        }
        generateSessionID(_session);
        #ifdef _DEBUG_
            printf("Session created with id: %s\n", _session->id);
        #endif
        _server->conn_established += 1;
    }else { return NULL; }

    return _session;
}

int breakSession(struct _sic_server_data* _server, struct _sic_session_data* _session){
    if (_session == NULL) return 1;
    if (_session->client_socket == 0) return 1;

    if (closeSessionSocketWin(_session) == 0){
        _server->conn_established--;
        return 0;
    }
    return 1;
} 

void generateSessionID(struct _sic_session_data* _session){
    // Concatenate the previous ID with the additional data
    char combinedInput[33]; // Make sure it's large enough to hold both strings
    if (_session->prev != NULL){
       snprintf(combinedInput, sizeof(combinedInput), "%s", _session->prev->id); 
    }else{
       snprintf(combinedInput, sizeof(combinedInput), "%s", "empty");  
    }

    // Generate an MD5 hash of the combined input to get the new ID
    char md5Output[MD5_DIGEST_LENGTH]; // 16 bytes for MD5 hash
    generateMD5(combinedInput, sizeof(combinedInput), md5Output);

    // Convert the binary MD5 hash to a human-readable string format for the new ID
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        snprintf(&(_session->id[i]), 3, "%02x", (unsigned int)md5Output[i]);
    }
}

int closeServerSocketWin(struct _sic_server_data* _server){
    #ifdef _DEBUG_
            printf("Closing server socket... ");
    #endif
    if (closesocket(_server->server_socket) == SOCKET_ERROR){
        #ifdef _DEBUG_
            printf("Failed to close server socket\n");
        #endif
        return 1;
    }
    #ifdef _DEBUG_
            printf("Closed\n");
    #endif
    return 0;
}

int closeSessionSocketWin(struct _sic_session_data* _session){
    #ifdef _DEBUG_
            printf("Closing session socket %s... ", _session->id);
    #endif
    if (closesocket(_session->client_socket) == SOCKET_ERROR){
        #ifdef _DEBUG_
            printf("Failed to close client socket\n");
        #endif
        return 1;
    }
    #ifdef _DEBUG_
            printf("Closed\n");
    #endif
    return 0;
}

int freeServer(struct _sic_server_data* _server){
    if (closeServerSocketWin(_server) == 0){
       free(_server); 
       return 0;
    }
    return 1;
}

void freeSession(struct _sic_session_data* _session){
    struct _sic_session_data* temp;
    if (_session->next == NULL && _session->prev == NULL){
        closeSessionSocketWin(_session);
        free(_session);
        #ifdef _DEBUG_
            printf("Single session deleted\n");
        #endif
    }else if (_session->next != NULL && _session->prev != NULL){
        struct _sic_session_data* temp = _session->next;
        _session->prev->next = temp;
        _session->next->prev = _session->prev;
        temp = _session->prev;
        #ifdef _DEBUG_
            printf("Middle session deleted\n");
        #endif
        closeSessionSocketWin(_session);
        free(_session);
    }else if (_session->next == NULL && _session->prev != NULL){
        _session->prev->next = NULL;
        temp = _session->prev;
        #ifdef _DEBUG_
            printf("First session deleted\n");
        #endif
        closeSessionSocketWin(_session);
        free(_session);
    }else if (_session->prev == NULL && _session->next != NULL){
        _session->next->prev = NULL;
        temp = _session->next;
        #ifdef _DEBUG_
            printf("Last session deleted\n");
        #endif
        closeSessionSocketWin(_session);
        free(_session);
    }
}

int sicDestroyWSA(){
    WSACleanup();
    return 0;
}

#endif
