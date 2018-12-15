#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "estruturas_e_prototipos.h"
/// Função que efetua a jogada stand
void func_stand(jogador **jogador_actual, _mesa *mesa, baralho **head_baralho, int *num_total, jogador *head_jogador, int num_jogadores, int num_baralhos, int *hi_lo)
{
    if((*jogador_actual)!=NULL)
    {
        if((*jogador_actual)->next != NULL)//até ao último jogador, exclusive
            (*jogador_actual)=((*jogador_actual)->next); // passa ao próximo jogador
        else//para o último jogador
        {
            (*mesa).num_cartas++;//número de cartas da mesa volta ao real pois agora já vai mostrar todas as cartas que tem
            (*jogador_actual)=NULL; //desde o último jogador falar até ao new_game o jogador actual fica a apontar para NULL para impossibilitar todas as tentivas de joagada
            dist_mesa(&(*mesa), &(*head_baralho), &(*num_total), num_baralhos, &(*hi_lo)); //dar carats à mesa
            pagamento_jogada(head_jogador, num_jogadores, (*mesa)); //fazer os pagamentos da jogada
        }
    }
}
/// Função que efetua a jogada hit
void func_hit(jogador **jogador_actual, _mesa *mesa, baralho**head_baralho, int*num_total, jogador *head_jogador, int num_jogadores, int num_baralhos, int *hi_lo)
{
    if((*jogador_actual)!=NULL)
    {                                          //dá uma carta ao jogador
        push_jogador(&((*jogador_actual)->player.carta_jogador), tirar_carta_aleatoria(&(*head_baralho), &(*num_total), num_baralhos, &(*hi_lo)), (*jogador_actual)); //Coloca a carta na pilha
        if((*jogador_actual)->player.pontuacao>21)
        {
            if((*jogador_actual)->player.ases!=0)//se tiver um ás a valer 11 então afinal não faz bust
            {
                (*jogador_actual)->player.pontuacao-=10;
                (*jogador_actual)->player.ases--;
            }
            else //para o caso de fazer bust
            {
                (*jogador_actual)->player.bust = 1;
                (*mesa).saldo += (*jogador_actual)->player.apostado;
                (*jogador_actual)->player.apostado = 0; //perde o dinheiro que tinha em jogo
                func_stand(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));//auto stand
            }
        }
    }
}
/// Função que efetua a jogada double, isto é o jogador actual dobra o dinheiro que tem na mesa apostado e faz um só hit independetemente do resultado
void func_double(jogador **jogador_actual, _mesa *mesa, baralho **head_baralho, int *num_total, jogador *head_jogador, int num_jogadores, int num_baralhos, int *hi_lo)
{
    jogador *aux = NULL;
    aux = (*jogador_actual);//auxiliar para 'gravar' o jogador actual
     //dobra o dinheiro que estava na mesa apostado
    (*jogador_actual)->player.dinheiro -= (*jogador_actual)->player.apostado;
    (*jogador_actual)->player.apostado += (*jogador_actual)->player.apostado;
    func_hit(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));//pede uma carta
    if ((*jogador_actual) == aux)//faz um stand logo a seguir excepto se excedeu e nesse caso, o stand já vem feitoe o jogador actual já mudou
        func_stand( &(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
}
/// Função que efetua a jogada surrender, isto é o jogador perde logo a jogada mas recupera metade do que tinha na mesa apostado
void func_surrender(jogador **jogador_actual, _mesa *mesa, baralho **head_baralho, int *num_total, jogador *head_jogador, int num_jogadores, int num_baralhos, int *hi_lo)
{
    (*jogador_actual)->player.dinheiro += 0.5*((*jogador_actual)->player.apostado);
    (*jogador_actual)->player.apostado = 0;
    (*jogador_actual)->player.desistiu = 1;
    (*jogador_actual)->player.num_cartas = 0;
    func_stand( &(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
}
///Função que decide as ações do jogador actual quando este é "EA", através da informação das matrizes
void func_ea(jogador **jogador_actual, _mesa *mesa, baralho **head_baralho, int *num_total,jogador *head_jogador, int num_jogadores, int num_baralhos, int **ea_hard, int **ea_soft, int *hi_lo)
{
    int pont_mesa;
    int num_acao =5; // os números da matriz nunca chegam a 5 então se este número for 5 é porque não foi alterado e o switch não é utilizado
    pont_mesa = (*mesa).pontuacao - calcular_pontos((*mesa).carta_mesa->carta.valor);//a mesa só tem duas cartas logo o topo da pilha é a segunda carta e
                                                                                     //queremos decidir só em função da pontuação da primeira carta
    if ((*jogador_actual)->player.ases == 0) //para o caso de não ter ases a valer 11(hard)
    {
        if((*jogador_actual)->player.pontuacao < 8)
            func_hit(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
        else
        {
            if((*jogador_actual)->player.pontuacao > 17)
                func_stand(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
            else //lê da matriz o que fazer se tiver pontuações entre 8 e 17
            {
                num_acao = ea_hard[((*jogador_actual)->player.pontuacao)-8][(pont_mesa)-2]; //localizar a linha e coluna selecinada pelos
            }                                                                                // pontos da mesa e jogador
        }
    }
    else //para o caso de ter ases a valer 11(soft)
    {
        if((*jogador_actual)->player.pontuacao > 19)
            func_stand(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
        else
        {
            if ((*jogador_actual)->player.pontuacao < 13)
                func_hit(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
            else //lê da matriz o que fazer se tiver pontuações entre 13 e 19
                num_acao = ea_soft[((*jogador_actual)->player.pontuacao)-13][(pont_mesa)-2]; //localizar a linha e coluna selecinada pelos
        }                                                                                    // pontos da mesa e jogador
    }
    if (num_acao !=5)
    {
        switch (num_acao)
        {
        case 0:   //s
            func_stand(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
            break;
        case 1:   //h
            func_hit(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
            break;
        case 2:   //Ds
            if ((*jogador_actual)->player.num_cartas == 2 && (*jogador_actual)->player.apostado <= (*jogador_actual)->player.dinheiro)  //Caso não tenha duas cartas não pode dar double
                func_double(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
            else
                func_stand(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
            break;
        case 3:   //Dh
            if ((*jogador_actual)->player.num_cartas == 2 && (*jogador_actual)->player.apostado <= (*jogador_actual)->player.dinheiro)  //Caso não tenha duas cartas não pode dar double
                func_double(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
            else
                func_hit(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
            break;
        default:  //Rh
            if ((*jogador_actual)->player.num_cartas == 2)     // Caso não tenha duas cartas não pode dar surrender
                func_surrender(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
            else
                func_hit(&(*jogador_actual), &(*mesa), &(*head_baralho), &(*num_total), head_jogador, num_jogadores, num_baralhos, &(*hi_lo));
            break;
        }
    }
}
