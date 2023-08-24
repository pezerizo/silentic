#ifndef _S_SERVER_
#define _S_SERVER_

#include <stdint.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

typedef struct Client{

    struct sockaddr_in remote_addr;
    SOCKET socket;
    WSADATA wsa;
    char *buffer;

} SILENTCLIENT, *PSILENTCLIENT;


PSILENTCLIENT initClient(uint16_t server_port, const char* server_ip, size_t buffer_size);
int connectClient(PSILENTCLIENT client);

#endif