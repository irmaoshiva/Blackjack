#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "estruturas_e_prototipos.h"
///Função que recebe um ponteiro para um jogador como entrada e inicializa os parâmetros do jogador, uns sempre, outros em função de uma flag
void ini_jogador(jogador *jogador, int flag)
{
    jogador->player.pontuacao=0;
    jogador->player.bj=0;
    jogador->player.ases=0;
    jogador->player.bust=0;
    jogador->player.num_cartas=0;
    jogador->player.desistiu =0;

    if (flag == 0)  //para o início de cada jogada
        clear_jogador(&(jogador->player.carta_jogador));
    else //para o início de um jogo
    {
        jogador->player.carta_jogador = NULL;
        jogador->player.empates=0;
        jogador->player.vitorias=0;
        jogador->player.derrotas=0;
    }
}
///função que tira uma carta aleatória do baralho, retornando-a, e fazendo free do bloco de memória do baralho que continha essa carta
s_carta tirar_carta_aleatoria(baralho **head_baralho, int *num_total, int num_baralhos, int *hi_lo)
{
    baralho *aux=NULL;
    baralho *aux2=NULL;
    s_carta carta_tirada; // carta que vai ser tirada do baralho e retornada
    int carta_aleatoria; //número da carta da lista a ser retirada

    srand(time(NULL));
    carta_aleatoria = (rand()%(*num_total));  //gerar um valor aleatório de 0 ao número de cartas que ainda há no baralho

    aux = (*head_baralho);

    if(carta_aleatoria==0) //para o caso da carta a ser retirada ser a primeira do baralho (2 de paus)
    {
        (*head_baralho)=aux->next;
        carta_tirada=aux->carta;
    }
    else //para todos os outros casos
    {
        for(int i=0; i<(carta_aleatoria-1); i++) //desloca o aux até à carta anterior àquela que vai ser retirada
        {
            aux=aux->next;
        }
        aux2 = aux->next;
        aux->next = aux2->next;
        carta_tirada = aux2->carta;
        free(aux2);
    }
    (*num_total)--; //decrementar o número total de cartas, pois saiu uma carta do baralho

    if (calcular_pontos(carta_tirada.valor) > 9)
        (*hi_lo)--;                               //contar cartas para a estratégia hi lo
    if(calcular_pontos(carta_tirada.valor) < 7)   //do "EA" sempre que é dada uma carta
        (*hi_lo)++;

    if((*head_baralho) == NULL) //sempre que se acabar o baralho, contruir um baralho de novo
    {
        criar_baralho(num_baralhos, &(*head_baralho));
        (*num_total) = 52*num_baralhos;
        (*hi_lo) = 0;
    }
    return carta_tirada;
}
///recebe como entrada o valor da carta (de 1 a 13), calcula a pontuação dessa carta e retorna o valor da pontuação
int calcular_pontos(int numero)
{
    int pontuacao;
    switch(numero)
    {
    case 10:
        pontuacao = 10;
        break;
    case 11:
        pontuacao = 10;
        break;
    case 12:
        pontuacao = 10;
        break;
    case 13:
        pontuacao = 11;
        break;
    default:
        pontuacao = numero+1;
        break;
    }
    return pontuacao;
}
