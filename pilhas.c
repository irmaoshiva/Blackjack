#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "estruturas_e_prototipos.h"

///Função que recebe com entrada a data de uma carta e um ponteiro para uma pilha de cartas, e coloca lá a nova carta sempre no topo
void push_jogador(baralho **carta_jogador, s_carta carta_tirada, jogador *jogador_actual)
{
    baralho* aux = NULL; //auxiliar para colocar a nova carta no topo da pilha
    if ((*carta_jogador)==NULL) //para o caso de o jogador ainda não ter nenhuma carta
    {
        (*carta_jogador) = (baralho*) calloc(1, sizeof(baralho)); //alocação de memória para a primeira carta
        if((*carta_jogador)==NULL)
        {
            printf("falha ao alocar memória para a primeira carta do jogador\n");
        }
        (*carta_jogador)->next = NULL;
    }
    else//para todos os outros casos
    {
        aux = (*carta_jogador);
        (*carta_jogador) = (baralho*) calloc(1, sizeof(baralho)); //a nova carta vai sempre para o topo da pilha
        (*carta_jogador)->next= aux; //a nova carta colocada na pilha aponta para a carta anteriormente colocada, que por sua vez apontava para a anterior
        if((*carta_jogador)==NULL)                                                                             //para assim se ter a sequência das cartas
            printf("falha ao alocar memóriam para uma nova carta de um jogador\n");
    }
    (*carta_jogador)->carta = carta_tirada;
    jogador_actual->player.pontuacao += calcular_pontos(carta_tirada.valor); //assim que dá carta ao jogador ajusta-lhe a pontuação
    jogador_actual->player.num_cartas++; //o número de cartas do jogador incrementa

    if(carta_tirada.valor == 13) //para o caso da carta ser um ás
        jogador_actual->player.ases++;
}
///Função que recebe com entrada a data de uma carta e a estrutura da mesa, e esquema igual ao 'push_jogador'
void push_mesa(_mesa *mesa, s_carta carta_tirada)
{
    baralho *aux=NULL;
    if ((*mesa).carta_mesa == NULL) //caso a mesa não tenha cartas
    {
        (*mesa).carta_mesa = (baralho*) calloc(1, sizeof(baralho));
        if((*mesa).carta_mesa == NULL)
            printf("falha ao alocar memória para a primeira carta do jogador\n");

        (*mesa).carta_mesa->next = NULL;
    }
    else
    {
        aux = (*mesa).carta_mesa;
        (*mesa).carta_mesa = (baralho*) calloc(1, sizeof(baralho));
        (*mesa).carta_mesa->next= aux;
        if((*mesa).carta_mesa == NULL)
            printf("falha ao alocar memóriam para uma nova carta de um jogador\n");
    }
    (*mesa).carta_mesa->carta = carta_tirada;
    (*mesa).pontuacao += calcular_pontos(carta_tirada.valor);
    (*mesa).num_cartas++;

    if(carta_tirada.valor == 13)
        (*mesa).ases++;
}
///Função que recebe o ponteiro para uma pilha de cartas e inverte a pilha(as cartas apontam umas para as outras no sentido inverso)
void inv_pilha (baralho **carta)
{
    if((*carta) != NULL) //Caso existam cartas para inverter
    {
        baralho *aux1=NULL, *aux2=NULL;
        if((*carta)->next->next !=NULL) // so faz este troço se tiver pelo menos 3 cartas
        {
            aux1 = (*carta);
            aux2 = aux1->next;
            (*carta) = aux2->next;
            aux1->next = NULL;          //colocar a 'antiga' cabeça da pilha para NULL pois vai ser a última
            while ((*carta)->next != NULL)
            {
                aux2->next = aux1;
                aux1 =aux2;
                aux2 = (*carta);
                (*carta) = (*carta)->next;      //trocar o sentido
            }
            aux2->next = aux1;
            (*carta)->next =aux2;
        }
        else //para o caso de serem duas cartas
        {   //(não é preciso para o caso de ser1, pois não acontece durante o jogo, ou não tem cartas ou tem duas ou mais)
            aux1 = (*carta);
            (*carta) =(*carta)->next;
            (*carta)->next = aux1;
            aux1->next = NULL;
        }
    }
}
