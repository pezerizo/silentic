#pragma once
#include "..\include\SessionManager.h"


int initWSA(PSILENTSERVER server){
    if (WSAStartup(MAKEWORD(2, 2), &(server->wsa)) != 0){
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


PSILENTSESSION initSession(PSILENTSERVER server, uint16_t port = 55550, unsigned long from_ip = INADDR_ANY){
    PSILENTSESSION newSession = (PSILENTSESSION)calloc(1, sizeof(SILENTSESSION));
    if (memset(newSession, 0x0, sizeof(SILENTSESSION)) != newSession) return NULL;

    for(size_t iter = 0, size_t miss = 0; iter < MAXSESSCOUNT; ++iter){
        if(server->session_container[iter] == NULL){

            newSession->session_addr.sin_family = AF_INET;
            newSession->session_addr.sin_addr.s_addr = from_ip;
            newSession->session_addr.sin_port = port;
            newSession->id = iter;
            newSession->session_socket = socket(AF_INET, SOCK_STREAM, 0);
            bind(newSession->session_socket, (struct sockaddr*)&(newSession->session_addr), sizeof(struct sockaddr));
            listen(newSession->session_socket, 2);


            server->session_container[iter] = newSession;
            server->session_in_container++;

            server->session_poll[iter].fd = newSession->session_socket;
            server->session_poll[iter].events = POLLIN | POLLOUT;
            WSAPoll(server->session_poll, SESSPOLL, -1);

            server->session_in_poll++;


            break;
        }else{
            if(++miss == MAXSESSCOUNT) return NULL;
        }
    }

    return NULL;
}