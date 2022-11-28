#include <stdlib.h>
#include <stdio.h>
#include "game_aux.h"
#include "game.h"

int main(void){
    //Creating the game
    game g = game_default();
    game_print(g);
    while (game_is_over(g)!=true)
    {
        //Printing the error's list
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                if (game_has_error(g,i,j)!=0)
                {
                    printf("Il y a une erreur en (%d,%d)\n",i,j);
                }
            }

        }
        //Reading player's will
        char c;
        scanf("%c",&c);

        if (c=='h')
        {
            printf("Vous devez jouer des coups dans des cases de coordonées (i,j) ; vous avez le choix entre b (black), w (white) ou e (vide)\n Lorsque vous écrivez vous devez indiquer w, b ou e puis les coordonées ainsi 'w 1 2' par exemple\n");
        }
        else if (c=='r')
        {
            game_restart(g);
            game_print(g);
        }
        else if (c=='q')
        {
            printf("shame\n");
            game_delete(g);
            return EXIT_SUCCESS;
        }
        else if (c == 'b'|| c == 'w'|| c == 'e')
        {
            //Actually playing
            int i = 0,j = 0;
            scanf("%d %d",&i,&j);

            if (c=='w')
            {
                if (game_check_move(g,i,j,S_ZERO))
                {
                    game_play_move(g,i,j,S_ZERO);
                    game_print(g);
                }
            }
            else if (c=='b')
            {
                if (game_check_move(g,i,j,S_ONE))
                {
                    game_play_move(g,i,j,S_ONE);
                    game_print(g);
                }
            }
            else if (c=='e')
            {
                if (game_check_move(g,i,j,S_EMPTY))
                {
                    game_play_move(g,i,j,S_EMPTY);
                    game_print(g);
                }
            }
            printf("Your move is impossible, try a new one\n\n");
            game_print(g);
        }
    }
    printf("congratulation\n");
    game_print(g);
    return EXIT_SUCCESS;
}