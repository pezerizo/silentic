#ifndef _S_STRUCTS_
#define _S_STRUCTS_

#include <stdint.h>
#include <pthread.h>

#define MAXBUFFSIZE 1024
#define SESSIDSIZE 16

struct _sic_server_data{
    struct sockaddr_in server_addr;
    SOCKET server_socket;
    WSADATA wsa;
    int16_t port;
    int16_t conn_allowed;
    int16_t conn_established;
    int16_t sess_items;
};

struct _sic_session_data{
    struct sockaddr_in client_addr;
    SOCKET client_socket;
    int16_t bytes_received;
    char in_buffer[MAXBUFFSIZE];
    
    char id[SESSIDSIZE+1];

    pthread_t session_thread;
    int16_t all_thread_count;

    struct _sic_session_data* prev;
    struct _sic_session_data* next;
    struct _sic_session_data* begin;
};

#endif