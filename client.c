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

int verifica_fim(struct action* mensagem) {
    switch (mensagem->type)
    {
    case 8:
        printf("GAME OVER!\n");
        return 1;
    case 6:
        printf("YOU WIN!\n");
        return 1;
    default:
        return 0;
    }
}

int verifica_erro(struct action* mensagem) {
    int linha = mensagem->coordinates[0];
    int coluna = mensagem->coordinates[1];
    if (mensagem->type == -1)
    {
        printf("error: command not found\n");
        return 1;
    }
    else if(linha >= 4 || coluna >= 4){
        printf("error: invalid cell\n");
        return 1;
    }
    else if (mensagem->board[linha][coluna] >= 0 && mensagem->type==1)
    {
        printf("error: cell already revealed\n");
        return 1;
    }
    else if (mensagem->board[linha][coluna] >= 0 && mensagem->type==2)
    {
        printf("error: cannot insert flag in revealed cell\n");
        return 1;
    }
        else if (mensagem->board[linha][coluna] == -3 && mensagem->type==2)
    {
        printf("error: cell already has a flag\n");
        return 1;
    }
    return 0;
}

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

    struct action* mensagem = malloc(sizeof(struct action));
    int fim = 0;
    int erro = 0;

    while (1)
    {   
        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        fgets(buf, BUFSZ-1, stdin);

        le_mensagem(buf, mensagem);

        erro = verifica_erro(mensagem);

        size_t count = send(s, mensagem, sizeof(struct action)+1 , 0);
        if(count != sizeof(struct action)+1) {
            logexit("send");
        }

        count = recv(s, mensagem, sizeof(struct action)+1, 0);
        if(count != sizeof(struct action)+1){
            logexit("receive");
        }
        if (mensagem->type == 7)
        {
            break;
        }

        if(erro == 0){
            fim = verifica_fim(mensagem);
            imprime_tabuleiro(mensagem->board);
            if (fim)
            {
                break;
            } 
        }
    }
    
    close(s);
    exit(EXIT_SUCCESS);
}