#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "common.h"

#define BL 5 // backlog pour listen
#define N 5000 // length pour send, XMAX * YMAX
#define NB_J 2 // nombre de joueurs
#define ES 0 // entrée standard
#define STR_LEN 20 // pour le commandes serveur

// cette structure n'est utilisé que pour le programme du serveur
// pour la gestion des informations sur la connexion des clients
// elle ne servira pas pour l'envoi de données 
typedef struct client{
    SAI cl;
    int nsocket;
    in_addr_t addr;
    int nport;
    unsigned int id_client;
} strclient;

typedef struct lightcycle{
    int X;
    int Y;
    int mur_lumiere;
    unsigned int direction;
}lightcycle;

void init_board(display_info * info){
    info->winner=-1;
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
                    info->board[j][i] = LIGHTCYCLE_INDEX_SHIFT +1;
                }
                
            } 
        }
    }
}

void init_lightcycle(lightcycle * lc1, lightcycle * lc2){
    lc1->X = 1;
    lc1->Y = YMAX/2;
    lc1->direction = RIGHT;
    lc1->mur_lumiere = 1;
    lc2->X = XMAX - 2;
    lc2->Y = YMAX/2;
    lc2->direction = LEFT;
    lc2->mur_lumiere = 1;
}

void collision_mur_jeu(display_info* info, lightcycle * lc, int id_joueur){
    if( (info->board[lc->X+1][lc->Y] == WALL && lc->direction == RIGHT)
    || (info->board[lc->X-1][lc->Y] == WALL && lc->direction == LEFT)
    || (info->board[lc->X][lc->Y+1] == WALL && lc->direction == DOWN)
    || (info->board[lc->X][lc->Y-1] == WALL && lc->direction == UP)){
        if(id_joueur == 1){
            info->winner = 2;
        }else if(id_joueur == 2){
            info->winner = 1;
        }
    }
}

void collision_mur_lumiere(display_info* info, lightcycle * lc, int id_joueur){
    if(lc->direction == RIGHT){
        if(info->board[lc->X+1][lc->Y]/10 == TRAIL_INDEX_SHIFT/10){
            if(id_joueur == 1){
                info->winner = 2;
            }else if(id_joueur == 2){
                info->winner = 1;
            }
        }
    }
    if(lc->direction == LEFT){
        if(info->board[lc->X-1][lc->Y]/10 == TRAIL_INDEX_SHIFT/10){
            if(id_joueur == 1){
                info->winner = 2;
            }else if(id_joueur == 2){
                info->winner = 1;
            }
        }
    }
    if(lc->direction == DOWN){
        if(info->board[lc->X][lc->Y+1]/10 == TRAIL_INDEX_SHIFT/10){
            if(id_joueur == 1){
                info->winner = 2;
            }else if(id_joueur == 2){
                info->winner = 1;
            }
        }
    }
    if(lc->direction == UP){
        if(info->board[lc->X][lc->Y-1]/10 == TRAIL_INDEX_SHIFT/10){
            if(id_joueur == 1){
                info->winner = 2;
            }else if(id_joueur == 2){
                info->winner = 1;
            }
        }
    }
}

void collision_lightcycles(display_info* info, lightcycle * lc){
    if(lc->direction == RIGHT){
        if(info->board[lc->X+1][lc->Y]/10 == LIGHTCYCLE_INDEX_SHIFT/10){
            info->winner = TIE;
        }
    }
    if(lc->direction == LEFT){
        if(info->board[lc->X-1][lc->Y] == LIGHTCYCLE_INDEX_SHIFT/10){
            info->winner = TIE;
        }
    }
    if(lc->direction == DOWN){
        if(info->board[lc->X][lc->Y+1] == LIGHTCYCLE_INDEX_SHIFT/10){
            info->winner = TIE;
        }
    }
    if(lc->direction == UP){
        if(info->board[lc->X][lc->Y-1] == LIGHTCYCLE_INDEX_SHIFT/10){
            info->winner = TIE;
        }
    }
}

