#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024

void usage(int argc, char **argv) {
    printf("usage %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n");
    exit(EXIT_FAILURE);
}

void main(int argc, char **argv) {
    if(argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if((argv[1], argv[2], &storage) != 0) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    } 

    struct sockaddr *addr = (struct sockaddr *) (&storage);

    if(bind(s, addr, sizeof(storage)) != 0){
        logexit("bind");
    }
    if(listen(s, 10) != 0){ //quantidade de conexoes pendentes para tratamento
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("cound to %s, waiting xonnections \n", addrstr);

    while (1)
    {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *) (&cstorage);

        int csock = accept(s, caddr, sizeof(cstorage));

        if(csock == -1) {
            logexit("accept");
        }

        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        char buf[BUFSZ];
        memset(buf,0,BUFSZ);
        size_t count = recv(csock, buf, BUFSZ, 0);

        printf("[msg] %s, %d bytes: %s\n", caddrstr,(int)count, buf);

        sprintf(buf, "remote endpoint: %s\n", caddrstr);
        count = send(csock, buf, strlen(buf)+1, 0);
        if(count != strlen(buf)+1) {
            logexit("send");
        }
        close(csock);
    }

    exit(EXIT_SUCCESS);
}