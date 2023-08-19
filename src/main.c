#include <stdio.h>
#include "Server.c"

int main(){
    PSILENTSERVER server = initServer(65354, "127.0.0.1");
    startServer(server);
    return 0;
}