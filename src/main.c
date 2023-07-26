#include <stdio.h>
#include <stdlib.h>
#include "..\include\gather.h"
#include "..\include\networkwin.h"
#include "..\include\structs.h"

int main(){
    struct _sic_server_data* server = calloc(1, sizeof(struct _sic_server_data));
    struct _sic_session_data* client = calloc(1, sizeof(struct _sic_session_data));

    server->port = 55555;
    server->conn_count = 1;

    sicInitWSA(server);
    createSocketTCPWin(server);
    bindSocketWin(server);
    listenSocketWin(server);
    acceptSocketWin(server, client);

    freeServerData(server);
    freeSessionsData(client);
    sicDestroyWSA();

    return 0;
}