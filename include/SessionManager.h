#ifndef _S_INET_
#define _S_INET_

#define _DEBUG_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define MAXSESSCOUNT 255
#define SESSPOLL MAXSESSCOUNT

typedef struct Server{
    ifdef __x86_64__
        uint64_t *wsa;
    #elif __i386__
        uint32_t *wsa;
    #else
        unsigned int *wsa;
    #endif
    
    #if MAXSESSCOUNT <= 256
        uint8_t session_in_container;
        uint8_t session_in_poll;
    #elif MAXSESSCOUNT > 256
        uint16_t session_in_container;
        uint16_t session_in_poll;
    #endif
    PSILENTSESSION session_container[MAXSESSCOUNT];
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
extern int initWSA(PSILENTSERVER server);

extern PSILENTSESSION initServer();
extern PSILENTSESSION initSession(PSILENTSERVER server, uint16_t port = 55550, unsigned long from_ip = INADDR_ANY);

extern PSILENTSESSION establishSession(PSILENTSERVER server, PSILENTSESSION session);
extern void openSession();
extern void manageSession();
extern void closeSession();

//----------------------------------------------------------------------

#endif