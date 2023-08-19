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
        printf("WSA init error\n");
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

    newServer->server_addr.sin_family = AF_INET;
    newServer->server_addr.sin_addr.s_addr = INADDR_ANY;
    newServer->server_addr.sin_port = htons(port);

    if (bind(newServer->server_socket, (struct sockaddr*)&(newServer->server_addr), sizeof(struct sockaddr)) == SOCKET_ERROR) {
        printf("Binding failed\n");
        closesocket(newServer->server_socket);
        WSACleanup();
        free(newServer);
        return NULL;
    }

    if (listen(newServer->server_socket, MAXCONN) == SOCKET_ERROR) {
        printf("Listening failed\n");
        closesocket(newServer->server_socket);
        WSACleanup();
        free(newServer);
        return NULL;
    }

    printf(">> server started %s:%d\n", inet_ntoa(newServer->server_addr.sin_addr), ntohs(newServer->server_addr.sin_port));

    return newServer;
}

void startServer(PSILENTSERVER server){
    int struct_size = sizeof(struct sockaddr);
    size_t miss = 0;

    while (1) {
        for (size_t iter = 0; iter < MAXCONN; ++iter){
            if (server->client_socket_status[iter] == SOCKETFREE) {
                printf(">> socket id: %d\n", iter);
                server->client_sockets[iter] = accept(server->server_socket, (struct sockaddr*)&(server->client_addrs[iter]) , &struct_size);
                if (server->client_sockets[iter] == -1) {
                    printf(">> accepting error socket id: %d\n", iter);
                    continue;
                }
                if((server->session_buffers[iter] = (uint8_t*)calloc(1, SESSBUFFSIZE)) == server->session_buffers[iter]) {
                    printf(">> session buffer allocated with %d bytes\n", SESSBUFFSIZE);
                }else{
                    printf(">> session buffer allocation error\n");
                }
                printf(">> connection established with client %s:%d\n", inet_ntoa(server->client_addrs[iter].sin_addr), server->client_addrs[iter].sin_port);
                server->client_socket_status[iter] = SOCKETBUSY;
            }
        }
    }
}

/*

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
*/