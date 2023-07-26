#include <stdio.h>
#include "..\include\gather.h"
#include "..\include\networkwin.h"
#include "..\include\structs.h"

int main(){
    //gatherTime();
    struct _s_session_data* session = createSocket();
    closeSocket(session);
    return 0;
}