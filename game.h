#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct action {
    int type;
    int coordinates[2];
    int board[4][4];
};

void imprime_tabuleiro(int board[4][4]);

void le_mensagem(char* acao, struct action* mensagem);

void le_resposta(struct action* mensagem, int tabuleiro_atual[4][4]);

void gera_resposta(struct action* mensagem, int tabuleiro_atual[4][4], int tabuleiro_inicial[4][4], int resultado);