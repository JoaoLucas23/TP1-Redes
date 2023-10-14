#include "game.h"

void copia_tabuleiro(int src[4][4], int dst[4][4]) {
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            dst[i][j] = src[i][j];
        }   
    }   
}

char traduz_caracter(int cod) {
    switch (cod)
    {
    case -1:
        return '*';
    case -2:
        return '-';
    case -3:
        return '>';
    case 0:
        return '0';
    default:
        return (char)(cod+'0');
    }
}

void imprime_tabuleiro(int board[4][4]) {
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%c\t\t", traduz_caracter(board[i][j]));
        }
        printf("\n"); 
    } 
}

int traduz_acao(char* acao) {
    char* acoes_possiveis[9] = {
        "start",
        "reveal",
        "flag",
        "state",
        "remove_flag",
        "reset",
        "win",
        "exit",
        "game_over"
    };
    for (int i = 0; i < 9; i++)
    {
        if(strcmp(acao, acoes_possiveis[i])==0){
            return i;
        }
    }
    return -1;
}

void le_mensagem(char* acao, struct action* mensagem) {
    acao = strtok(acao, " \n");
    mensagem->type = traduz_acao(acao);
    if (mensagem->type == 1 || mensagem->type == 2 || mensagem->type == 4)
    {
        mensagem->coordinates[0] = atoi(strtok(NULL,","));
        mensagem->coordinates[1] = atoi(strtok(NULL,","));
    }
}

void gera_resposta(struct action* mensagem, int tabuleiro_atual[4][4], int tabuleiro_inicial[4][4], int resultado, int vitoria) {
    if (!resultado)
    {
        copia_tabuleiro(tabuleiro_inicial,mensagem->board);
        mensagem->type = 8;
        return;
    }
    else if (vitoria)
    {
        copia_tabuleiro(tabuleiro_inicial,mensagem->board);
        mensagem->type = 6;
        return;
    }
    else if (mensagem->type == 0 || mensagem->type == 1 || mensagem->type == 2 || mensagem->type == 4) {
        copia_tabuleiro(tabuleiro_atual, mensagem->board);
        mensagem->type = 3;
        return;
    }
    else if (mensagem->type == 5){
        // reseta o tabuleiro
        printf("starting new game");
        return;
    }
    else if (mensagem-> type == 7) {
        // encerra conexao
        return;
    }
    return;
}
