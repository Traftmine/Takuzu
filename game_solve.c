#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("Error: please respect this following syntax: $ ./game_solve <option> <input> [<output>] \n");
    printf("The output is optional\n");
    return EXIT_FAILURE;
  }

  if (strcmp("-c", argv[1]) != 0 && strcmp("-s", argv[1]) != 0) {
    printf("Error: second argument <option> should be '-c' if you wish to know the number of existing soltuions to your game \n");
    printf("OR '-s' if you wish to search and save the first solution found to your game in your file given as a 4th argument \n");
    return EXIT_FAILURE;
  }

  game g = game_load(argv[2]);

  if (strcmp("-s", argv[1]) == 0) {
    if (game_nb_solutions(g) == 0) {
      fprintf(stderr, "There's no solution\n");
      return EXIT_FAILURE;
    }
    game g_solved = one_game_solution(g, 0, 0);
    game_save(g_solved, argv[3]);
  }

  if (strcmp("-c", argv[1]) == 0) {
    FILE *file_name = fopen(argv[3], "w");

    if (file_name == NULL) {
      fprintf(stderr, "Couldn't open the file\n");
      return EXIT_FAILURE;
    }
    uint nb_soltions = game_nb_solutions(g);
    printf("There was %d solutions for this game\n", nb_soltions);
    fprintf(file_name, "%u", nb_soltions);
    fclose(file_name);
  }
  return EXIT_SUCCESS;
}