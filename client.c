#include <ncurses.h>
#include <termios.h>
#include <time.h>
#include "common.h"
#include <arpa/inet.h>
#include <sys/time.h>

#define XMAX 100
#define YMAX 50
#define NB_COLORS 5
#define TRAIL_INDEX_SHIFT 50

#define BLUE_ON_BLACK       5
#define RED_ON_BLACK        2
#define YELLOW_ON_BLACK     1
#define MAGENTA_ON_BLACK    3
#define CYAN_ON_BLACK       4

#define BLUE_ON_BLUE        50
#define RED_ON_RED          52
#define YELLOW_ON_YELLOW    51
#define MAGENTA_ON_MAGENTA  53
#define CYAN_ON_CYAN        54

#define ES 0 // entrée standard

void tune_terminal()
{
    struct termios term;
    tcgetattr(0, &term);
    term.c_iflag &= ~ICANON;
    term.c_lflag &= ~ICANON;
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &term);
}

void init_graphics()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(100);
    start_color();
    init_pair(BLUE_ON_BLACK, COLOR_BLUE, COLOR_BLACK);
    init_pair(RED_ON_BLACK, COLOR_RED, COLOR_BLACK);
    init_pair(YELLOW_ON_BLACK, COLOR_YELLOW, COLOR_BLACK);
    init_pair(MAGENTA_ON_BLACK, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(CYAN_ON_BLACK, COLOR_CYAN, COLOR_BLACK);

    init_pair(BLUE_ON_BLUE, COLOR_BLUE, COLOR_BLUE);
    init_pair(RED_ON_RED, COLOR_RED, COLOR_RED);
    init_pair(YELLOW_ON_YELLOW, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(MAGENTA_ON_MAGENTA, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(CYAN_ON_CYAN, COLOR_CYAN, COLOR_CYAN);

    init_pair(WALL, COLOR_WHITE, COLOR_WHITE);
}


void display_character(int color, int y, int x, int character) {
    attron(COLOR_PAIR(color));
    mvaddch(y, x, character);
    attroff(COLOR_PAIR(color));
}

void init_board(display_info * info){
    info->winner= -1;
    for (size_t i = 0; i < YMAX; i++) {
        for (size_t j = 0; j < XMAX; j++){
            info->board[j][i] = 32; // espace
        }
    }
    
    for (size_t i = 0; i < YMAX; i++) {
        
        for (size_t j = 0; j < XMAX; j++){

            if (i == 0 || i == YMAX-1) {
                info->board[j][i]=WALL; // murs du jeu
            }
            else if (j == 0 || j == XMAX-1) {
                info->board[j][i]=WALL;
                
            }else if (i == YMAX/2 ){
                if(j == 1){
                    info->board[j][i]=LIGHTCYCLE_INDEX_SHIFT;
                } else if (j == XMAX - 2){
                    info->board[j][i] = LIGHTCYCLE_INDEX_SHIFT+1;
                }
                
            } 
        }
    }

}

void display_board(display_info * info){
    for (size_t i = 0; i < YMAX; i++) {
        for (size_t j = 0; j < XMAX; j++) {

            if (info->board[j][i] == WALL) {

                display_character(WALL, i, j, ACS_VLINE);

            } else if (info->board[j][i] == WALL) {

                display_character(WALL, i, j, ACS_HLINE);

            } else if(info->board[j][i] == LIGHTCYCLE_INDEX_SHIFT){
   
                display_character(BLUE_ON_BLACK, i, j, ACS_DIAMOND);

            } else if(info->board[j][i] == LIGHTCYCLE_INDEX_SHIFT+1){
                
                display_character(YELLOW_ON_BLACK, i, j, ACS_DIAMOND);

            } else if(info->board[j][i]==TRAIL_INDEX_SHIFT){

                display_character(BLUE_ON_BLUE,i,j,'X');

            } else if (info->board[j][i]==TRAIL_INDEX_SHIFT+1){

                display_character(YELLOW_ON_YELLOW,i,j,'Y');
            
            }

        mvaddstr(0, XMAX/2 - strlen("C-TRON")/2, "C-TRON");
        }
    }
}


int main(int argc, char **argv)
{
    if(argc != 4)
    {
      printf("Usage : %s @dest num_port nb_joueurs\n", argv[0]);
      exit(-1);
    }
    if(atoi(argv[3]) > 2){
        printf("Nombre de joueurs illégale\n");
        exit(-1);
    }
    
    int sockfd;
    
    CHECK((sockfd = socket(AF_INET, SOCK_STREAM, 0)) != -1);
    SAI serveur;
    socklen_t fromlen = sizeof(SAI);
    serveur.sin_addr.s_addr =  htonl(atoi(argv[1]));
    serveur.sin_family = AF_INET;
    serveur.sin_port = htons(atoi(argv[2]));
    struct client_init_infos infocl;
    infocl.nb_players = atoi(argv[3]);
    

    CHECK(connect(sockfd, (SA*) &serveur, fromlen)!=-1);
 
    fd_set readfds;    
    fd_set readfds2;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    readfds2 = readfds;

    // l'envoi du nombre de joueurs sur ce client 
    CHECK(send(sockfd, &infocl.nb_players, sizeof(int), 0)!=-1);        

    // le client attend jusqu'a ce que le serveur commence le jeu
    
    int nfds;
    nfds = max(sockfd, ES) + 1;
    FD_SET(sockfd, &readfds);
    FD_SET(ES,  &readfds);
    readfds2 = readfds;
    int my_id;
    CHECK(select(nfds, &readfds2, 0, 0, 0)!=-1);
    if(FD_ISSET(sockfd, &readfds2)){
        CHECK(recv(sockfd, &my_id, sizeof(int), 0)!=-1); 
    }

    tune_terminal();
    init_graphics();
    srand(time(NULL));
    
    display_info plateau_jeu;
    init_board(&plateau_jeu);

    readfds2 = readfds;
    int go = 1;
    while(go < 4){
        CHECK(select(sockfd+1, &readfds2, 0, 0, 0)!=-1);

        if(FD_ISSET(sockfd, &readfds2)){
            CHECK(recv(sockfd, &go, sizeof(int), 0)!=-1); 
        }
        if(go == 1){

            clear();
            mvaddstr(YMAX/2, XMAX/2 - strlen("READY?")/2, "READY?");
            refresh();
            sleep(1);

        } else if(go == 2){

            clear();
            mvaddstr(YMAX/2, XMAX/2 - strlen("SET")/2, "SET");
            refresh();  
            sleep(1);  

        } else if(go == 3){

            clear();
            mvaddstr(YMAX/2, XMAX/2 - strlen("GO")/2, "GO");
            refresh();
            sleep(1);

        }
        go++;
        
    }
    
    int go_done = 1;
    CHECK(send(sockfd, &go_done, sizeof(int), 0)!=-1);

    readfds2 = readfds;
    for (;;) {
        if(plateau_jeu.winner == -1){
            // affichage du plateau du jeu initial
            clear();
            display_board(&plateau_jeu); 
            refresh();

        }
       
        CHECK(select(nfds, &readfds2, 0, 0, 0)!=-1);
        if(FD_ISSET(ES, &readfds2)){
            // l'envoi de l'information sur une touche appuyée
            // sur laquelle le serveur se base dans les calculs
            struct client_input my_input;
            my_input.id = my_id;
            char touche = '\0';
            scanf("%c", &touche);
            my_input.input =touche;
            CHECK(send(sockfd, &my_input, sizeof(struct client_input), 0)!=-1);

        }
        if(FD_ISSET(sockfd, &readfds2)){
            
            CHECK(recv(sockfd, &plateau_jeu, sizeof(display_info), 0)!=-1);
            
        }
        
        readfds2 = readfds;

        // affichage à la fin du jeu
        if(infocl.nb_players == 1){
            if(plateau_jeu.winner == TIE){

                clear();
                mvaddstr(YMAX/2, XMAX/2 - strlen("GAME OVER")/2, "GAME OVER");
                
            } else if(plateau_jeu.winner == my_id){

                clear();
                mvaddstr(YMAX/2, XMAX/2 - strlen("YOU WON")/2, "YOU WON");

            } else if(plateau_jeu.winner != -1 && plateau_jeu.winner != my_id){

                clear();
                mvaddstr(YMAX/2, XMAX/2 - strlen("YOU LOST")/2, "YOU LOST");

            }
        } else if(infocl.nb_players == 2){

            if(plateau_jeu.winner == TIE){

                clear();
                mvaddstr(YMAX/2, XMAX/2 - strlen("GAME OVER")/2, "GAME OVER");

            } else if(plateau_jeu.winner == 1){

                clear();
                mvaddstr(YMAX/2, XMAX/2 - strlen("PLAYER 1 WON")/2, "PLAYER 1 WON");

            } else if(plateau_jeu.winner == 2){

                clear();
                mvaddstr(YMAX/2, XMAX/2 - strlen("PLAYER 2 WON")/2, "PLAYER 2 WON");
                
            } 
        }
        
        refresh();
    }

    CHECK(close(sockfd)!=-1);
    return 0;
}