#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "estruturas_e_prototipos.h"
///Função que distribui as cartas todas para a mesa depois de todos os jogadores terem jogado
void dist_mesa(_mesa *mesa, baralho **head_baralho, int *num_total, int num_baralhos, int *hi_lo)
{
    if ((*mesa).ases == 2)//para o caso das duas primeiras serem ases
    {
        (*mesa).pontuacao -= 10;
        (*mesa).ases--;
    }
    while((*mesa).pontuacao <17)//para com pontuação maior ou igual a 17
    {
        push_mesa(&(*mesa), tirar_carta_aleatoria(&(*head_baralho), &(*num_total), num_baralhos, &(*hi_lo)));
        if((*mesa).pontuacao >21)
        {
            if ((*mesa).ases>0)//se tiver ases a valer 11 afinal faz bust
            {
                (*mesa).pontuacao -= 10;
                (*mesa).ases --;
            }
            else //para o caso da mesa fazer bust
                (*mesa).bust = 1;
        }
    }
}
///Função que paga ou tira dinheiro em função das pontuações aos jogadores no fim da jogada e conota as vitórias, etc
void pagamento_jogada(jogador *head_jogador, int num_jogadores, _mesa mesa)
{
    jogador *aux=NULL;
    aux = head_jogador; //auxiliar que percorre

    for (int i =0; i<num_jogadores; i++)
    {
        if(aux->player.bj == 1 && mesa.bj == 0)
        {
            aux->player.vitorias++;
            aux->player.dinheiro += (aux->player.apostado)*(2.5);
        }
        else
        {
            if (mesa.bj ==1 && aux->player.bj == 0)
            {
                aux->player.derrotas++;
            }
            else
            {
                if (aux->player.bust == 0 && aux->player.desistiu == 0)
                {
                    if(mesa.bust == 1 || mesa.pontuacao < aux->player.pontuacao)
                    {
                        aux->player.dinheiro += 2*(aux->player.apostado);
                        aux->player.vitorias++;
                    }
                    else
                    {
                        if (mesa.pontuacao > aux->player.pontuacao)
                        {
                            aux->player.derrotas++;
                        }
                    }

                    if (aux->player.pontuacao == mesa.pontuacao)
                    {
                        aux->player.dinheiro += aux->player.apostado;
                        aux->player.empates++;
                    }
                }
                else//sempre que o jogador faz bust perde, mesmo que a mesa também o faça
                {
                    aux->player.derrotas++;
                }
            }
        }
        aux->player.apostado = 0;
        aux = aux->next;
    }
}
///Função que tira um jogador da mesa (sai da lista de jogadores, e vai para outra lista para ficar lá guardado), que tem como entrada esse mesmo jogador e
void jogador_sai(jogador **lista_final, jogador **head_jogador, jogador **jogador_actual)///o ponteiro para a lista de jogadores para poder fazer os ajustes
{                                                                                        ///à lista.
    jogador *aux1=NULL, *aux2=NULL, *aux3=NULL;
    aux1 = (*jogador_actual);
    aux2 = (*lista_final);
    aux3 = (*head_jogador);
    (*jogador_actual) = (*jogador_actual)->next;
    if (aux1 == (*head_jogador))    //para o caso de ser o primeiro jogador da lista a ser o retirado
    {
        (*head_jogador) = (*head_jogador)->next;
        aux1->next = NULL;
    }
    else//para todos os outros casos
    {
        while(aux3->next != aux1)
        {
            aux3 = aux3->next;
        }
        aux3->next = aux1->next;
        aux1->next =NULL;//vai entrar na próxima lista a apontar para NULL
    }

    if (aux2 !=NULL)       //Caso a lista dos jogadores eliminados tenha jogadores
    {
        while(aux2->next !=NULL)
        {
            aux2 = aux2->next;
        }
        aux2->next = aux1;
    }
    else        //Caso a lista dos jogadores eliminados não tenha jogadores
        (*lista_final) = aux1;
}
///Função que permite alterar a aposta de um jogador, lendo-a do teclado
void ler_aposta(jogador *head_jogador)
{
    char name[10];
    char aux_a [sizeof(int)];
    int nova_aposta=0;
    int p=0;//inteiro utilizado para saber quando sair dos ciclos 'while' desta função
    jogador *aux=NULL;

    aux = head_jogador;
    printf("\ndiga o nome do jogador no qual quer alterar a aposta:\n");
    fgets( name, 10, stdin);    // O tamanho máximo do nome é igual a 8 letras
    name[strlen(name)-1] = '\0';

    while(aux != NULL && p==0)
    {
        if(strcmp(aux->player.nome, name) == 0 && strcmp(aux->player.tipo, "EA") != 0) //entra no if se o nome existir na mesa e se não for nenhum "EA"
            p=1;                                                                       //caso não entre no if,faz um print e sai da função para permitir
        else                                                                           //ao utilizador continuar a jogar sem alterar a aposta de ninguém
            aux = aux->next;
    }
    if (aux == NULL)
    {
        printf("inseriu um nome inválido, ou de uma entidade artificial.\nvolte a pressionar 'b' se continua a querer alterar a aposta de um jogador.\nvolte ao quadrado de jogo.\n");
        return;
    }
    else
    {
        while(p == 1)
        {
            printf("escreva o novo valor da aposta:\n");
            fgets(aux_a, sizeof(int), stdin);
            sscanf(aux_a, "%d", &nova_aposta);
            if (nova_aposta < 1)
                printf("aposta inválida.\n\n"); //não é necessário cobrir o caso da aposta ser maior que o dinheiro do jogador, pois se isso
            else                                //acontecer,  no new_game isso é ajustado e o jogador faz "all in" automaticamente
                p = 2;
        }
        aux->player.aposta = nova_aposta;
        printf("alteracao feita, retorne ao quadrado de jogo\n\n");
    }
}
///Função que permite adicionar um jogador ao jogo onde houver espaço livre, o inteiro 'num_casa' representa o número da casa onde se foi
void add_jogador(jogador **head_jogador, int num_casa, int *casas_livres, int *num_jogadores, _mesa *mesa)  ///clicado com o rato (de 0 a 3)
{
    char aux_numeros[7];
    char aux_letras[10];         //à medida que vão sendo adicionados jogadores à lista, o jogadores estão posicionados na lista sempre
                                       //pela ordem em que vão estar na mesa
    jogador * aux=NULL, *aux2=NULL;
    aux = (*head_jogador);
    aux2 = aux;

    while(aux!=NULL)
    {
        if (aux->player.table_pos == num_casa)//se já houver alguém nessa casa, não é possível adicionar mais ninguém, sai de imediato da função
            return;
        else
            aux = aux->next;
    }

    aux = (jogador*)calloc(1, sizeof(jogador)); //alocar o novo jogador a adiconar à lista de jogadores
    if(aux == NULL)
        printf("falha ao alocar memória para o novo jogador a ser inserido.\n");

    if (num_casa == 0 || (*head_jogador) == NULL)//para o caso de se estar a adicionar na primeira casa
    {
        aux->next = (*head_jogador);
        (*head_jogador) = aux;
    }
    else //para todos os outros casos
    {
        while((aux2->next != NULL) && (aux2->player.table_pos < num_casa))
        {
            aux2 = aux2->next;
        }
        aux->next = aux2->next;
        aux2->next = aux;
    }

    while ((strcmp(aux->player.tipo, "EA")) != 0 && (strcmp(aux->player.tipo, "HU") != 0)) //para o caso de ser um tipo inválido, volta a perguntar até
    {                                                                                       //que seja um tipo válido
        printf("\nescreva o tipo do novo jogador (EA-entidade artificial) ou (HU-humano):\n");
        fgets( aux_letras, 10, stdin);
        aux_letras[strlen(aux_letras)-1]='\0';
        strcpy(aux->player.tipo, aux_letras);

        if ((strcmp(aux->player.tipo, "EA")) != 0 && (strcmp(aux->player.tipo, "HU") != 0))
            printf("tipo inválido.");
    }
    printf("escreva o nome do novo jogador (máx 8 letras):\n");
    fgets( aux_letras, 10, stdin);
    aux_letras[strlen(aux_letras)-1]='\0';
    strcpy(aux->player.nome, aux_letras);

    printf("escreva o valor monetário com que o jogador entra na mesa:\n");
    fgets(aux_numeros, 7, stdin);
    sscanf(aux_numeros, "%f", &(aux->player.dinheiro));

    while(aux->player.aposta < 1)
    {
    printf("e escreva o valor monetário  para as apostas do jogador:\n");  // não é preciso rejeitar se a aposta for inferior ao dinheiro, pois
    fgets(aux_numeros, 7, stdin);                                          //sempre que isto acontece, na função new_game a aposta é
    sscanf(aux_numeros, "%d", &(aux->player.aposta));                      //ajustada e o jogador faz automaticamente "all in"
    printf("pronto para jogar, retorne ao quadrado de jogo!\n");
    if(aux->player.aposta < 1)
        printf("aposta inválida\n.");
    }

    (*mesa).saldo += aux->player.dinheiro;//aumenta o saldo da mesa com o valor com que o novo jogador entra
    ini_jogador(aux, 1);//inicializar por completo o jogador, para um jogo novo
    aux->player.table_pos = num_casa;
    (*num_jogadores)++;
    (*casas_livres) = 0;//sai do modo de adicionar jogadores
}
///Função que começa o início de uma nova jogada
void new_game (jogador **jogador_actual, jogador **head_jogador, baralho **head_baralho, int *num_total, int *num_jogadores, _mesa *mesa, jogador **lista_final, int num_baralhos, int *hi_lo, int num_cartas)
{
    (*jogador_actual) = (*head_jogador);

    clear_mesa(&(*mesa)); //limpa  pilha de cartas da mesa
    for (int i = 0; i <(*num_jogadores); i++)
    {
        if ((*jogador_actual)->player.dinheiro < 1) //para o caso de ficar sem dinheiro e cobrir também o caso de ter cêntimos, pois a aposta é um inteiro
        {                                                                                                    //e nunca iria apostar
            jogador_sai(&(*lista_final), &(*head_jogador), &(*jogador_actual));
            (*num_jogadores)--;

            if ( i == (*num_jogadores)) // se tirar o último jogador já não faz o resto da função e dá logo as cartas e mete
            {                               //o jogador acutal a apontar para o primeiro
                dist_cartas_inicial(&(*head_baralho), &(*num_total), (*head_jogador), (*num_jogadores), &(*mesa), num_baralhos, &(*hi_lo));
                (*jogador_actual) = (*head_jogador);
                return;
            }
        }
        if (strcmp((*jogador_actual)->player.tipo, "EA") == 0) //ajusta o que o "EA" aposta em cada joagada em função da estratégia hi lo
        {
            if ((*hi_lo)/((num_cartas/52)+1)  <= 0)
                (*jogador_actual)->player.apostado = ((*jogador_actual)->player.aposta);
            else
            {
                if((*hi_lo)/((num_cartas/52)+1) <=2)
                    (*jogador_actual)->player.apostado = ((*hi_lo)/((num_cartas/52)+1))*((*jogador_actual)->player.aposta)*2;
                else
                    (*jogador_actual)->player.apostado = ((*jogador_actual)->player.aposta)*2*((*hi_lo)/((num_cartas/52)+1) +2);
            }
            if ((*jogador_actual)->player.apostado > (*jogador_actual)->player.dinheiro) //para o caso de 'querer' apostar mais do que aquilo que tem
                (*jogador_actual)->player.apostado = (*jogador_actual)->player.dinheiro;
        }
        else
        {
            if((*jogador_actual)->player.dinheiro < (*jogador_actual)->player.aposta) //para o caso de 'querer' apostar mais do que aquilo que tem
                (*jogador_actual)->player.apostado = (*jogador_actual)->player.dinheiro;
            else
                (*jogador_actual)->player.apostado = (*jogador_actual)->player.aposta;
        }

        ini_jogador((*jogador_actual), 0); //inicializa os jogadores para a nova jogada
        (*jogador_actual) = (*jogador_actual)->next;
    }
    dist_cartas_inicial(&(*head_baralho), &(*num_total), (*head_jogador), (*num_jogadores), &(*mesa), num_baralhos, &(*hi_lo));
    (*jogador_actual) = (*head_jogador);
}
///função que escreve o ficheiro com os resultados do jogo
void escrever_ficheiro(jogador *head_jogador, jogador *lista_final, _mesa *mesa)
{
    FILE *status;

    status = fopen ("status.txt", "w");
    while(head_jogador != NULL)
    {
        fprintf (status, "%s (%s): \n ganhou %d jogadas;\n perdeu %d jogadas;\n empatou %d jogadas;\n\n",head_jogador->player.nome, head_jogador->player.tipo, head_jogador->player.vitorias, head_jogador->player.derrotas, head_jogador->player.empates);
        (*mesa).saldo -= head_jogador->player.dinheiro;
        head_jogador = head_jogador->next;
    }
    while(lista_final != NULL)
    {
        fprintf (status, "%s (%s): \n ganhou %d jogadas;\n perdeu %d jogadas;\n empatou %d jogadas;\n\n",lista_final->player.nome, lista_final->player.tipo, lista_final->player.vitorias, lista_final->player.derrotas, lista_final->player.empates);
        (*mesa).saldo -= lista_final->player.dinheiro;
        lista_final = lista_final->next;
    }
    fprintf(status, "Saldo da Mesa: %.1f", (*mesa).saldo);
    fclose(status);
}
