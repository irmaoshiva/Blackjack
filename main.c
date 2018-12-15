#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH_WINDOW 900      // window width
#define HEIGHT_WINDOW 525     // window height
#define CARD_WIDTH 67         // card width
#define CARD_HEIGHT 97        // card height
#define WINDOW_POSX 500       // initial position of the window: x
#define WINDOW_POSY 250       // initial position of the window: y
#define EXTRASPACE 150
#define MARGIN 5

#include "estruturas_e_prototipos.h"
#include "prototipos_grafica.h"

/**
 * main function: entry point of the program
 * only to invoke other functions !
 */
int main( int argc, char* args[] )
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *serif = NULL;
    SDL_Surface *cards[MAX_DECK_SIZE+1], *imgs[2];
    SDL_Event event;
    int delay = 300;
    int quit = 0;
    int num_jogadores=0; //número de jogadores em jogo
    int num_baralhos=0; //número de baralhos a utilizar para se fazer o baralho total
    int num_total=0; //número total de cartas que há no baralho sempre actualizado
    int casas_livres=0; //enable para o modo de adicionar jogadores ao jogo através da tecla 'a'
    int hi_lo=0; // inteiro que conta cartas para a estratégia hi lo dos 'EA'
    int **ea_soft; //ponteiro para a matriz alocada com as jogadas a efectuar pelo 'EA' para o caso de este ter ases a valerem 11
    int **ea_hard; //  ""        ""          ""         ""       ""     ""     ""    ""           ""        não ter ases a valerem 11
    int delay_ea=1000; //delay inicial para as jogadas dos 'EA'
    jogador *head_jogador=NULL; //ponteiro para o primeiro jogador da lista de jogadores em jogo
    jogador *jogador_actual=NULL; //ponteiro que aponta para o jogador que está a falar
    jogador *lista_final=NULL; //ponteiro que aponta para o primeiro jogador dos que já saíram do jogo (depois de perderem todo o dinheiro)
    baralho *head_baralho=NULL; // ponteiro que aponta para a primeira carta do baralho
    _mesa mesa; //estrutura da mesa

    if(argc != 3)
    {
        printf("número de ficheiros incorrecto(argc).\n");
        exit(EXIT_FAILURE);
    }
    mesa.saldo = 0; // inicializar o saldo da mesa

    ficheiro_config(&num_jogadores, &num_baralhos, &head_jogador, &mesa, args);
    ficheiro_ea(&ea_hard, &ea_soft, args);
    jogador_actual = head_jogador;//o jogador acutal começa no primeiro jogador da lista
    criar_baralho(num_baralhos, &head_baralho);
    num_total = num_baralhos*52;
    dist_cartas_inicial(&head_baralho, &num_total, head_jogador, num_jogadores, &mesa, num_baralhos, &hi_lo);

    // initialize graphics
    InitEverything(WIDTH_WINDOW, HEIGHT_WINDOW, &serif, imgs, &window, &renderer);
    // loads the cards images
    LoadCards(cards);

    while( quit == 0 )
    {
        if (jogador_actual!=NULL && jogador_actual->player.pontuacao == 21) // cobre os 2 casos, o blackjack e o 21
            func_stand( &jogador_actual, &mesa, &head_baralho, &num_total, head_jogador, num_jogadores, num_baralhos, &hi_lo);

        while (jogador_actual!= NULL && strcmp(jogador_actual->player.tipo, "EA") == 0) //sempre que o jogador actual for "EA", faz o tipo de jogo pré-
        {                                                                               //definido até que passe para a mesa ou para um humano
            SDL_Delay(delay_ea);
            func_ea(&jogador_actual, &mesa, &head_baralho, &num_total, head_jogador, num_jogadores, num_baralhos, ea_hard, ea_soft, &hi_lo);

            RenderTable(serif, imgs, renderer, num_jogadores, head_jogador, jogador_actual, casas_livres, mesa, delay_ea);
            RenderHouseCards(cards, renderer, mesa);
            RenderPlayerCards(cards, renderer, num_jogadores, head_jogador);
            SDL_RenderPresent(renderer);
        }

        // while there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT )
            {
                if(jogador_actual == NULL) //se clicarem na cruz do quadrado de jogo, o programa acaba o jogo (só o faz no fim de uma jogada)
                    quit = 1;
            }
            else if ( event.type == SDL_KEYDOWN )
            {
                switch ( event.key.keysym.sym )
                {
                case SDLK_q:
                    if(jogador_actual == NULL)  // quando for premida a tecla 'q', o programa acaba o jogo (só o faz no fim de uma jogada)
                        quit = 1;
                    break;
                case SDLK_s: //quando for premida a tecla 's', o jogador actual faz a função stand
                        func_stand(&jogador_actual, &mesa, &head_baralho, &num_total, head_jogador, num_jogadores, num_baralhos, &hi_lo);
                    break;
                case SDLK_h:  //quando for premida a tecla 'h', o jogador actual faz a função hit
                    if(jogador_actual != NULL)
                        func_hit(&jogador_actual, &mesa, &head_baralho, &num_total, head_jogador, num_jogadores, num_baralhos, &hi_lo);
                    break;
                case SDLK_n: //quando for premida a tecla 'n', faz nova jogada (só o faz no fim de uma jogada) e se houverem jogadores na mesa
                    if(jogador_actual == NULL && head_jogador !=NULL && casas_livres == 0)
                        new_game(&jogador_actual, &head_jogador, &head_baralho, &num_total, &num_jogadores, &mesa, &lista_final, num_jogadores, &hi_lo, num_total);
                    break;
                case SDLK_b: //quando for premida a tecla 'b', permite alterar a aposta de um jogador (só o faz no fim de uma jogada) e
                    if(jogador_actual == NULL && casas_livres == 0 && head_jogador!=NULL)          // se houverem jogadores na mesa
                        ler_aposta(head_jogador);
                    break;
                case SDLK_d://quando for premida a tecla 'd', o jogador actual dobra aposta e faz um só hit
                    if((jogador_actual != NULL) && (jogador_actual->player.num_cartas == 2) && (jogador_actual->player.dinheiro >= jogador_actual->player.apostado))
                        func_double(&jogador_actual, &mesa, &head_baralho, &num_total, head_jogador, num_jogadores, num_baralhos, &hi_lo);
                    break;
                case SDLK_r://quando for premida a tecla 'r', o jogador actual desiste da jogada e recebe metade do que tinha apostado
                    if((jogador_actual!=NULL) && (jogador_actual->player.num_cartas == 2))
                        func_surrender(&jogador_actual, &mesa, &head_baralho, &num_total, head_jogador, num_jogadores, num_baralhos, &hi_lo);
                    break;
                case SDLK_a://quando for premida a tecla 'a', activa ou desativa modo de adicionar jogadores (só o faz no fim de uma jogada)
                    if(jogador_actual == NULL && num_jogadores < 4)
                    {
                        if(casas_livres == 0)
                            casas_livres = 1;//desativa modo de adicionar jogadores
                        else
                            casas_livres = 0; //desativa modo de adicionar jogadores
                    }
                    break;
                case SDLK_UP: //aumenta o delay dos "EA" (só o faz fora das jogadas dos "EA")
                    if(delay_ea < 5000)
                        delay_ea += 100;
                    break;
                case SDLK_DOWN:// diminui o delay dos "EA" (só o faz fora das jogadas dos "EA")
                    if(delay_ea > 0)
                        delay_ea -= 100;
                    break;
                default:
                    break;

                }
            }
             //lê a posição onde o rato clicou
            else if((event.type == SDL_MOUSEBUTTONDOWN) && (jogador_actual == NULL) && (casas_livres == 1))
            {
                if((event.button.x>11 && event.button.x<215) && (event.button.y>290 && event.button.y<510)) //primeiro quadrado
                    add_jogador(&head_jogador, 0, &casas_livres, &num_jogadores, &mesa);
                if((event.button.x>220 && event.button.x<425) && (event.button.y>290 && event.button.y<510))//segundo quadrado
                    add_jogador(&head_jogador, 1, &casas_livres, &num_jogadores, &mesa);
                if((event.button.x>428 && event.button.x<632) && (event.button.y>290 && event.button.y<510))//terceiro quadrado
                    add_jogador(&head_jogador, 2, &casas_livres, &num_jogadores, &mesa);
                if((event.button.x>637 && event.button.x<840) && (event.button.y>290 && event.button.y<510))//quarto quadrado
                    add_jogador(&head_jogador, 3, &casas_livres, &num_jogadores, &mesa);
            }
        }

        // render game table
        RenderTable(serif, imgs, renderer, num_jogadores, head_jogador, jogador_actual, casas_livres, mesa, delay_ea);
        // render house cards
        RenderHouseCards(cards, renderer, mesa);
        // render player cards
        RenderPlayerCards(cards, renderer, num_jogadores, head_jogador);
        // render in the screen all changes above
        SDL_RenderPresent(renderer);
        // add a delay
        SDL_Delay( delay );

        if (quit==1)
        {
            escrever_ficheiro(head_jogador, lista_final, &mesa); //escreve o ficheiro com os resultados
            free_all(head_jogador, head_baralho, mesa, lista_final, &ea_hard, &ea_soft); //desalocar a memória
        }
    }

    // free memory allocated for images and textures and close everything including fonts
    UnLoadCards(cards);
    TTF_CloseFont(serif);
    SDL_FreeSurface(imgs[0]);
    SDL_FreeSurface(imgs[1]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
