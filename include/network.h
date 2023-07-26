#ifndef _S_INET
#define _S_INET

#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") // Link the Windows Sockets library

struct _s_session_data{

};

int createSocket(){
    WSDATA wsa;
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
        printf("Failed to initialize Winsock\n");
        return 1;
    }

    SOCKET server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_socket == INVALID_SOCKET){
        printf("Failed to create socket\n");
        return 1;
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}

#endif