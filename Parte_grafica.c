#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "estruturas_e_prototipos.h"
#include "prototipos_grafica.h"

const char myName[] = "P. Moreira  &  A. Godinho";
const char myNumber[] = "ist - 85228       ist - 84006";

/**
 * RenderTable: Draws the table where the game will be played, namely:
 * -  some texture for the background
 * -  the right part with the IST logo and the student name and number
 * -  squares to define the playing positions of each player
 * -  names and the available money for each player
 * \param _money amount of money of each player
 * \param _img surfaces where the table background and IST logo were loaded
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderTable(TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer* _renderer, int num_jogadores, jogador *head_jogador, jogador *jogador_actual, int casas_livres, _mesa mesa, int delay_ea)
{
    SDL_Color black = { 0, 0, 0 }; // black
    SDL_Color white = { 255, 255, 255 }; // white
    SDL_Color red = {255, 0, 0}; //red
    SDL_Color pontuacao = {0, 255, 0}; // light green
    SDL_Color blue = {0, 0, 255}; // blue
    SDL_Color yellow = {255, 255, 0}; // yellow
    SDL_Color purple = {160, 25, 160}; //purple

    char name_money_str[STRING_SIZE];
    SDL_Texture *table_texture;
    SDL_Rect tableSrc, tableDest, playerRect;
    int separatorPos = (int)(0.95f*WIDTH_WINDOW); // seperates the left from the right part of the window
    int height;

    // set color of renderer to some color
    SDL_SetRenderDrawColor( _renderer, 255, 255, 255, 255 );

    // clear the window
    SDL_RenderClear( _renderer );

    tableDest.x = tableSrc.x = 0;
    tableDest.y = tableSrc.y = 0;
    tableSrc.w = _img[0]->w;
    tableSrc.h = _img[0]->h;

    tableDest.w = separatorPos;
    tableDest.h = HEIGHT_WINDOW;

    table_texture = SDL_CreateTextureFromSurface(_renderer, _img[0]);
    SDL_RenderCopy(_renderer, table_texture, &tableSrc, &tableDest);

    // render the IST Logo
    height = RenderLogo(separatorPos, 0, _img[1], _renderer);

    // render the student name
    height += RenderText(separatorPos+3*MARGIN, height, myName, _font, &black, _renderer);

    // this renders the student number
    RenderText(separatorPos+3*MARGIN, height, myNumber, _font, &black, _renderer);

    height+= 80;  // para começar escrever os nome dos jogadores mais a baixo que o nome dos alunos


    sprintf(name_money_str,"EA delay:");
    RenderText(780, 50, name_money_str, _font, &purple, _renderer);   //escreve o delay actualizado das jogadas das "EA" no campo superior
    sprintf(name_money_str,"%.1fseg", (delay_ea)*0.001);              // direto da mesa a roxo (não é muito nítido pois tamem não é muito importante)
    RenderText(780, 75, name_money_str, _font, &purple, _renderer);

    if (mesa.bust == 0)
    {
        if(mesa.num_cartas == 1) //escreve os pontos da primeira carta mesa
        {
            sprintf(name_money_str,"Mesa: %d pontos", mesa.pontuacao - calcular_pontos(mesa.carta_mesa->carta.valor));
            RenderText(30, 140, name_money_str, _font, &yellow, _renderer);
        }
        else
        {
            if (mesa.bj == 1) //escreve que a mesa tem balck jack a preto
            {
                sprintf(name_money_str,"Mesa: BLACK JACK");
                RenderText(30, 140, name_money_str, _font, &black, _renderer);
            }
            else// escreve os pontos da mesa
            {
                sprintf(name_money_str,"Mesa: %d pontos", mesa.pontuacao);
                RenderText(30, 140, name_money_str, _font, &yellow, _renderer);
            }
        }
    }
    else //escreve que a mesa fez bust
    {
        sprintf(name_money_str,"Mesa: BUST");
        RenderText(30, 140, name_money_str, _font, &red, _renderer);
    }

    if (casas_livres == 1) //escreve que está me MODE ADD PLaYER
    {
        sprintf(name_money_str,"MODE ADD PLAYER!");
        RenderText(10, 120, name_money_str, _font, &blue, _renderer);
    }

    for( int i = 0; i<4 ; i++)//desenha sempre os quatro quadrados a branco, excepto quando o modo de adicionar jogadores está activo e nesse caso desenha
    {                                                                            //a azul e depois os que estão ocupados serão desenhados a branco por cima
        SDL_SetRenderDrawColor(_renderer, 255 - (255*casas_livres), 255 - (255*casas_livres), 255, 255 );
        playerRect.x = i*(separatorPos/4-5)+10;
        playerRect.y = (int) (0.55f*HEIGHT_WINDOW);
        playerRect.w = separatorPos/4-5;
        playerRect.h = (int) (0.42f*HEIGHT_WINDOW);
        SDL_RenderDrawRect(_renderer, &playerRect);
    }
    for ( int i = 0; i < num_jogadores; i++) //correr os jogadores todos da lista de jogadores
    {
        SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255 );
        playerRect.x = (head_jogador->player.table_pos)*(separatorPos/4-5)+10;
        playerRect.y = (int) (0.55f*HEIGHT_WINDOW);                                   //escreve a branco, o nome e dinheiro que o jogador tem na
        playerRect.w = separatorPos/4-5;                                             // mesa apostado e o seu quadrado de jogo
        playerRect.h = (int) (0.42f*HEIGHT_WINDOW);
        sprintf(name_money_str,"%s -- %d$  na mesa", head_jogador->player.nome, head_jogador->player.apostado);
        RenderText(playerRect.x+20, playerRect.y-30, name_money_str, _font, &white, _renderer);
        SDL_RenderDrawRect(_renderer, &playerRect);

        if (head_jogador->player.bj == 1)// escreve BLACKJACKa preto e mete quadrado de jogo do jogador a preto
        {
            SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255 );
            playerRect.x = (head_jogador->player.table_pos)*(separatorPos/4-5)+10;
            playerRect.y = (int) (0.55f*HEIGHT_WINDOW);
            playerRect.w = separatorPos/4-5;
            playerRect.h = (int) (0.42f*HEIGHT_WINDOW);
            sprintf(name_money_str,"BLACK JACK");
            RenderText(playerRect.x+20, playerRect.y-50, name_money_str, _font, &black, _renderer);
            SDL_RenderDrawRect(_renderer, &playerRect);
        }
        else
        {
            if (head_jogador->player.bust == 1) //se nao fizerblackjakj ou escreve BUST a vermelho por cima do jogador ou....
            {
                SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255 );
                sprintf(name_money_str,"BUST");
                RenderText(playerRect.x+20, playerRect.y-50, name_money_str, _font, &red, _renderer);
            }
            else
            {
                if(head_jogador->player.desistiu != 1)
                {
                    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255 ); //..... esvreve a pontuacao do jogador a verde
                    sprintf(name_money_str,"%d pontos", head_jogador->player.pontuacao);
                    RenderText(playerRect.x+20, playerRect.y-50, name_money_str, _font, &pontuacao, _renderer);
                }
                if (jogador_actual == head_jogador) //o quadrado de jogo do jogador que está a falar fica delimitado a vermelho
                {
                    SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255 );
                    playerRect.x = (head_jogador->player.table_pos)*(separatorPos/4-5)+10;
                    playerRect.y = (int) (0.55f*HEIGHT_WINDOW);
                    playerRect.w = separatorPos/4-5;
                    playerRect.h = (int) (0.42f*HEIGHT_WINDOW);
                    SDL_RenderDrawRect(_renderer, &playerRect);
                }
            }
        }
        RenderText(separatorPos+1*MARGIN, height, head_jogador->player.nome, _font, &black, _renderer);   // escreve no separa dor do lado escerdo o nome
        RenderText(separatorPos+13*MARGIN, height, head_jogador->player.tipo, _font, &black, _renderer);  // de cada jogador da lista de jogadores, o tipo
        sprintf(name_money_str,"%.1f euros", head_jogador->player.dinheiro);                              // de jogador e o dinheiro que cada jogador tem
        RenderText(separatorPos+5*MARGIN, height + 23, name_money_str, _font, &black, _renderer);         // sempre actualizado

        head_jogador = head_jogador->next;
        height+=70; // para escrever os dados do proximo jogador mais a baixo à medida que passa de jogador
    }

    // destroy everything
    SDL_DestroyTexture(table_texture);
}

/**
 * RenderHouseCards: Renders cards of the house
 * \param _house vector with the house cards
 * \param _pos_house_hand position of the vector _house with valid card IDs
 * \param _cards vector with all loaded card images
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderHouseCards(SDL_Surface **_cards, SDL_Renderer* _renderer, _mesa mesa)
{
    int card, x, y, a;
    int div = WIDTH_WINDOW/CARD_WIDTH;
    baralho *aux;

    // drawing all house cards
    inv_pilha(&(mesa.carta_mesa)); //inverter a pilha de cartas para as cartas serem desenhadas pela ordem que foram recebidas
    aux = mesa.carta_mesa; //auxiliar que corre a pilha de cartas quando está invertida, para ser desenhada
    for ( card = 0; card < mesa.num_cartas; card++)
    {
        a = (aux->carta.valor-1) + 13*(aux->carta.naipe-1); // vai dar sempre a número da carta do baralho de 0 a 51
        // calculate its position
        x = (div/2-(mesa.num_cartas)/2+card)*CARD_WIDTH + 15;
        y = (int) (0.26f*HEIGHT_WINDOW);
        // render it !
        RenderCard(x, y, a, _cards, _renderer);
        aux = aux->next;
    }
    inv_pilha(&(mesa.carta_mesa)); //desinverter a pilha para voltar a ficar o formato normal de pilha (fica igual ao que estava antes de ser invertida)
    // just one card ?: draw a card face down
    if (mesa.num_cartas == 1) //para o caso da mesa só ter uma carta à mostra
    {
        x = (div/2-(mesa.num_cartas)/2+1)*CARD_WIDTH + 15;
        y = (int) (0.26f*HEIGHT_WINDOW);
        RenderCard(x, y, 52, _cards, _renderer);
    }
}

/**
 * RenderPlayerCards: Renders the hand, i.e. the cards, for each player
 * \param _player_cards 2D array with the player cards, 1st dimension is the player ID
 * \param _pos_player_hand array with the positions of the valid card IDs for each player
 * \param _cards vector with all loaded card images
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderPlayerCards(SDL_Surface **_cards, SDL_Renderer* _renderer, int num_jogadores, jogador *head_jogador)
{
    int pos, x, y, num_player, card, a;
    // for every card of every player
    baralho *aux=NULL;

    for ( num_player = 0; num_player < num_jogadores; num_player++) //desenhar as cartas de todos os jogadores
    {
        inv_pilha(&head_jogador->player.carta_jogador); //inverter a pilha de cartas para as cartas serem desenhadas pela ordem que foram recebidas
        aux = head_jogador->player.carta_jogador; // auxiliar que corre a pilha de cartas quando está invertida, para ser desenhada

        for ( card = 0 ; card < head_jogador->player.num_cartas; card++)
        {
            a = (aux->carta.valor-1) + 13*(aux->carta.naipe-1); // vai dar sempre a número da carta do baralho de 0 a 51
            // draw all cards of the player: calculate its position: only 4 positions are available !
            pos = card % 4;
            x = (int) (head_jogador->player.table_pos)*((0.95f*WIDTH_WINDOW)/4-5)+(card/4)*12+15;
            y = (int) (0.55f*HEIGHT_WINDOW)+10;
            if ( pos == 1 || pos == 3) x += CARD_WIDTH + 30;
            if ( pos == 2 || pos == 3) y += CARD_HEIGHT+ 10;
            // render it !
            RenderCard(x, y, a, _cards, _renderer);
            aux = aux->next;
        }                                                       //desinverter a pilha para voltar a ficar o formato normal de pilha
        inv_pilha(&head_jogador->player.carta_jogador);         //(fica igual ao que estava antes de ser invertida)
        head_jogador = head_jogador->next;
    }
}

/**
 * RenderCard: Draws one card at a certain position of the window, based on the card code
 * \param _x X coordinate of the card position in the window
 * \param _y Y coordinate of the card position in the window
 * \param _num_card card code that identifies each card
 * \param _cards vector with all loaded card images
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderCard(int _x, int _y, int _num_card, SDL_Surface **_cards, SDL_Renderer* _renderer)
{
    SDL_Texture *card_text;
    SDL_Rect boardPos;

    // area that will be occupied by each card
    boardPos.x = _x;
    boardPos.y = _y;
    boardPos.w = CARD_WIDTH;
    boardPos.h = CARD_HEIGHT;

    // render it !
    card_text = SDL_CreateTextureFromSurface(_renderer, _cards[_num_card]);
    SDL_RenderCopy(_renderer, card_text, NULL, &boardPos);

    // destroy everything
    SDL_DestroyTexture(card_text);
}

/**
 * LoadCards: Loads all images of the cards
 * \param _cards vector with all loaded card images
 */
