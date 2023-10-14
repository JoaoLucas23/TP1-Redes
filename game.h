#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct action {
    int type;
    int coordinates[2];
    int board[4][4];
};

void imprime_tabuleiro(char board[4][4]);

void le_mensagem(char* acao, struct action* mensagem);