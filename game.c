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