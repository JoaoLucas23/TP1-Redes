#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024

int board_inicial[4][4];

void le_tabuleiro_inicial(char* arquivo) {
    FILE *arquivo_entrada = fopen(arquivo, "r");

    // Lê os valores do arquivo e armazena na matriz board_inicial
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
                fscanf(arquivo_entrada,"%d,", &board_inicial[i][j]);
                printf("%d\t", board_inicial[i][j]);
        }
        printf("\n");
    }
    fclose(arquivo_entrada);
}

void usage(int argc, char **argv) {
    printf("usage %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if(argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if(server_sockaddr_init(argv[1], argv[2], &storage) != 0) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    } 

    int enable=1;
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0) {
        logexit("setsockopt");
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
    printf("bound to %s, waiting connections \n", addrstr);

    le_tabuleiro_inicial(argv[4]);

    while (1)
    {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *) (&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);

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

        sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
        count = send(csock, buf, strlen(buf)+1, 0);
        if(count != strlen(buf)+1) {
            logexit("send");
        }
        close(csock);
    }

    exit(EXIT_SUCCESS);
}  