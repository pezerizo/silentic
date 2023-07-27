#include <stdio.h>
#include <stdlib.h>
#include "..\include\session_manager.h"
#include "..\include\structs.h"

int main(){
    if (signal(SIGINT, handleCtrlC) == SIG_ERR) {
        perror("Error registering signal handler");
        return EXIT_FAILURE;
    }

    struct _sic_server_data* server = calloc(1, sizeof(struct _sic_server_data));
    struct _sic_session_data* client = NULL;

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