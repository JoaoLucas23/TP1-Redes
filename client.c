#include "common.h"
#include "game.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSZ 1024

void usage(int argc, char **argv) {
    printf("usage %s <server IP> <server port>\n", argv[0]);
    printf("example: %s 127.0.0.1 51511", argv[0]);
    exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
    if(argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if(addrparse(argv[1], argv[2], &storage) != 0) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    } 

    struct sockaddr *addr = (struct sockaddr *) (&storage);

    if (connect(s, addr, sizeof(storage)) != 0) {
        logexit("connect");
    }

    char addrstr[BUFSZ];

    addrtostr(addr, addrstr, BUFSZ);
    printf("connected to %s \n", addrstr);

    // memset(buf, 0, BUFSZ);
    // printf("mensagem> ");
    // fgets(buf, BUFSZ-1, stdin);
    //   send(s, buf, strlen(buf)+1, 0);

    while (1)
    {
        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        printf("mensagem> ");
        fgets(buf, BUFSZ-1, stdin);

        struct action* mensagem = malloc(sizeof(struct action));
        le_mensagem(buf, mensagem);

        size_t count = send(s, mensagem, sizeof(struct action)+1 , 0);
        if(count != sizeof(struct action)+1) {
            logexit("send");
        }

        count = recv(s, mensagem, sizeof(struct action)+1, 0);
        printf("count recv: %lu\n",count);
        imprime_tabuleiro(mensagem->board);
    }
    
    close(s);
    exit(EXIT_SUCCESS);
}