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
        if (arquivo_entrada == NULL) {
        perror("Erro ao abrir o arquivo");
    }

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

    if (mensagem->type==1)
    {
        if (board_inicial[linha][coluna] == -1) {
            return 0;
        }
        board_atual[linha][coluna] = board_inicial[linha][coluna];
    }
    else if(mensagem->type==2) {
        board_atual[linha][coluna] = -3;
    }
    else if(mensagem->type==4) {
        board_atual[linha][coluna] = -2;
    }
    return 1;
}

int verifica_vitoria() {
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (board_inicial[i][j] != -1 && board_atual[i][j] == -2)
            {
                return 0;
            }
        }
    }
    return 1;
}

void usage(int argc, char **argv) {
    printf("usage %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if(argc < 5) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if(server_sockaddr_init(argv[1], argv[2], &storage) != 0) {
        usage(argc, argv);
    }

    le_tabuleiro_inicial(argv[4]);

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s==-1){
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

    while (1)
    {
        struct action* mensagem = malloc(sizeof(struct action));

        int csock;
        struct sockaddr_storage cstorage;
        struct sockaddr* caddr = (struct sockaddr*)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        csock = accept(s, caddr,&caddrlen);
        if(csock==-1){
            logexit("accept");
        }
        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("client connected\n");

        while (1)
        {
            int resultado = 1;
            int vitoria = 0;

            size_t count = recv(csock, mensagem, sizeof(struct action), 0);

            if (mensagem->type==1 || mensagem->type==2 || mensagem->type==4)
            {
                resultado = atualiza_tabuleiro(mensagem);
                vitoria = verifica_vitoria();
            }

            gera_resposta(mensagem, board_atual, board_inicial, resultado, vitoria);

            count = send(csock, mensagem, sizeof(struct action), 0);
            if(count != sizeof(struct action)) {
                logexit("send");
            }
            if (mensagem->type==7)
            {
                break;
            }
        }
        close(csock);
    }
}