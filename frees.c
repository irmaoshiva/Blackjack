#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "estruturas_e_prototipos.h"

///Função que retira da pilha todas as cartas recebidas do jogador e desaloca os blocos de memória que as continha
void clear_jogador(baralho **carta_jogador)
{
    baralho *aux = NULL;

    while ((*carta_jogador) != NULL)
    {
        aux = (*carta_jogador);
        (*carta_jogador) = (*carta_jogador)->next;
        free(aux);  //Desaloca a memória alocada por uma carta
    }
}
///Função que retira da pilha todas as cartas recebidas da mesa  e desaloca os blocos de memória que as continha
void clear_mesa (_mesa *mesa)
{
    baralho*aux = NULL;

    while ((*mesa).carta_mesa != NULL)
    {
        aux = (*mesa).carta_mesa;
        (*mesa).carta_mesa = (*mesa).carta_mesa->next;
        free(aux);  //Desaloca a memória alocada por uma carta
    }
}
///Função que desaloca a memória utilizada pela matriz que contém a informação para as jogadas das "EA"
void free_matriz(int ***ea_hard, int ***ea_soft)
{
    for (int i=0; i<10; i++)
    {
        free((*ea_hard)[i]);    //Desaloca a memória alocada por cada linha do ficheiro EA_hard
        if (i < 7)
            free((*ea_soft)[i]);    //Desaloca a memória alocada por cada linha do ficheiro EA_soft
    }
    free((*ea_hard));   //Desaloca a memória alocada para o vetor de ponteiros que apontam para as linhas
    free((*ea_soft));   //Desaloca a memória alocada para o vetor de ponteiros que apontam para as linhas
}
///Função que desaloca toda a memória alocada para jogadores cartas, e matriz das "EA" no final fo programa
void free_all(jogador *head_jogador, baralho *head_baralho, _mesa mesa,jogador *lista_final, int ***ea_hard, int ***ea_soft)
{
    jogador *aux_jogador=NULL;      //ponteiro que ajuda a percorrer os jogadores
    baralho *aux_baralho=NULL;      //ponteiro que ajuda a percorrer as cartas do baralho
    jogador *aux_lista=NULL;        //ponteiro que ajuda percorrer os jogadores da lista final

    while (head_baralho != NULL) //desaloca o bloco de memória que contém cada carta até acabar o baralho
    {
        aux_baralho=head_baralho;
        head_baralho = head_baralho->next;
        free(aux_baralho);
    }
    while (head_jogador != NULL)
    {
        aux_jogador = head_jogador;
        clear_jogador(&(head_jogador->player.carta_jogador));//desaloca as cartas do jogador
        head_jogador = head_jogador->next;      //desaloca o bloco de memória que contém o jogador
        free(aux_jogador);
    }
    clear_mesa(&mesa);
    while (lista_final != NULL) //igual ao primeiro "while" desta função
    {
        aux_lista = lista_final;
        clear_jogador(&(lista_final->player.carta_jogador));
        lista_final = lista_final->next;
        free(aux_lista);
    }
    free_matriz(&(*ea_hard), &(*ea_soft)); //desaloca a matriz
}
