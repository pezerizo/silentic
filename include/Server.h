#ifndef _S_SERVER_
#define _S_SERVER_

#include <stdint.h>
#include <winsock2.h>
#include <windows.h>

#include "SessionModule.h"
#include "DBModule.h"
#include "UIModule.h"
#include "CommandModule.h"
#include "ParserModule.h"

#define MAXCONN 1

typedef struct Server{

    WSADATA wsa;
    WSAEVENT event_obj;
    SOCKET server_socket;
    SOCKET client_socket;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

} SILENTSERVER, *PSILENTSERVER;

PSILENTSERVER initServer(uint16_t port, const char* ip);
void startServer(PSILENTSERVER server);

#endif