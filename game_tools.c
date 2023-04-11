#include "game_tools.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

/* ************************************************************************** */

game game_load(char *filename) {
  // Opening the file and checking if it was opened
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    printf("That file doesn't exist\n");
    exit(EXIT_FAILURE);
  }

  // Reading the first line and setting the parameters
  int x, y, z, w;
  fscanf(f, "%d %d %d %d", &x, &y, &z, &w);
  game g = game_new_empty_ext(x, y, z, w);

  // Reading all the other lines and playing those moves
  char play;
  fscanf(f, "%c", &play);
  for (int i = 0; i <= game_nb_cols(g); i++) {
    for (int j = 0; j <= game_nb_rows(g); j++) {
      fscanf(f, "%c", &play);
      switch (play) {
        case 'e':
          game_set_square(g, i, j, S_EMPTY);
          break;
        case 'w':
          game_set_square(g, i, j, S_ZERO);
          break;
        case 'b':
          game_set_square(g, i, j, S_ONE);
          break;
        case 'W':
          game_set_square(g, i, j, S_IMMUTABLE_ZERO);
          break;
        case 'B':
          game_set_square(g, i, j, S_IMMUTABLE_ONE);
          break;
      }
    }
  }
  // Closing the file and returning the game
  fclose(f);
  return g;
}

/* ************************************************************************** */

void game_save(cgame g, char *filename) {
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  uint i = game_nb_rows(g);
  uint j = game_nb_cols(g);
  FILE *fichier = NULL;
  fichier = fopen(filename, "w");
  if (fichier == NULL) {
    exit(EXIT_FAILURE);
  }
  fprintf(fichier, "%d %d ", i, j);
  if (game_is_wrapping(g) == true) {
    fprintf(fichier, "1 ");
  } else {
    fprintf(fichier, "0 ");
  }
  if (game_is_unique(g) == true) {
    fprintf(fichier, "1\n");
  } else {
    fprintf(fichier, "0\n");
  }
  for (int x = 0; x < j; x++) {
    for (int y = 0; y < i; y++) {
      square value = game_get_square(g, x, y);
      if (value == S_EMPTY) {
        fprintf(fichier, "e");
      }
      if (value == S_ONE) {
        fprintf(fichier, "b");
      }
      if (value == S_ZERO) {
        fprintf(fichier, "w");
      }
      if (value == S_IMMUTABLE_ONE) {
        fprintf(fichier, "B");
      }
      if (value == S_IMMUTABLE_ZERO) {
        fprintf(fichier, "W");
      }
      if (y == i - 1) {
        fprintf(fichier, "\n");
      }
    }
  }
  fclose(fichier);
}

/* ************************************************************************** */

void game_solve_rec(game g, uint i, uint j, uint *counter, uint S_ZEROs, uint S_ONEs) {
  if (game_nb_rows(g) <= i) {
    if (game_is_over(g)) {
      (*counter)++;
    }
  } else if (game_nb_cols(g) <= j) {
    game_solve_rec(g, i + 1, 0, counter, game_nb_cols(g) / 2, game_nb_cols(g) / 2);
  } else if (game_is_immutable(g, i, j)) {
    if (game_get_number(g, i, j) == 0) {
      game_solve_rec(g, i, j + 1, counter, S_ZEROs - 1, S_ONEs);
    } else {
      game_solve_rec(g, i, j + 1, counter, S_ZEROs, S_ONEs - 1);
    }
  } else {
    if (S_ZEROs > 0 && game_check_move(g, i, j, S_ZERO)) {
      game_set_square(g, i, j, S_ZERO);
      game_solve_rec(g, i, j + 1, counter, S_ZEROs - 1, S_ONEs);
    }
    if (S_ONEs > 0 && game_check_move(g, i, j, S_ONE)) {
      game_set_square(g, i, j, S_ONE);
      game_solve_rec(g, i, j + 1, counter, S_ZEROs, S_ONEs - 1);
    }
  }
}

/* ************************************************************************** */

game one_game_solution(game g, uint i, uint j) {
  if (game_nb_rows(g) <= i) {
    if (game_is_over(g)) {
      return g;
    }
    return NULL;
  } else if (game_nb_cols(g) <= j) {
    return one_game_solution(g, i + 1, 0);
  } else if (game_is_immutable(g, i, j)) {
    return one_game_solution(g, i, j + 1);
  } else {
    game_set_square(g, i, j, S_ZERO);
    game game_solution = one_game_solution(g, i, j + 1);
    if (game_solution != NULL) {
      return game_solution;
    }
    game_set_square(g, i, j, S_ONE);
    return one_game_solution(g, i, j + 1);
  }
  return NULL;
}

/* ************************************************************************** */

uint game_nb_solutions(cgame g) {
  uint i = 0, j = 0, counter = 0, begin = game_nb_cols(g) / 2;
  game g_nb_sols = game_copy(g);  // copy the game because it's unchangeable so that I can change it when calling recursive version
  game_solve_rec(g_nb_sols, i, j, &counter, begin, begin);
  return counter;
}

/* ************************************************************************** */
bool game_solve(game g) {
  if (game_nb_solutions(g) <= 0) {
    return false;
  } else {
    return true;
  }
}
