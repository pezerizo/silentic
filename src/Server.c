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

DWORD WINAPI sessionThreadFunction(LPVOID session) {
    printf("id of session: %d | buffer address %p\n", ((PSILENTSESSION)session)->id, ((PSILENTSESSION)session)->buffer);
    WSANETWORKEVENTS netEvents;
    DWORD result;
    int _id = ((PSILENTSESSION)session)->id;
    while (1)
    {
        result = WSAWaitForMultipleEvents(1, &(((PSILENTSESSION)session)->server->client_events[_id]), FALSE, WSA_INFINITE, FALSE);

        if (result == WSA_WAIT_FAILED)
        {
            printf("error\n");
            break;
        }

        if (result == WSA_WAIT_EVENT_0)
        {
            if (WSAEnumNetworkEvents(*(((PSILENTSESSION)session)->socket), ((PSILENTSESSION)session)->server->client_events[_id], &netEvents) == 0)
            {
                if (netEvents.lNetworkEvents & FD_READ)
                {
                    printf("Something went from %s:%d\n", inet_ntoa(((PSILENTSESSION)session)->addr->sin_addr), ((PSILENTSESSION)session)->addr->sin_port);
                }
            }
        }
    }
    return 0;
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

                PSILENTSESSION session = (PSILENTSESSION)calloc(1, sizeof(SILENTSESSION));
                session->server = server;
                session->socket = &(server->client_sockets[iter]);
                session->addr = &(server->client_addrs[iter]);
                session->buffer = server->session_buffers[iter];
                session->id = iter;


                server->client_events[iter] = WSACreateEvent();
                WSAEventSelect(server->client_sockets[iter], server->client_events[iter], FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT);

                server->session_threads[iter] = CreateThread(
                    NULL,               // Default security attributes
                    0,                  // Default stack size
                    sessionThreadFunction,     // Thread function
                    (LPVOID)session,               // Thread function argument
                    0,                  // Default creation flags
                    NULL                // Pointer to thread identifier (optional)
                );

                server->client_socket_status[iter] = SOCKETBUSY;
            }
        }
    }
}