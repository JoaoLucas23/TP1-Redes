#include "common.h"
#include "game.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024

int board_inicial[4][4];

int board_atual[4][4];

void le_tabuleiro_inicial(char* arquivo) {
    FILE *arquivo_entrada = fopen(arquivo, "r");

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // Lê os valores do arquivo e armazena na matriz board_inicial
            fscanf(arquivo_entrada,"%d,", &board_inicial[i][j]);
            printf("%d\t\t", board_inicial[i][j]);
            // incializa o board atual com o codigo da string '-'
            board_atual[i][j] = -2;
        }
        printf("\n");
    }
    fclose(arquivo_entrada);
}

int atualiza_tabuleiro(struct action* mensagem) {
    int linha = mensagem->coordinates[0];
    int coluna = mensagem->coordinates[1];

    if(linha >= 4 || coluna >= 4){
        printf("o error: invalid cell");
    }
    if (mensagem->type==1)
    {
        if(board_atual[linha][coluna] == board_inicial[linha][coluna]) {
            printf("error: invalid cell");
        } else if (board_inicial[linha][coluna] == -1) {
            return 0;
        }
        board_atual[linha][coluna] = board_inicial[linha][coluna];
    }
    else if(mensagem->type==2) {
        if(board_atual[linha][coluna] == board_inicial[linha][coluna]) {
            printf("error: cell already has a flag");
        } else if(board_atual[linha][coluna] >= 0) {
            printf("error: cannot insert flag in revealed cell");
        } else {
            board_atual[linha][coluna] = -3; 
        }
    }
    else if(mensagem->type==4) {
        board_atual[linha][coluna] = -2;
    } 
    return 1;  
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
    if(listen(s, 100) != 0){ //quantidade de conexoes pendentes para tratamento
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections \n", addrstr);

    le_tabuleiro_inicial(argv[4]);

    int resultado = 1;

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
        printf("client connected\n");
        
        struct action* mensagem = malloc(sizeof(struct action));
 
        size_t count = recv(csock, mensagem, sizeof(struct action)+1, 0);

        if (mensagem->type==1 || mensagem->type==2 || mensagem->type==4)
        {
            resultado = atualiza_tabuleiro(mensagem);
        }
        gera_resposta(mensagem, board_atual, board_inicial, resultado);
        
        count = send(csock, mensagem, sizeof(struct action)+1, 0);
        if(count != sizeof(struct action)+1) {
            logexit("send");
        }
        close(csock);
    }

    exit(EXIT_SUCCESS);
}  