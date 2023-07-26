#ifndef _S_STRUCTS_
#define _S_STRUCTS_

#include <stdint.h>

#define MAXBUFFSIZE 1024

struct _sic_server_data{
    struct sockaddr_in server_addr;
    SOCKET server_socket;
    WSADATA wsa;
    };

struct _sic_session_data{
    struct sockaddr_in client_addr;
    SOCKET client_socket;
    int16_t bytes_received;
    char in_buffer[MAXBUFFSIZE0];
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