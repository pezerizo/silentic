#ifndef _S_SERVER_
#define _S_SERVER_

#include <stdint.h>
#include <winsock2.h>
#include <windows.h>

#define MAXCONN 4
#define SESSBUFFSIZE 4096

#define SOCKETFREE 0
#define SOCKETBUSY 1
#define SOCKETPEND 2

typedef struct Server{

    WSADATA wsa;

    SOCKET server_socket;
    struct sockaddr_in server_addr;

    WSAEVENT client_events[MAXCONN];
    HANDLE session_threads[MAXCONN];
    SOCKET client_sockets[MAXCONN];
    struct sockaddr_in client_addrs[MAXCONN];
    uint8_t client_socket_status[MAXCONN];
    uint8_t *session_buffers[MAXCONN];

} SILENTSERVER, *PSILENTSERVER;

typedef struct SessionID{

    PSILENTSERVER server;
    SOCKET *socket;
    struct sockaddr_in *addr;
    int id;
    uint8_t *buffer;

} SILENTSESSION, *PSILENTSESSION;


PSILENTSERVER initServer(uint16_t port, const char* ip);
void startServer(PSILENTSERVER server);

#endif