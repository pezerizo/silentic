#ifndef _S_SERVER_
#define _S_SERVER_

#include <stdint.h>
#include <winsock2.h>
#include <windows.h>

#define MAXCONN 1
#define SESSBUFFSIZE 4096

#define SOCKETFREE 0
#define SOCKETBUSY 1
#define SOCKETPEND 2

typedef struct Server{

    WSADATA wsa;

    SOCKET server_socket;
    struct sockaddr_in server_addr;

    WSAEVENT client_events;
    SOCKET client_sockets[MAXCONN];
    struct sockaddr_in client_addrs[MAXCONN];
    uint8_t client_socket_status[MAXCONN];
    uint8_t *session_buffers[MAXCONN];

} SILENTSERVER, *PSILENTSERVER;

PSILENTSERVER initServer(uint16_t port, const char* ip);
void startServer(PSILENTSERVER server);

#endif