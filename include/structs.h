#ifndef _S_STRUCTS_
#define _S_STRUCTS_

#include <stdint.h>

#define MAXBUFFSIZE 1024
#define SESSIDSIZE 16

struct _sic_server_data{
    struct sockaddr_in server_addr;
    SOCKET server_socket;
    WSADATA wsa;
    char* address;
    int16_t port;
    int16_t conn_count;
    };

struct _sic_session_data{
    struct sockaddr_in client_addr;
    SOCKET client_socket;
    int16_t bytes_received;
    char in_buffer[MAXBUFFSIZE];
    char id[SESSIDSIZE+1];

    struct _sic_session_data* prev;
    struct _sic_session_data* next;
    struct _sic_session_data* start;
};

struct _s_time{
    int16_t year;
    int16_t month;
    int16_t day;
    int16_t hour;
    int16_t minute;
    int16_t second;
    };




#endif