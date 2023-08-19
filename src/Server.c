#include "..\include\Server.h"

PSILENTSERVER initServer(uint16_t port, const char* ip) {

    PSILENTSERVER newServer = (PSILENTSERVER)calloc(1, sizeof(SILENTSERVER));
    if (!newServer) {
        return NULL;
    }

    if (memset(newServer, 0x0, sizeof(SILENTSERVER)) != newServer) {
        free(newServer);
        return NULL;
    }

    if (WSAStartup(MAKEWORD(2, 2), &(newServer->wsa)) != 0) {
        #ifdef _DEBUG_
            printf("WSA init error\n");
        #endif
        free(newServer);
        return NULL;
    }

    newServer->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (newServer->server_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        free(newServer);
        return NULL;
    }

    newServer->event_obj = WSACreateEvent();
    if (newServer->event_obj == WSA_INVALID_EVENT) {
        printf("Event creation failed\n");
        closesocket(newServer->server_socket);
        WSACleanup();
        free(newServer);
        return NULL;
    }

    newServer->server_addr.sin_family = AF_INET;
    newServer->server_addr.sin_addr.s_addr = INADDR_ANY;
    newServer->server_addr.sin_port = htons(port);

    if (bind(newServer->server_socket, (struct sockaddr*)&(newServer->server_addr), sizeof(struct sockaddr)) == SOCKET_ERROR) {
        printf("Binding failed\n");
        WSACloseEvent(newServer->event_obj);
        closesocket(newServer->server_socket);
        WSACleanup();
        free(newServer);
        return NULL;
    }

    if (listen(newServer->server_socket, MAXCONN) == SOCKET_ERROR) {
        printf("Listening failed\n");
        WSACloseEvent(newServer->event_obj);
        closesocket(newServer->server_socket);
        WSACleanup();
        free(newServer);
        return NULL;
    }

    if (WSAEventSelect(newServer->server_socket, newServer->event_obj, FD_ACCEPT) == SOCKET_ERROR) {
        printf("WSAEventSelect failed\n");
        WSACloseEvent(newServer->event_obj);
        closesocket(newServer->server_socket);
        WSACleanup();
        free(newServer);
        return NULL;
    }

    return newServer;
}



void startServer(PSILENTSERVER server){
    WSANETWORKEVENTS netEvents;
    while (1)
    {
        DWORD result = WSAWaitForMultipleEvents(1, &(server->event_obj), FALSE, WSA_INFINITE, FALSE);

        if (result == WSA_WAIT_FAILED)
        {
            printf("error\n");
            break;
        }

        if (result == WSA_WAIT_EVENT_0)
        {
            // Event object is signaled, socket event has occurred
            if (WSAEnumNetworkEvents(server->server_socket, server->event_obj, &netEvents) == 0)
            {
                if (netEvents.lNetworkEvents & FD_ACCEPT)
                {
                    printf("Socket event: FD_ACCEPT\n");
                }
            }
        }
    }
}