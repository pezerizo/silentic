#ifndef _S_GATHER_
#define _S_GATHER_

#include <windows.h>
#include <stdint.h>

struct _s_time{
    int16_t year;
    int16_t month;
    int16_t day;
    int16_t hour;
    int16_t minute;
    int16_t second;
};


void gatherTime(){
    SYSTEMTIME st;

    GetLocalTime(&st);

    struct _s_time time = {
        st.wYear,
        st.wMonth,
        st.wDay,
        st.wHour,
        st.wMinute,
        st.wSecond
    };

    printf("%d", time.year);
}

#endif