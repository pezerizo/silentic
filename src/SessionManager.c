#pragma once
#include "..\include\SessionManager.h"


PSILENTSERVER initServer(){
    PSILENTSERVER newServer = (PSILENTSERVER)calloc(1, sizeof(SILENTSERVER));
    if (memset(newServer, 0x0, sizeof(SILENTSERVER)) != newServer) return NULL;
    return newServer;
}

PSILENTSESSION initSession(){
    PSILENTSESSION newSession = (PSILENTSESSION)calloc(1, sizeof(SILENTSESSION));
    if (memset(newSession, 0x0, sizeof(SILENTSESSION)) != newSession) return NULL;
    
    return newSession;
}

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

extern PSILENTSESSION establishSession(PSILENTSERVER server, PSILENTSESSION session){

}