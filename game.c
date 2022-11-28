#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game_aux.h"
#include "game.h"

struct game_s
{
    square **tab; // j'ai besoin d'avoir un double tableau sinon c'est la merde, si vous avez besoin d'un simple tableau pcq vous trouvez pas avec un double envoyez un msg et je vous aide
};

game game_new(square* squares){
    game g = malloc(sizeof(game));
    if (g == NULL){
        printf("g wasn't allocated");
    }
    g->tab = malloc(DEFAULT_SIZE*DEFAULT_SIZE*sizeof(square));
    if (g->tab == NULL){
        printf("g->tab wasn't allocated");
    }

    return g;
}

game game_new_empty(void){
    game g = malloc(sizeof(game));
    if (g == NULL){
        printf("g wasn't allocated");
    }
    g->tab = malloc((DEFAULT_SIZE*DEFAULT_SIZE)*sizeof(square));
    if (g->tab == NULL){
        printf("g->tab wasn't allocated");
    }
    for (int i = 0; i < DEFAULT_SIZE; i++){
        for (int j = 0; j < DEFAULT_SIZE; j++){
             g->tab[i][j]=S_EMPTY;
        }
    }
    return g;
}

game game_copy(cgame g)
{
    game copied_game = game_new_empty();
    for(uint x = 0; x< DEFAULT_SIZE ; x++){
        for(uint y = 0; y<DEFAULT_SIZE; y++){
            game_set_square(copied_game,x,y,game_get_square(g,x,y));
        }
    }
    return copied_game;
}

bool game_equal(cgame g1, cgame g2)
{
    for(uint x = 0; x< DEFAULT_SIZE ; x++){
        for(uint y = 0; y<DEFAULT_SIZE; y++){
            if (game_get_square(g1,x,y) != game_get_square(g2,x,y) ){
                return false;
            }
        }
    }
    return true;
}
void game_delete(game g){
    if(g != NULL){
        if(g->tab != NULL){
            g->tab= NULL; 
        }
        free(g->tab);
    }
    free(g);
}

void game_set_square(game g, uint i, uint j, square s){
    if( g == NULL ){
        fprintf(stderr,"Error: invalid pointer to game in game_set_square!\n");
        exit(EXIT_FAILURE);
    }

    if ( i >= DEFAULT_SIZE){
        fprintf(stderr,"Error: i value is greater or equal to DEFAULT_SIZE in game_set_square! \n");
        exit(EXIT_FAILURE);
    }
    if ( j >= DEFAULT_SIZE){
        fprintf(stderr,"Error: j value is greater or equal to DEFAULT_SIZE in game_set_square! \n");
        exit(EXIT_FAILURE);
    }
    if(s !=  S_EMPTY || s != S_ZERO || s != S_ONE || s != S_IMMUTABLE_ZERO || s != S_IMMUTABLE_ONE){
        fprintf(stderr,"Error: invalid sqaure value in game_set_square! \n");
        exit(EXIT_FAILURE);
    }

    g->tab[i][j]= s;

}

square game_get_square(cgame g, uint i, uint j)
{
    if (g != NULL && i < DEFAULT_SIZE && i >= 0 && j >= 0 && j < DEFAULT_SIZE){
        square s = g->tab[i][j];
        return s;
    }
    printf("Wrong parameters");
    return EXIT_FAILURE;
}
int game_get_number(cgame g, uint i, uint j)
{
    if (g != NULL && i < DEFAULT_SIZE && i >= 0 && j >= 0 && j < DEFAULT_SIZE){
        if (game_get_square(g,i,j) == S_IMMUTABLE_ONE || game_get_square(g,i,j) == S_ONE){
            return 1;
        }
        else if (game_get_square(g,i,j) == S_IMMUTABLE_ZERO || game_get_square(g,i,j) == S_ZERO){
            return 0;
        }
        else{
            return -1;
        }
    }
    printf("Wrong parameters");
    return EXIT_FAILURE;
}
int game_get_next_square(cgame g, uint i, uint j, direction dir, uint dist)
{
    if (g != NULL && i < DEFAULT_SIZE && i >= 0 && j >= 0 && j < DEFAULT_SIZE && dist <= 2){
        if (dir == UP){
            return game_get_square(g,i,j-dist);
        }
        else if (dir == DOWN){
            return game_get_square(g,i,j+dist);
        }
        else if (dir == LEFT){
            return game_get_square(g,i-dist,j);
        }
        else if (dir == RIGHT){
            return game_get_square(g,i+dist,j);
        }
        else{
            printf("Wrong parameters");
            return EXIT_FAILURE;
        }
    }
    printf("Wrong parameters");
    return EXIT_FAILURE;
}
int game_get_next_number(cgame g, uint i, uint j, direction dir, uint dist)
{
    if (g != NULL && i < DEFAULT_SIZE && i >= 0 && j >= 0 && j < DEFAULT_SIZE && dist <= 2){
        if (dir == UP){
            return game_get_number(g,i,j+dist);
        }
        else if (dir == DOWN){
            return game_get_number(g,i,j-dist);
        }
        else if (dir == LEFT){
            return game_get_number(g,i-dist,j);
        }
        else if (dir == RIGHT){
            return game_get_number(g,i+dist,j);
        }
        else{
            printf("Wrong parameters");
            return EXIT_FAILURE;
        }
    }
    printf("Wrong parameters");
    return EXIT_FAILURE;
}
bool game_is_empty(cgame g, uint i, uint j)
{
    if (g != NULL && i < DEFAULT_SIZE && i >= 0 && j >= 0 && j < DEFAULT_SIZE)
    {
        return game_get_square(g,i,j) == S_EMPTY;
    }
    printf("Wrong parameters");
    return false;
}

