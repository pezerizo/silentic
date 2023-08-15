#ifndef _S_INET_
#define _S_INET_

#define _DEBUG_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SESSSTACK 255
#define SESSPOLL SESSSTACK

typedef struct Server{
    ifdef __x86_64__
        uint64_t *wsa;
    #elif __i386__
        uint32_t *wsa;
    #else
        unsigned int *wsa;
    #endif

    PSILENTSESSION session_stack[SESSSTACK];
    WSAPOLLFD session_poll[SESSPOLL];

} SILENTSERVER, *PSILENTSERVER;


typedef struct Session{
    SOCKET session_socket;
    SOCKET client_socket;

    struct sockaddr_in session_addr;
    struct sockaddr_in client_addr;

    uint16_t port;
    uint16_t id;
    uint8_t *buffer;
} SILENTSESSION, *PSILENTSESSION;

//----------------------------------------------------------------------

extern PSILENTSESSION initServer();
extern PSILENTSESSION initSession(PSILENTSERVER server);

extern int initWSA(PSILENTSERVER server);
//extern void destroyWSA();

extern PSILENTSESSION establishSession(PSILENTSERVER server, PSILENTSESSION session);
extern void openSession();
extern void manageSession();
extern void closeSession();

//----------------------------------------------------------------------

#endif