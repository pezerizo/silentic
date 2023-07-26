#ifndef _S_STRUCTS_
#define _S_STRUCTS_

#include <stdint.h>

struct _s_session_data{
    struct sockaddr_in server_addr;
    SOCKET server_socket;
    WSADATA wsa;
    int16_t bytes_received;
    char in_buffer[1024];
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