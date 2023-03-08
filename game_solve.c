#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "game.h"
#include "game_tools.h"

int main(int argc, char* argv[]) {
    if(argc > 4 || argc < 3){
        printf("Error: please respect this following syntax: $ ./game_solve <option> <input> [<output>] \n");
        printf("The output is optional\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp("-c", argv[1]) != 0 && strcmp("-s", argv[1]) != 0 ){
        printf("Error: second argument <option> should be '-c' if you wish to know the number of existing soltuions to your game \n");
        printf( "OR '-s' if you wish to search and save the first solution found to your game in your file given as a 4th argument \n");
        exit(EXIT_FAILURE);
    }

    game g = game_load(argv[2]);

    if ( strcmp("-s", argv[1]) == 0 ){
        if (game_solve(g)){
            char * file_name;
            if(argc == 4){
                file_name =  argv[3];
            }
            else{
                file_name = "stderr";
            }
            game_save(g,file_name);
        }
        else {
            return EXIT_FAILURE;
        }
    }
    else if (strcmp("-c", argv[1]) == 0){
        
        if(argc == 4){
            FILE* file_name = fopen(argv[3], "w");
            fprintf(file_name,"The number of soltutions found to your game is: %u",game_nb_solutions(g));

        }
        else{
            fprintf(stderr,"The number of soltutions found to your game is: %u",game_nb_solutions(g));
        }
        
    }
    return EXIT_SUCCESS;
}