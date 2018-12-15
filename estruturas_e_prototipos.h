#ifndef ESTRUTURAS_H_INCLUDED
#define ESTRUTURAS_H_INCLUDED

///estrutura das cartas
typedef struct
{
    int valor; //de 1 a 13
    int naipe; //de 0 a 4
} s_carta;
///estrutura para a lista de cartas

typedef struct lista_carta
{
    s_carta carta;
    struct lista_carta *next;
} baralho;

///estrutura dos jogadores
typedef struct
{
    char nome[9];
    char tipo[3];
    int aposta; //dinheiro pré-definido para as apostas do jogador no jogo
    int apostado; //dinheiro que ele tem na mesa apostado
    float dinheiro;
    baralho *carta_jogador; //ponteiro para a pilha de cartas que o jogador possui
    int pontuacao;
    int bj;  //se tem blackjack(1) ou não(0)
    int ases; //número de ases a valer 11
    int bust; // se fez bust(1) ou não(0)
    int num_cartas; //número de cartas que o jogador possui
    int empates;
    int vitorias;
    int derrotas;
    int desistiu; // se o jogador desistiu(1) ou não(0)
    int table_pos; // posição do jogaodor na mesa(número da casa em que joga) de 0 a 3
} s_jogador;

///estrutura para a lista de jogadores
typedef struct lista_jogador
{
    s_jogador player;
    struct lista_jogador *next;
} jogador;

///estrutura da mesa
typedef struct
{
    int pontuacao;
    int bj;        //igual ao jogador
    int ases;      // ""   ""   ""
    int bust;      // ""   ""   ""
    int num_cartas;// ""   ""   ""
    float saldo;  // saldo final da mesa, tudo o que ganhou ou perdeu para todos os jogadores que jogaram até se fazer quit
    baralho *carta_mesa; //ponteiro para a pilha de cartas que a mesa possui
} _mesa;

//inicio.c
void ficheiro_config(int *num_jogadores, int *num_baralhos, jogador**head_jogador, _mesa *mesa, char *args[]);
void ficheiro_ea(int ***ea_hard, int ***ea_soft, char *args[]);
void criar_baralho(int num_baralhos, baralho**head_baralho);
void dist_cartas_inicial(baralho**head_baralho, int*num_total, jogador* head_jogador,int num_jogadores, _mesa *mesa, int num_baralhos, int *hi_lo);
//funcoes_auxiliares.c
void ini_jogador(jogador *jogador, int flag);
s_carta tirar_carta_aleatoria(baralho**head_baralho, int *num_total, int num_baralhos, int *hi_lo);
int calcular_pontos(int numero);
//pilhas.c
void push_jogador(baralho**carta_jogador, s_carta carta_tirada, jogador * jogador_atual);
void push_mesa(_mesa *mesa, s_carta carta_tirada);
void inv_pilha (baralho **carta);
//jogada.c
void func_stand(jogador **jogador_actual, _mesa *mesa, baralho **head_baralho, int *num_total,jogador *head_jogador, int num_jogadores, int num_baralhos, int *hi_lo);
void func_hit(jogador **jogador_actual, _mesa *mesa, baralho**head_baralho, int*num_total, jogador *head_jogador, int num_jogadores, int num_baralhos, int *hi_lo);
void func_double(jogador **jogador_actual, _mesa *mesa, baralho **head_baralho, int *num_total, jogador *head_jogador, int num_jogadores, int num_baralhos, int *hi_lo);
void func_surrender(jogador **jogador_actual, _mesa *mesa, baralho **head_baralho, int *num_total, jogador *head_jogador, int num_jogadores, int num_baralhos, int *hi_lo);
void func_ea(jogador **jogador_actual, _mesa *mesa, baralho **head_baralho, int *num_total,jogador *head_jogador, int num_jogadores, int num_baralhos, int **ea_hard, int **ea_soft, int *hi_lo);
//fim_jogada.c
void dist_mesa(_mesa *mesa, baralho **head_baralho, int *num_total, int num_baralhos, int *hi_lo);
void pagamento_jogada(jogador *head_jogador, int num_jogadores, _mesa mesa);
void jogador_sai(jogador **lista_final, jogador **head_jogador, jogador **jogador_actual);
void ler_aposta(jogador *head_jogador);
void add_jogador(jogador **head_jogador, int num_casa, int *casas_livres, int *num_jogadores, _mesa *mesa);
void new_game (jogador **jogador_actual, jogador **head_jogador, baralho **head_baralho, int *num_total, int *num_jogadores, _mesa *mesa, jogador **lista_final, int num_baralhos, int *hi_lo, int num_cartas);
void escrever_ficheiro(jogador *head_jogador, jogador *lista_final, _mesa *mesa);
//frees.c
void clear_mesa (_mesa *mesa);
void clear_jogador(baralho **carta_jogador);
void free_matriz(int ***ea_hard, int ***ea_soft);
void free_all(jogador *head_jogador, baralho *head_baralho, _mesa mesa, jogador *lista_final, int ***ea_hard, int ***ea_soft);
#endif // ESTRUTURAS_H_INCLUDED