bool game_is_immutable(cgame g, uint i, uint j)
{
    if (g != NULL && i < DEFAULT_SIZE && i >= 0 && j >= 0 && j < DEFAULT_SIZE)
    {
        if (game_get_square(g,i,j) == S_IMMUTABLE_ZERO || game_get_square(g,i,j) == S_IMMUTABLE_ONE){
        return true;
        }
    }
    return false;
}
int game_has_error(cgame g, uint i, uint j)
{
    
    if(g==NULL){
        exit(EXIT_FAILURE);
    }
    if(j-1>=0 && j+1<6){
        if(game_get_square(g,i,j) == game_get_square(g,i,j-1) && game_get_square(g,i,j) == game_get_square(g,i,j+1)){
            exit(EXIT_FAILURE);
        }
    }
    if(j+1<6 && j+2<6){
        if(game_get_square(g,i,j) == game_get_square(g,i,j+1) && game_get_square(g,i,j) == game_get_square(g,i,j+2)){
            exit(EXIT_FAILURE);
        }
    }
    if(j-1>=0 && j-2>=0){
       if(game_get_square(g,i,j) == game_get_square(g,i,j-1) && game_get_square(g,i,j) == game_get_square(g,i,j-2)){
            exit(EXIT_FAILURE);
        } 
    }
    if(i-1>=0 && i+1<6){
        if(game_get_square(g,i,j) == game_get_square(g,i-1,j) && game_get_square(g,i,j) == game_get_square(g,i+1,j)){
            exit(EXIT_FAILURE);
        }
    }
    if(i+1<6 && i+2<6){
       if(game_get_square(g,i,j) == game_get_square(g,i+1,j) && game_get_square(g,i,j) == game_get_square(g,i+2,j)){
            exit(EXIT_FAILURE);
        } 
    }
    if(i-1>=0 && i-2>=0){
        if(game_get_square(g,i,j) == game_get_square(g,i-1,j) && game_get_square(g,i,j) == game_get_square(g,i-2,j)){
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

bool game_check_move(cgame g, uint i, uint j, square s)
{
    if(g==NULL){
        exit(EXIT_FAILURE);
    }
    if(i>DEFAULT_SIZE || j>DEFAULT_SIZE){
        return false;
    }
    if(i<0 || j<0){
        return false;
    }
    if(s!=S_EMPTY){
        return false;
    }
    if(s!=S_ONE){
        return false;
    }
    if(s!=S_ZERO){
        return false;
    }
    if(game_get_square(g,i,j)==S_IMMUTABLE_ONE || game_get_square(g,i,j)==S_IMMUTABLE_ZERO){
        return false;
    }
    return true;
}
void game_play_move(game g, uint i, uint j, square s)
{
    if(g==NULL){
        exit(EXIT_FAILURE);
    }
    if(i>DEFAULT_SIZE || j>DEFAULT_SIZE){
        return;
    }
    if(s==S_IMMUTABLE_ONE || s==S_IMMUTABLE_ZERO){
        return;
    }
    if(game_get_square(g,i,j)==(S_IMMUTABLE_ONE) ||game_get_square(g,i,j)==(S_IMMUTABLE_ZERO)){
        return;
    }else{
        game_set_square(g,i,j,s);

    }
}

bool game_is_over(cgame g)
{
    if(g==NULL){
        exit(EXIT_FAILURE);
    }
    int compteur_zero_ligne, compteur_un_ligne;
    int compteur_zero_colonne, compteur_un_colonne;
    for(int i=0; i<DEFAULT_SIZE; i++){
        for(int j=0; j<DEFAULT_SIZE; i++){
            if(game_has_error(g,i,j)!=0){
                return false;
            }
            if(game_get_square(g,i,j)==S_EMPTY){
                return false;
            }
            /*verifie si il y a le meme nombre de 0 et de 1 dans le tableau*/
            if(game_get_number(g,i,j)==0){
                compteur_zero_ligne+=1;
            }
            if(game_get_number(g,j,i)==0){
                compteur_zero_colonne+=1;
            }
            if(game_get_number(g,i,j)==1){
                compteur_un_ligne+=1;
            }
            if(game_get_number(g,j,i)==1){
                compteur_un_colonne+=1;
            }
        }
        if(compteur_un_ligne != compteur_zero_ligne){
            return false;
        }
        if(compteur_un_colonne != compteur_zero_colonne){
            return false;
        }
    }

    return true;
}
void game_restart(game g)
{
    if(g==NULL){
        exit(EXIT_FAILURE);
    }
    for(int i=0; i<DEFAULT_SIZE; i++){
        for(int j=0; j<DEFAULT_SIZE; j++){
            game_set_square(g,i,j,S_EMPTY);
        }
    }
    game_set_square(g,0,1,S_IMMUTABLE_ONE);
    game_set_square(g,0,2,S_IMMUTABLE_ZERO);
    game_set_square(g,2,1,S_IMMUTABLE_ZERO);
    game_set_square(g,2,4,S_IMMUTABLE_ZERO);
    game_set_square(g,3,1,S_IMMUTABLE_ZERO);
    game_set_square(g,3,2,S_IMMUTABLE_ONE);
    game_set_square(g,4,2,S_IMMUTABLE_ONE);
    game_set_square(g,4,5,S_IMMUTABLE_ZERO);
    game_set_square(g,5,5,S_IMMUTABLE_ZERO);
}