void lightcycle_move(display_info* info, lightcycle * lc, int id_joueur){
    if(lc->X >0 && lc->X < XMAX-1 && lc->Y > 0 && lc->Y <YMAX -1){

        if(lc->mur_lumiere == 1){
            if(id_joueur == 1){
                info->board[lc->X][lc->Y]=TRAIL_INDEX_SHIFT + id_joueur -1;
            } else if(id_joueur == 2){
                info->board[lc->X][lc->Y]=TRAIL_INDEX_SHIFT + id_joueur -1;
            }
        }else{
            if(id_joueur == 1){
                info->board[lc->X][lc->Y]=' ';// pour effacer la trace
            } else if(id_joueur == 2){
                info->board[lc->X][lc->Y]=' ';
            }
        }
            
        switch (lc->direction){
            case UP:
                lc->Y-=1;
                break;
            case DOWN:
                lc->Y+=1;
                break;
            case LEFT:
                lc->X-=1;
                break;
            case RIGHT:
                lc->X+=1;
                break;
            default:
                break;
        }
        
        collision_lightcycles(info, lc);
        collision_mur_jeu(info, lc, id_joueur);
        collision_mur_lumiere(info, lc, id_joueur);
        
        
        if(info->winner == -1){ // si personne n'a gagné, le lightcycle continue de bouger
            info->board[lc->X][lc->Y]=LIGHTCYCLE_INDEX_SHIFT + id_joueur -1;  
        }

    }
    
}



void change_dir(char dir, lightcycle * lc, int id_client){
    int dir_nouv = -1;
    switch(dir){
        case 'z':
            dir_nouv = UP;
            break;
        case 'q':
            dir_nouv = LEFT;
            break;
        case 's':
            dir_nouv = DOWN;
            break;
        case 'd':
            dir_nouv = RIGHT;
            break;
        case 'i':
            dir_nouv = UP;
            break;
        case 'j':
            dir_nouv = LEFT;
            break;
        case 'k':
            dir_nouv = DOWN;
            break;
        case 'l':
            dir_nouv = RIGHT;
            break;
        default:
            break;   
        
    }
    // verification des directions mutuellement exclusives
    if(dir_nouv != -1){
        if(dir_nouv==UP && lc->direction==DOWN){
        lc->direction=DOWN;
        }
        else if(dir_nouv==DOWN && lc->direction==UP){
            lc->direction=UP;
        }
        else if (dir_nouv==LEFT && lc->direction==RIGHT)
        {
            lc->direction=RIGHT;
        }
        else if (dir_nouv==RIGHT && lc->direction==LEFT)
        {
            lc->direction=LEFT;
        }
        else{
            lc->direction=dir_nouv;
        }
    }
    
}

void mur_lumiere_on_off(lightcycle * lc){
    if(lc->mur_lumiere == 1){
        lc->mur_lumiere = 0;
    } else{
        lc->mur_lumiere = 1;
    }
}

void setup_server(int * sockfd, int port_serveur){
    CHECK((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) != -1);

    SAI my_addr;
    socklen_t len = sizeof(SAI);

    my_addr.sin_addr.s_addr =  htonl(INADDR_ANY); // initialisation infos serveur
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port_serveur);
    int x = 1;
    setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(int));
    CHECK(bind(*sockfd, (SA*) &my_addr, len)!=-1);
   
    CHECK(listen(*sockfd, BL)!=-1); 
    
}