void LoadCards(SDL_Surface **_cards)
{
    int i;
    char filename[STRING_SIZE];

    // loads all cards to an array
    for (i = 0 ; i < MAX_DECK_SIZE; i++ )
    {
        // create the filename !
        sprintf(filename, ".//cartas//carta_%02d.png", i+1);
        // loads the image !
        _cards[i] = IMG_Load(filename);
        // check for errors: deleted files ?
        if (_cards[i] == NULL)
        {
            printf("Unable to load image: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }
    // loads the card back
    _cards[i] = IMG_Load(".//cartas//carta_back.jpg");
    if (_cards[i] == NULL)
    {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}


/**
 * UnLoadCards: unloads all card images of the memory
 * \param _cards vector with all loaded card images
 */
void UnLoadCards(SDL_Surface **_array_of_cards)
{
    // unload all cards of the memory: +1 for the card back
    for (int i = 0 ; i < MAX_DECK_SIZE + 1; i++ )
    {
        SDL_FreeSurface(_array_of_cards[i]);
    }
}

/**
 * RenderLogo function: Renders the IST Logo on the window screen
 * \param x X coordinate of the Logo
 * \param y Y coordinate of the Logo
 * \param _logoIST surface with the IST logo image to render
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderLogo(int x, int y, SDL_Surface *_logoIST, SDL_Renderer* _renderer)
{
    SDL_Texture *text_IST;
    SDL_Rect boardPos;

    // space occupied by the logo
    boardPos.x = x;
    boardPos.y = y;
    boardPos.w = _logoIST->w;
    boardPos.h = _logoIST->h;

    // render it
    text_IST = SDL_CreateTextureFromSurface(_renderer, _logoIST);
    SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

    // destroy associated texture !
    SDL_DestroyTexture(text_IST);
    return _logoIST->h;
}

/**
 * RenderText function: Renders the IST Logo on the window screen
 * \param x X coordinate of the text
 * \param y Y coordinate of the text
 * \param text string where the text is written
 * \param font TTF font used to render the text
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderText(int x, int y, const char *text, TTF_Font *_font, SDL_Color *_color, SDL_Renderer* _renderer)
{
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect solidRect;

    solidRect.x = x;
    solidRect.y = y;
    // create a surface from the string text with a predefined font
    text_surface = TTF_RenderText_Blended(_font,text,*_color);
    if(!text_surface)
    {
        printf("TTF_RenderText_Blended: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    // create texture
    text_texture = SDL_CreateTextureFromSurface(_renderer, text_surface);
    // obtain size
    SDL_QueryTexture( text_texture, NULL, NULL, &solidRect.w, &solidRect.h );
    // render it !
    SDL_RenderCopy(_renderer, text_texture, NULL, &solidRect);

    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
    return solidRect.h;
}

/**
 * InitEverything: Initializes the SDL2 library and all graphical components: font, window, renderer
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _img surface to be created with the table background and IST logo
 * \param _window represents the window of the application
 * \param _renderer renderer to handle all rendering in a window
 */
void InitEverything(int width, int height, TTF_Font **_font, SDL_Surface *_img[], SDL_Window** _window, SDL_Renderer** _renderer)
{
    InitSDL();
    InitFont();
    *_window = CreateWindow(width, height);
    *_renderer = CreateRenderer(width, height, *_window);

    // load the table texture
    _img[0] = IMG_Load("table_texture.png");
    if (_img[0] == NULL)
    {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // load IST logo
    _img[1] = SDL_LoadBMP("ist_logo.bmp");
    if (_img[1] == NULL)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // this opens (loads) a font file and sets a size
    *_font = TTF_OpenFont("FreeSerif.ttf", 16);
    if(!*_font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitSDL: Initializes the SDL2 graphic library
 */
void InitSDL()
{
    // init SDL library
    if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        printf(" Failed to initialize SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitFont: Initializes the SDL2_ttf font library
 */
void InitFont()
{
    // Init font library
    if(TTF_Init()==-1)
    {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * CreateWindow: Creates a window for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \return pointer to the window created
 */
SDL_Window* CreateWindow(int width, int height)
{
    SDL_Window *window;
    // init window
    window = SDL_CreateWindow( "BlackJack", WINDOW_POSX, WINDOW_POSY, width+EXTRASPACE, height, 0 );
    // check for error !
    if ( window == NULL )
    {
        printf("Failed to create window : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return window;
}

/**
 * CreateRenderer: Creates a renderer for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _window represents the window for which the renderer is associated
 * \return pointer to the renderer created
 */
SDL_Renderer* CreateRenderer(int width, int height, SDL_Window *_window)
{
    SDL_Renderer *renderer;
    // init renderer
    renderer = SDL_CreateRenderer( _window, -1, 0 );

    if ( renderer == NULL )
    {
        printf("Failed to create renderer : %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // set size of renderer to the same as window
    SDL_RenderSetLogicalSize( renderer, width+EXTRASPACE, height );

    return renderer;
}
