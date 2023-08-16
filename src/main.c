#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "SessionManager.c"

int main(){
    PSILENTSERVER silentic = initServer();
    initWSA(silentic);
    initSession(silentic, 55550, 8192);


    return 0;
}