int main(int argc, char **argv){
    if(argc < 2)
    {
      printf("Usage: %s [port_serveur] [refresh_rate] ou %s [port_serveur]\n", argv[0], argv[0]);
      exit(-1);
    } 
   
    int refresh_rate = 100; // par defaut 100 ms
    if(argc == 3){
        refresh_rate = atoi(argv[2]);
    }
    int sockfd;
    int port_serveur = atoi(argv[1]);
    setup_server(&sockfd, port_serveur);

    SAI clients[NB_J];
    socklen_t len = sizeof(SAI);

    strclient tc[NB_J]; // tableau de clients
    struct client_init_infos infos_clients[NB_J];

    //preparation de la structure strclient
    
    unsigned int nombre_clients = 0;

    // initialisation des ensembles de fd
    fd_set readfds;  
    fd_set readfds2;
   
    FD_ZERO(&readfds);
    FD_ZERO(&readfds2);
    FD_SET(sockfd, &readfds);
    unsigned int nb_joueurs = 0;
    unsigned int id_client = 0;
    int fd_max= sockfd;
    FD_ZERO(&readfds2);
    FD_SET(sockfd, &readfds2);
    // connexion des joueurs
    while(nb_joueurs < 2){
       
        readfds2 = readfds;
        
        int ret_select;
        CHECK(ret_select=select(fd_max+1, &readfds2, NULL, NULL, NULL)!=-1);
        if(FD_ISSET(sockfd, &readfds2)){
            int sock_recv_1 = accept(sockfd, (SA*) &clients[nb_joueurs],(socklen_t*) &len);

            tc[nb_joueurs].nsocket = sock_recv_1;
            tc[nb_joueurs].cl = clients[nb_joueurs];
            tc[nb_joueurs].nport = clients[nb_joueurs].sin_port;
            tc[nb_joueurs].addr = clients[nb_joueurs].sin_addr.s_addr;
            tc[nb_joueurs].id_client = ++id_client;
            fd_max = max(tc[nb_joueurs].nsocket, fd_max);
            FD_SET(tc[nb_joueurs].nsocket, &readfds);
            CHECK(recv(tc[nb_joueurs].nsocket, &infos_clients[nb_joueurs].nb_players, sizeof(int), 0)!=-1);

            CHECK(send(tc[nb_joueurs].nsocket, &tc[nb_joueurs].id_client, sizeof(int), 0)!=-1);
            if(infos_clients[nb_joueurs].nb_players == 2){
                nombre_clients = 1;
                nb_joueurs = 2;
            } else if(infos_clients[nb_joueurs].nb_players == 1){
                nombre_clients++;
                nb_joueurs++;
            }
        }
    }
    
    
    display_info plateau_jeu;
    init_board(&plateau_jeu);
    
    int nfds = 0;
    for(int i=0;i<nombre_clients; i++){
        nfds = max(tc[i].nsocket, nfds);
    }
    nfds++;
    readfds2 = readfds;
    
    // l'envoi d'informations pour affichage de 'Ready? Set. Go'
    int go = 1;
    while(go < 4){
        for(int j=0;j<nombre_clients;j++){
            CHECK(send(tc[j].nsocket, &go, sizeof(int), 0)!=-1);
        }
        go++;
    }

    CHECK(select(nfds, &readfds2, 0, 0, 0)!=-1);

    for(int i=0;i<nombre_clients; i++){
        int go_done;
        CHECK(recv(tc[i].nsocket, &go_done, sizeof(int), 0)!=-1); 
     
    }
    // la partie commence apres que les deux joueurs sont presents 
    // et qu'ils ont affiché tous les deux 'Ready? Set. Go'

    // initialisation des lightcycles des joueurs
    lightcycle lc[2];
    init_lightcycle(&lc[0], &lc[1]);

    FD_SET(ES, &readfds);
    readfds2 = readfds;

    
    while(1){
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = refresh_rate * 1000; 

        int before = clock()  * 1000 / CLOCKS_PER_SEC ;
        CHECK(select(nfds, &readfds2, 0, 0, &timeout)!=-1);
        int end = before + refresh_rate;
        do {
            before = clock() * 1000 / CLOCKS_PER_SEC;
        } while (before <= end);

        for(int i=0;i<nombre_clients; i++){
            if(FD_ISSET(tc[i].nsocket, &readfds2)){ // test si fd appartient dans read

                struct client_input cl_input;
                // le serveur recoit la valeur de la touche appuyee par le client i
                CHECK(recv(tc[i].nsocket, &cl_input, sizeof(struct client_input), 0)!=-1); 
                char buf = cl_input.input;
                int cl_id = cl_input.id;

                if(infos_clients[i].nb_players == 1){
                    if(buf == 'z' || buf == 'q' || buf == 's'|| buf == 'd'){
                        change_dir(buf, &lc[cl_id-1], cl_id);
                    } else if(buf==' '){
                        mur_lumiere_on_off(&lc[cl_id-1]);
                    }
                } else{
                    
                    if(buf == 'z' || buf == 'q' || buf == 's'|| buf == 'd'){
                        // c'est le joueur num. 1 qui joue
                        change_dir(buf, &lc[0], cl_id);

                    } else if(buf == 'i' || buf == 'j' || buf == 'k'|| buf == 'l'){
                        // c'est le joueur num. 2 qui joue
                        change_dir(buf, &lc[1], cl_id); 

                    } else if(buf == ' '){
                        // c'est le joueur num. 1 qui joue
                        mur_lumiere_on_off(&lc[0]);

                    } else if(buf == 'm'){
                        // c'est le joueur num. 2 qui joue
                        mur_lumiere_on_off(&lc[1]);
                        
                    }
                }
            }
        }

        if(FD_ISSET(ES, &readfds2)){
            char * entree = malloc(sizeof(char) * STR_LEN);
            scanf("%s", entree);

            if(strcmp(entree, "restart") == 0){
                init_board(&plateau_jeu);
                init_lightcycle(&lc[0], &lc[1]);
            } else if(strcmp(entree, "quit") == 0){ // quitter le jeu
                break;
            }
        }

        // mettre à jour les positions et l'information sur le gagnant
        lightcycle_move(&plateau_jeu, &lc[1], 2);
        lightcycle_move(&plateau_jeu, &lc[0], 1);      

        for(int j=0;j<nombre_clients;j++){

            CHECK(send(tc[j].nsocket, &plateau_jeu, sizeof(display_info), 0)!=-1);
            
        }

        // le jeu se termine après qu'un client a gagné
        // ou si le gagnat est à l'égalité
        if(plateau_jeu.winner != -1){
            break;
        }
        readfds2 = readfds;        
    }
    
    CHECK(close(sockfd)!=-1);
    return 0;
}