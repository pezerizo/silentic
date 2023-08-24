#include "..\include\Server.h"

PSILENTSERVER initServer(uint16_t port, const char* ip) {

    PSILENTSERVER new_server = (PSILENTSERVER)calloc(1, sizeof(SILENTSERVER));
    if (!new_server) {
        return NULL;
    }

    if (memset(new_server, 0x0, sizeof(SILENTSERVER)) != new_server) {
        free(new_server);
        return NULL;
    }

    if (WSAStartup(MAKEWORD(2, 2), &(new_server->wsa)) != 0) {
        printf("WSA init error\n");
        free(new_server);
        return NULL;
    }

    new_server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (new_server->server_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        free(new_server);
        return NULL;
    }

    new_server->server_addr.sin_family = AF_INET;
    new_server->server_addr.sin_addr.s_addr = INADDR_ANY;
    new_server->server_addr.sin_port = htons(port);

    if (bind(new_server->server_socket, (struct sockaddr*)&(new_server->server_addr), sizeof(struct sockaddr)) == SOCKET_ERROR) {
        printf("Binding failed\n");
        closesocket(new_server->server_socket);
        WSACleanup();
        free(new_server);
        return NULL;
    }

    if (listen(new_server->server_socket, MAXCONN) == SOCKET_ERROR) {
        printf("Listening failed\n");
        closesocket(new_server->server_socket);
        WSACleanup();
        free(new_server);
        return NULL;
    }

    printf(">> server started %s:%d\n", inet_ntoa(new_server->server_addr.sin_addr), ntohs(new_server->server_addr.sin_port));

    return new_server;
}

DWORD WINAPI sessionThreadFunction(LPVOID _session) {

    WSANETWORKEVENTS net_events;
    DWORD result;
    PSILENTSESSION session = (PSILENTSESSION)_session;
    int _id = session->id;
    while (1) {
        result = WSAWaitForMultipleEvents(1, &(session->server->client_events[_id]), FALSE, WSA_INFINITE, FALSE);

        if (result == WSA_WAIT_FAILED) {
            printf("error\n");
            break;
        }

        if (result == WSA_WAIT_EVENT_0) {

            if (WSAEnumNetworkEvents(*(session->socket), session->server->client_events[_id], &net_events) == 0) {

                if (net_events.lNetworkEvents & FD_READ) {
                    
                    int bytes_received = recv(*(session->socket), session->buffer, SESSBUFFSIZE-1, 0);
                    session->buffer[bytes_received] = '\0';
                    if (bytes_received == SOCKET_ERROR) {
                        printf("[error | %s:%d]>> Error receiving data\n", inet_ntoa(session->addr->sin_addr), session->addr->sin_port);
                    } else if (bytes_received == 0) {
                        printf("[error | %s:%d]>> Connection closed by the remote side\n", inet_ntoa(session->addr->sin_addr), session->addr->sin_port);
                    } else {
                        SYSTEMTIME time;
                        memcpy(&time, session->buffer, sizeof(SYSTEMTIME));
                        printf("\n[debug | %s:%d]>> bytes: %d message: %s\n", inet_ntoa(session->addr->sin_addr), session->addr->sin_port, bytes_received, session->buffer);
                        printf("Current Date and Time:\n");
                        printf("Year: %d\n", time.wYear);
                        printf("Month: %02d\n", time.wMonth);
                        printf("Day: %02d\n", time.wDay);
                        printf("Hour: %02d\n", time.wHour);
                        printf("Minute: %02d\n", time.wMinute);
                        printf("Second: %02d\n", time.wSecond);
                        printf("Milliseconds: %03d\n", time.wMilliseconds);
                    }

                } else if (net_events.lNetworkEvents & FD_CLOSE) {

                    printf("[debug | %s:%d]>> connection closed by client\n", inet_ntoa(session->addr->sin_addr), session->addr->sin_port);
                    freeSession(session);
                    return 0;
                }
            }
        }
    }
    return 0;
}


void startServer(PSILENTSERVER server){
    int struct_size = sizeof(struct sockaddr);
    size_t miss = 0;

    do {

        for (size_t iter = 0; iter < MAXCONN; ++iter) {

            if (server->client_socket_status[iter] == SOCKETFREE) {

                server->client_sockets[iter] = accept(server->server_socket, (struct sockaddr*)&(server->client_addrs[iter]) , &struct_size);

                if (server->client_sockets[iter] == -1) {
                    printf("[error]>> accepting error socket id: %d\n", iter);
                    continue;
                }
                if ((server->session_buffers[iter] = (uint8_t*)calloc(1, SESSBUFFSIZE)) == server->session_buffers[iter]) {
                    printf("[debug]>> buffer for session allocated with %d bytes\n", SESSBUFFSIZE);
                } else {
                    printf("[error]>> session buffer allocation error\n");
                }

                printf("[debug]>> connection established with client %s:%d\n", inet_ntoa(server->client_addrs[iter].sin_addr), server->client_addrs[iter].sin_port);

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
                server->session_in_job++;
            }
        }
    } while (server->session_in_job != 0);
}


void freeSession(PSILENTSESSION session){
    const char* ip = inet_ntoa(session->addr->sin_addr);
    uint16_t port = session->addr->sin_port;
    if (shutdown(*(session->socket), SD_BOTH) == SOCKET_ERROR) {
        printf("[error | %s:%d]>> Error shutting down socket\n", ip, port);
    }
    if (closesocket(*(session->socket)) == SOCKET_ERROR) {
        printf("[error | %s:%d]>> Error closing socket\n", ip, port);
    }
    free(session->buffer);
    memset(session->addr, 0x0, sizeof(struct sockaddr_in));
    if (!ResetEvent(session->server->client_events[session->id])) {
        printf("[error | %s:%d]>> Error resetting event object\n", ip, port);
    }
    if (WSACloseEvent(session->server->client_events[session->id]) == FALSE) {
        printf("[error | %s:%d]>> Error closing event object\n", ip, port);
    }
    session->server->client_socket_status[session->id] = 0;
    (session->server->session_in_job)--;
    printf("[debug | %s:%d]>> is free now\n", ip, port);
}