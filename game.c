#include "game.h"

void imprime_tabuleiro(char board[4][4]) {
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%c\t\t", board[i][j]);
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