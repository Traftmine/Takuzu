#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "game.h"
#include "game_aux.h"

/* Fonction assert */
#define ASSERT(expr)                                                                  \
  do                                                                                  \
  {                                                                                   \
    if ((expr) == 0)                                                                  \
    {                                                                                 \
      fprintf(stderr, "[%s:%d] Assertion '%s' failed!\n", __FILE__, __LINE__, #expr); \
      abort();                                                                        \
    }                                                                                 \
  } while (0)

int test_dummy(void){
    return EXIT_SUCCESS;
}

void usage(int argc, char* argv[]){
    fprintf(stderr, "You need to give one argument");
    exit(EXIT_FAILURE);
}

bool test_game_play_move(void){
    game g = game_new_empty();
    game_play_move(g,0,0,S_ZERO);
    game_play_move(g,0,1,S_ONE);
    ASSERT(game_get_square(g,0,0)==S_ZERO);
    ASSERT(game_get_square(g,0,1)==S_ONE);
    return true;
}
bool test_game_has_error(void){
    game g = game_new_empty();
    game_set_square(g,0,0,S_ZERO);
    game_set_square(g,0,1,S_ZERO);
    game_set_square(g,0,2,S_ZERO);
    ASSERT(game_has_error(g,0,2)!=0 && game_has_error(g,0,2)!=false);
    game_set_square(g,0,2,S_ONE);
    ASSERT(game_has_error(g,0,2)==0);
    g = game_default_solution();
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            ASSERT(game_has_error(g,i,j)==0);
        }
    }
    game_set_square(g,1,3,S_ONE);
    ASSERT(game_has_error(g,1,4)!=0 && game_has_error(g,1,4)!=false);
    ASSERT(game_has_error(g,1,3)!=0 && game_has_error(g,1,3)!=false);
    ASSERT(game_has_error(g,1,2)!=0 && game_has_error(g,1,2)!=false);
    ASSERT(game_has_error(g,1,1)!=0 && game_has_error(g,1,1)!=false);
    g = game_default();
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            ASSERT(game_has_error(g,i,j)==0);
        }
    }
    g = game_default();
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            game_set_square(g,i,j,S_ZERO);
        }
    }
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            ASSERT(game_has_error(g,i,j)!=false && game_has_error(g,i,j)!=0);
        }
    }
    game_delete(g);
    return true;
}

bool test_game_restart(void){
    game g = game_default();
    game g_copy = game_copy(g);
    game_play_move(g,1,1,S_ZERO);
    game_restart(g);
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            square s = game_get_square(g,i,j);
            square s_copy = game_get_square(g_copy,i,j);
            ASSERT(s == s_copy);
        }

    }
    game_delete(g);
    game_delete(g_copy);
    return true;
}

bool test_game_is_over(void){
    square s_one[6][6];
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            s_one[i][j] = S_ONE;
        }
    }
    square s_zero[6][6];
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            s_zero[i][j] = S_ZERO;
        }
    }

    game g_one = game_new(s_one);
    game g_zero = game_new(s_zero);
    ASSERT(!game_is_over(g_one));
    ASSERT(!game_is_over(g_zero));
    game_delete(g_one);
    game_delete(g_zero);
    game g_solution = game_default_solution();
    ASSERT(game_is_over(g_solution));
    game_play_move(g_solution, 1, 1, S_ZERO);
    ASSERT(!game_is_over(g_solution));
    game_set_square(g_solution, 1, 1, S_EMPTY);
    game_set_square(g_solution, 5, 1, S_ONE);
    ASSERT(!game_is_over(g_solution));
    g_solution = game_default_solution();
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
           if (i<3)
           {
               if (j<3)
                {
                    game_set_square(g_solution,i,j,S_ZERO);
                }
                else
                {
                    game_set_square(g_solution,i,j,S_ONE);
                }
           }
           else
           {
                if (j<3)
                {
                    game_set_square(g_solution,i,j,S_ONE);
                }
                else
                {
                    game_set_square(g_solution,i,j,S_ZERO);
                }
           }
        }
    }
    ASSERT(!game_is_over(g_solution));
    game_delete(g_solution);

    return true;
}

bool test_game_default_solution(void){
    game g = game_default();
    if(g==NULL){
        return false;
    }
    g = game_default_solution();
    for(int i=0; i<DEFAULT_SIZE; i++){
        for(int j=0; j<DEFAULT_SIZE; j++){
            if(game_has_error(g,i,j)==true){
                return false;
            }
        }
    }
    if(game_is_over(g)!=true){
        return false;
    }
    game_delete(g);
    return true;
}

bool test_game_check_move(void){ 
    game g = game_new_empty();
    ASSERT(game_check_move(g,0,0,S_IMMUTABLE_ZERO)==false);
    ASSERT(game_check_move(g,0,1,S_IMMUTABLE_ONE)==false);
    ASSERT(game_check_move(g,0,7,S_ONE)==false);
    game_set_square(g,0,0,S_ONE);
    game_set_square(g,0,1,S_IMMUTABLE_ONE);
    ASSERT(game_check_move(g,0,1,S_ONE)==false);
    ASSERT(game_check_move(g,0,0,S_ZERO)==true);
    game_delete(g);
    return true;
}


int main(int argc, char* argv[]){
    if(argc == 1){
        usage(argc, argv);
    }
    if(strcmp("dummy", argv[1]) == 0){
        test_dummy();
    }
    if(strcmp("play_move", argv[1]) == 0){
        test_game_play_move();
    }
    if(strcmp("game_restart", argv[1]) == 0){
        test_game_restart();
    }
    if(strcmp("game_is_over", argv[1])==0){
        test_game_is_over();
    }
    if(strcmp("game_has_error", argv[1])==0){
        test_game_has_error();
    }
    if(strcmp("game_default_solution", argv[1])==0){
        test_game_default_solution();
    }
    if(strcmp("game_check_move", argv[1])==0){
        test_game_check_move();
    }

}
