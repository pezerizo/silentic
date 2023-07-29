#include <stdio.h>
#include <stdlib.h>
#include "..\include\session_manager.h"
#include "..\include\structs_manager.h"

int main(){
    struct _sic_server_data* server = calloc(1, sizeof(struct _sic_server_data));
    struct _sic_session_data* client = NULL;

    server->port = 55555;
    server->conn_allowed = 2;
    server->sess_items = 0;

    initSession(server, client);
    initSession(server, client);
    initSession(server, client);

    sicInitWSA(server);
    createSocketTCPWin(server);
    bindSocketWin(server);
    listenSocketWin(server);
    acceptSocketWin(server, client);


    freeSessionList(server, client);
    freeServerData(server);
    sicDestroyWSA();

    return 0;
}