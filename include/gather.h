#ifndef _S_GATHER_
#define _S_GATHER_

#include <windows.h>
#include <stdint.h>
#include "structs.h"

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