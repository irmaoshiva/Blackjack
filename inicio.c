#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "estruturas_e_prototipos.h"

///função que lê do ficheiro de confiuração o número de baralhos e a informação nessecária relativamente aos jogadores e os inicializa de modo a estarem aptos a começar o jogo
void ficheiro_config(int *num_jogadores, int *num_baralhos, jogador **head_jogador, _mesa *mesa, char *args[])
{
    FILE *fp;
    jogador*aux=NULL; // ponteiro que vai percorrendo a lista à medida que a vai criando
    (*mesa).saldo = 0;// inicializar o saldo da mesa no início do jogo

    fp = fopen(args[1], "r"); // 'r' pois queremos ler do ficheiro
    fscanf(fp, "%d-%d", &(*num_baralhos), &(*num_jogadores)); // leitura dos inteiros número de baralhos e número de jogadores
    if ( ((*num_baralhos) < 4 || (*num_baralhos)>8) || ((*num_jogadores < 1) || (*num_jogadores) > 4) )//sair se o ficheiro de configuração estiver
    {                                                                                                         //mal escrito
        printf("número de jogadores ou número de baralhos inválido, tem de estar entre 1 e 4 inclusive e entre 4 e 8 inclusive, respectivamente.\n");
        exit(EXIT_FAILURE);
    }

    for(int i=0; i< (*num_jogadores); i++) //alocar memória para a lista com todos os jogadores
    {
        if (*head_jogador==NULL) // para o caso de ser o primeiro jogador
        {
            (*head_jogador) = (jogador*)calloc(1, sizeof(jogador));
            if((*head_jogador)==NULL)
                printf("falha ao alocar memória no primeiro jogador\n");  //verificar se não houve problemas na alocação de memória

            aux=(*head_jogador);
        }
        else //para o resto dos jogadores
        {
            aux->next=(jogador*)calloc(1, sizeof(jogador));
            aux=aux->next;
            if(aux==NULL)
                printf("falha ao alocar memória para o jogador %d\n", i+1); //verificar se não houve problemas na alocação de memória

            aux->next=NULL;
        }
        ini_jogador(aux, 1); //inicialização total do jogador
        aux->player.table_pos =i; // definir a posição do jogador na mesa

        //ler do ficheiro as strings tipo e nome e também  dinheiro(float) e a aposta(int)
        fscanf(fp, "\n%[^-]-%[^-]-%f-%d", aux->player.tipo, aux->player.nome, &(aux->player.dinheiro),&(aux->player.aposta));
        aux->player.apostado = aux->player.aposta; //dinheiro que o jogador mete na mesa é o dinheiro definido como aposta
        if ( ((strcmp(aux->player.tipo, "EA")) != 0 && (strcmp(aux->player.tipo, "HU") != 0)) || (aux->player.dinheiro < 10 || aux->player.dinheiro > 500) || (aux->player.aposta < 2 || aux->player.aposta > (aux->player.dinheiro)*0.25) ) //sair se o ficheiro de configuração estiver mal escrito
        {
            printf("tipo, dinheiro ou aposta do jogador %s inválido, há dois tipos possíveis 'EA', e 'HU', o dinheiro tem de estar compreendido entre 10 e 500 inclusive, e a aposta tem de estar entre 2 e 1/4 do dinheiro também inclusive\n", aux->player.nome);
            exit(EXIT_FAILURE);
        }
        (*mesa).saldo += aux->player.dinheiro; //somar ao saldo da mesa o dinheiro inicial de todos os jogadores
    }
    fclose(fp); //fechar o ficheiro
}
///função que lê de um ficheiro a informação para ações a praticar no jogo pelas "EA" e a guarda em duas matriz (hard e soft)
void ficheiro_ea(int ***ea_hard, int ***ea_soft, char *args[])
{
    FILE *fp;
    fp = fopen(args[2], "r");

    (*ea_hard) = (int**) calloc(10, sizeof(int*)); //alocação do vetor com 10 ponteiros que vão apontar para as primeiras 10 linhas da matriz
    if ((*ea_hard) == NULL)
        printf("falha ao alocar memória para os ponteiros para as linhas do ea_hard.\n");

    for (int i=0; i<10; i++)
    {
        (*ea_hard)[i] = (int*) calloc(10, sizeof(int));//alocação de cada linha de 10 colunas da matriz que ficam a ser
        if (((*ea_hard)[i]) == NULL)                   //dadas pelos ponteiros do vetor alocado em cima
            printf("falha ao alocar memória para um das linhas do ea_hard.\n");

        for(int j=0; j<10; j++)
        {
            fscanf(fp, "%d", &((*ea_hard)[i][j])); //Leitura das decisões da entidade artificial do ficheiro para a respectiva posição da matriz
        }
    }

    (*ea_soft) = (int**) calloc(7, sizeof(int*));//alocação do vetor com 7 ponteiros que vão apontar para as últimas 7 linhas da matriz
    if ((*ea_soft) == NULL)
        printf("falha ao alocar memória para os ponteiros para as linhas do ea_soft.\n");

    for (int k=0; k<7; k++)
    {
        (*ea_soft)[k] = (int*) calloc(10, sizeof(int));//nova alocação de cada linha de 10 colunas da matriz que fica a ser
        if (((*ea_soft)[k]) == NULL)                       //dadas pelos ponteiros do novo vetor alocado em cima
            printf("falha ao alocar memória para um das linhas do ea_hard.\n");

        for(int l=0; l<10; l++)
        {
            fscanf(fp, "%d", &((*ea_soft)[k][l]));//continuação da leitura das decisões da entidade artificial do ficheiro para a respectiva posição da matriz
        }
    }
    fclose(fp); //fechar o ficheiro
}
///função que cria o baralho total ordenado por cartas e por naipes
void criar_baralho(int num_baralhos, baralho **head_baralho)
{
    baralho *aux=NULL; //ponteiro que percorre a lista do baralho à medida que a cria
    int b=0; // para atribuir o naipe à carta de 1 a 4

    for(int i=0; i<num_baralhos*52; i++)
    {
        if ((*head_baralho)==NULL) // para o caso de ser a primeira carta a ser "criada"
        {
            (*head_baralho)=(baralho*)calloc(1, sizeof(baralho));
            if((*head_baralho)==NULL)
                printf("falha ao alocar memória para a primeira carta do baralho\n"); //verificar se não houve problemas na alocação de memória

            aux=(*head_baralho);
        }
        else //para as restantes cartas
        {
            aux->next = (baralho*)calloc(1, sizeof(baralho));
            aux=aux->next;
            if(aux==NULL)
                printf("falha ao alocar memória para a carta %d do baralho\n", i+1); //verificar se não houve problemas na alocação de memória

        }
        aux->next=NULL;// sempre que se cria uma nova carta enquanto não se criar a pŕoxima, o ponteiro desta fica aapontar para NULL!
        aux->carta.valor= (i%13) +1; //dar repetida e sequencialmente à carta os valores de 1 a 13
        aux->carta.naipe= b +1;//  ""   ""  ""             ""           ""        naipes de 1 a 4

        if( (i%13) == 12) //mudar para o próximo naipe quando se chega ao último valor do anterior
            b++;

        if(b==4) // quando chega ao 4 naipe acaba um baralho, o naipe volta a 0 para o próximo
            b=0;
    }
}
/// Função que distribui as primeiras duas cartas para todos os jogadores e mesa
void dist_cartas_inicial(baralho**head_baralho, int*num_total, jogador* head_jogador,int num_jogadores, _mesa *mesa, int num_baralhos, int *hi_lo)
{
    jogador *aux = NULL; //ponteiro que percorre duas vezes cada jogador para se distribuir as cartas uma de cada vez
    (*mesa).ases = 0;
    (*mesa).bj = 0;
    (*mesa).pontuacao = 0;
    (*mesa).bust = 0;
    (*mesa).carta_mesa = NULL;

    for(int j=0; j<2; j++) //dá duas vezes uma a cada um
    {
        aux = head_jogador; //Colocar o ponteiro auxiiliar criado a apontar para o primeiro jogador
        for(int i=0; i<num_jogadores; i++)
        {   //Colocar uma carta na pilha de cartas do jogador
            push_jogador(&(aux->player.carta_jogador) , tirar_carta_aleatoria(&(*head_baralho), &(*num_total), num_baralhos, &(*hi_lo)), aux);
            if (aux->player.pontuacao == 21) //reconherçer logo os blackjack's
                aux->player.bj = 1;

            if (aux->player.ases == 2) //para o caso específico de calharem dois ases nas duas primeiras cartas
            {
                aux->player.pontuacao -= 10;
                aux->player.ases--;
            }
            aux->player.dinheiro -= (aux->player.apostado)/(2.0); //tira duas vezes metade da aposta ao dinheiro
            aux=aux->next;
        }
        push_mesa(&(*mesa), tirar_carta_aleatoria(&(*head_baralho), &(*num_total), num_baralhos, &(*hi_lo))); //Colocar uma carta na pilha de cartas da mesa
    }
    if ((*mesa).pontuacao == 21)
        (*mesa).bj =1;

    (*mesa).num_cartas = 1;//para só ficar desenhada uma carta virada para cima
}
