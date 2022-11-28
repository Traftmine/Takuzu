#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game.h"
#include "game_aux.h"

void game_print(cgame g){
    printf("   ");
    for(uint i = 0; i< DEFAULT_SIZE ; i++){ // print of 012345
        printf("%u",i);
    }
    printf("\n");

    printf("   ");
    for(uint i = 0; i< DEFAULT_SIZE ; i++){
        printf("-");
    }
    printf("\n");

    for(uint x = 0; x< DEFAULT_SIZE ; x++){
        printf("x |");
        for(uint y = 0; y<DEFAULT_SIZE; y++){
            if(game_get_square(g,x,y) == S_EMPTY){
                printf(" ");
            }
            else{
                if(game_get_square(g,x,y) == S_ONE){
                    printf("b");
                }
                else if(game_get_square(g,x,y) == S_ZERO){
                    printf("w");
                }
                else if(game_get_square(g,x,y) == S_IMMUTABLE_ONE){
                    printf("B");
                }
                else if(game_get_square(g,x,y) == S_IMMUTABLE_ZERO){
                    printf("W");
                }
            }
        }
        printf("|\n");
    }
    printf("   ");
    for(uint i = 0; i< DEFAULT_SIZE ; i++){
        printf("-");
    }
    printf("\n");
}

game game_default(void){
    game g;
    for (int i = 0; i < DEFAULT_SIZE; i++){
        for (int j = 0; j < DEFAULT_SIZE; j++){
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
    return g;
}

game game_default_solution(void){
    game g;
    g = game_default();
    game_set_square(g,0,0,S_ZERO);
    game_set_square(g,0,3,S_ONE);
    game_set_square(g,0,4,S_ZERO);
    game_set_square(g,0,5,S_ONE);
    game_set_square(g,1,0,S_ZERO);
    game_set_square(g,1,1,S_ONE);
    game_set_square(g,1,2,S_ONE);
    game_set_square(g,1,3,S_ZERO);
    game_set_square(g,1,4,S_ONE);
    game_set_square(g,1,5,S_ZERO);
    game_set_square(g,2,0,S_ONE);
    game_set_square(g,2,2,S_ZERO);
    game_set_square(g,2,3,S_ONE);
    game_set_square(g,2,5,S_ONE);
    game_set_square(g,3,0,S_ONE);
    game_set_square(g,3,3,S_ZERO);
    game_set_square(g,3,4,S_ZERO);
    game_set_square(g,3,5,S_ONE);
    game_set_square(g,4,0,S_ZERO);
    game_set_square(g,4,1,S_ONE);
    game_set_square(g,4,3,S_ZERO);
    game_set_square(g,4,4,S_ONE);
    game_set_square(g,5,0,S_ONE);
    game_set_square(g,5,1,S_ZERO);
    game_set_square(g,5,2,S_ZERO);
    game_set_square(g,5,3,S_ONE);
    game_set_square(g,5,4,S_ONE);
    return g;
}