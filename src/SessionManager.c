#ifndef _S_INET_C_
#define _S_INET_C_

#include "..\include\SessionManager.h"



int initWSA(PSILENTSERVER server){
    if (WSAStartup(MAKEWORD(2, 2), server->wsa) != 0){
        #ifdef _DEBUG_
            printf("\tWSA init errorr\n");
        #endif
        WSACleanup();
        return 1;
    }
    return 0;
}


PSILENTSERVER initServer(){
    PSILENTSERVER newServer = (PSILENTSERVER)calloc(1, sizeof(SILENTSERVER));
    if (memset(newServer, 0x0, sizeof(SILENTSERVER)) != newServer) return NULL;
    return newServer;
}


PSILENTSESSION initSession(PSILENTSERVER server, uint16_t port, uint16_t buffer_size){
    PSILENTSESSION newSession = (PSILENTSESSION)calloc(1, sizeof(SILENTSESSION));
    if (memset(newSession, 0x0, sizeof(SILENTSESSION)) != newSession) return NULL;

    size_t miss = 0
    for(size_t iter = 0; iter < MAXSESSCOUNT; ++iter){
        if(server->session_container[iter] == NULL){

            newSession->session_addr.sin_family = AF_INET;
            newSession->session_addr.sin_addr.s_addr = INADDR_ANY;
            newSession->session_addr.sin_port = htons(port);
            newSession->id = iter;
            newSession->port = port;
            newSession->buffer = (uint8_t*)calloc(1, buffer_size);
            newSession->session_socket = socket(AF_INET, SOCK_STREAM, 0);

            bind(newSession->session_socket, (struct sockaddr*)&(newSession->session_addr), sizeof(struct sockaddr));

            server->session_container[iter] = newSession;
            server->session_in_container++;

            server->session_poll[iter].fd = newSession->session_socket;
            server->session_poll[iter].events = POLLIN | POLLOUT;
            WSAPoll(server->session_poll, SESSPOLL, -1);

            server->session_in_poll++;

            listen(newSession->session_socket, 1);
            break;
        }else{
            if(++miss == MAXSESSCOUNT) return NULL;
        }
    }

    return NULL;
}

void startServer(PSILENTSERVER server){
    while(1){
        for(size_t iter = 0; iter < MAXSESSCOUNT; ++iter){
            if((server->session_container[iter]->session_socket != 0 &&
                server->session_poll[iter].fd != NULL &&
                server->session_container[iter]->client_addr == 0) && (server->session_poll[iter].revents & POLLIN)){
                    server->session_container[iter]->client_socket = accept(server->session_container[iter]->session_socket, (struct sockaddr *)&(server->session_container[iter]->client_addr), sizeof(struct sockaddr));
                    printf("new session created with %s", inet_ntoa(server->session_container[iter]->client_addr.sin_addr))
                }
            else if((server->session_container[iter]->session_socket != 0 &&
                server->session_poll[iter].fd != NULL &&
                server->session_container[iter]->client_addr == 0) && (server->session_poll[iter].revents & POLLIN)){
                    printf("something to read from %s", inet_ntoa(server->session_container[iter]->client_addr.sin_addr))
                }
        }
    }
}

#endif