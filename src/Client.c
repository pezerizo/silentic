#include "../include/Client.h"

PSILENTCLIENT initClient(uint16_t server_port, const char* server_ip, size_t buffer_size){
    PSILENTCLIENT client = (PSILENTCLIENT)calloc(1, sizeof(SILENTCLIENT));
    memset(client, 0x0, sizeof(SILENTCLIENT));

    WSAStartup(MAKEWORD(2, 0), &(client->wsa));
    client->socket = socket(AF_INET, SOCK_STREAM, 0);
    client->remote_addr.sin_family = AF_INET;
    client->remote_addr.sin_addr.s_addr = inet_addr(server_ip);
    client->remote_addr.sin_port = htons(server_port);

    client->buffer = (char*)calloc(1, buffer_size);

    return client;
}

int connectClient(PSILENTCLIENT client){
    DWORD conn_stat;
    conn_stat = connect(client->socket, (struct sockaddr*)&(client->remote_addr), sizeof(struct sockaddr));
    printf("client connected...\n");
    printf("sending system time...\n");
    SYSTEMTIME time;
    GetSystemTime(&time);
    memcpy(client->buffer, &time, sizeof(SYSTEMTIME));
    if(send(client->socket, client->buffer, sizeof(SYSTEMTIME), 0) == sizeof(SYSTEMTIME)){
        printf("data has sended...\n");
    }

    shutdown(client->socket, SD_BOTH);
    closesocket(client->socket);
    WSACleanup();
    return 0;
}


int main(int argc, char** argv){
    uint16_t port = atoi(argv[2]);
    const char* ip = argv[1];
    size_t buffer_size = sizeof(SYSTEMTIME);

    PSILENTCLIENT client = initClient(port, ip, buffer_size);
    connectClient(client);

    free(client);
}