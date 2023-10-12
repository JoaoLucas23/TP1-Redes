#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024

void usage(int argc, char **argv) {
    printf("usage %s <server IP> <server port>");
    printf("example: %s 127.0.0.1 51511");
    exit(EXIT_FAILURE);
}

void logexit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void main(int argc, char **argv) {
    if(argc < 3) {
        usage(argc, argv);
    }

    int s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    } 

    struct sockaddr_storage storage;
    if(addrparse(argv[1], argv[2], &storage) != 0) {
        usage(argc, argv);
    }
    struct sockaddr *addr = (struct sockaddr *) (&storage);

    if (connect(s, addr, sizeof(storage)) != 0) {
        logexit("connect");
    }

    char addrstr[BUFSZ];

    addrtostr(addr, addrstr, BUFSZ);
    printf("connected to %s \n");

    char buf[BUFSZ];
    memset(buf, 0, BUFSZ);
    printf("mensagem> ");
    fgets(buf, BUFSZ-1, stdin);
    int count = send(s, buf, strlen(buf)+1, 0);

    if(count != strlen(buf)+1) {
        logexit("send");
    }

    memset(buf, 0, BUFSZ);
    unsigned total = 0;
    while (1)
    {
        count = recv(s, buf + total, BUFSZ - total, 0);
        if(count == 0) {
            // conexao fechada
            break;
        }
        total += count;
    }
    
    close(s);

    printf("received %u bytes\n");
    puts(buf);

    exit(EXIT_SUCCESS);
}