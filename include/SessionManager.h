#ifndef _S_INET_
#define _S_INET_

#define _DEBUG_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

typedef struct Server{
    ifdef __x86_64__
        uint64_t *globalWsa;
    #elif __i386__
        uint32_t *globalWsa;
    #else
        unsigned int *globalWsa;
    #endif

} SILENTSERVER, *PSILENTSERVER


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
extern PSILENTSESSION initSession();

extern int initWSA();
extern void destroyWSA();

extern void createSession();
extern void openSession();
extern void manageSession();
extern void closeSession();
extern void destroySession();

//----------------------------------------------------------------------

#endif