#define __USE_GNU
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

/* ASSERT FUNCTION */

#define ASSERT(expr)                                                                  \
  do {                                                                                \
    if ((expr) == 0) {                                                                \
      fprintf(stderr, "[%s:%d] Assertion '%s' failed!\n", __FILE__, __LINE__, #expr); \
      abort();                                                                        \
    }                                                                                 \
  } while (0)

// TEST DUMMY //

int test_dummy() { return true; }

// TEST GET SQUARE //

bool test_get_square(void) {
  game g = game_default();
  if (g == NULL) {
    return false;
  }
  int w = 0;  // creation of a counter to check if it's good for the 36 cases
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == S_EMPTY || game_get_square(g, i, j) == S_ZERO || game_get_square(g, i, j) == S_ONE ||
          game_get_square(g, i, j) == S_IMMUTABLE_ONE || game_get_square(g, i, j) == S_IMMUTABLE_ZERO) {
        w++;
      }
    }
  }
  if (w == 36) {
    game_delete(g);
    return true;
  } else {
    game_delete(g);
    return false;
  }
}

// TEST GAME DEFAULT //

bool test_default(void) {
  game g = game_default();  // creation of two games in order to compare them, game_default must be empty when there's no immutable square
  game g_solution = game_default_solution();
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      if (game_is_immutable(g_solution, i, j) == false) {
        ASSERT(game_get_square(g, i, j) == S_EMPTY);
      } else {
        ASSERT(game_get_square(g, i, j) == game_get_square(g_solution, i, j));
      }
    }
  }
  game_delete(g_solution);
  game_delete(g);
  return true;
}

// TEST GET NUMBER //

bool test_get_number(void) {
  game g = game_new_empty();  // Setting squares in an empty game and then checking if we have the right square with the function
  game_set_square(g, 0, 0, S_EMPTY);
  game_set_square(g, 0, 1, S_ZERO);
  game_set_square(g, 0, 2, S_ONE);
  game_set_square(g, 0, 3, S_IMMUTABLE_ZERO);
  game_set_square(g, 0, 4, S_IMMUTABLE_ONE);
  ASSERT(game_get_number(g, 0, 0) == -1);
  ASSERT(game_get_number(g, 0, 1) == 0);
  ASSERT(game_get_number(g, 0, 2) == 1);
  ASSERT(game_get_number(g, 0, 3) == 0);
  ASSERT(game_get_number(g, 0, 4) == 1);
  game_delete(g);
  return true;
}

// TEST IS IMMUTABLE //

bool test_is_immutable(void) {
  game g = game_default();  // Creating a game that already has immutables, then checking if the function finds them

  if (game_is_immutable(g, 0, 1) == true && game_is_immutable(g, 2, 1) == true && game_is_immutable(g, 3, 2) == true && game_is_immutable(g, 4, 5) == true &&
      game_is_immutable(g, 5, 4) == false && game_is_immutable(g, 0, 0) == false) {
    game_delete(g);
    return true;
  } else {
    game_delete(g);
    return false;
  }
}

// TEST IS EMPTY //

bool test_is_empty(void) {
  game g = game_new_empty();  // Similar idea as is immutable but with squares
  game_set_square(g, 0, 1, S_EMPTY);
  game_set_square(g, 0, 2, S_ZERO);
  game_set_square(g, 0, 3, S_ONE);
  game_set_square(g, 0, 4, S_IMMUTABLE_ZERO);
  game_set_square(g, 0, 5, S_IMMUTABLE_ONE);
  ASSERT(game_is_empty(g, 0, 1) == true);
  ASSERT(!game_is_empty(g, 0, 2) == true);
  ASSERT(!game_is_empty(g, 0, 3) == true);
  ASSERT(!game_is_empty(g, 0, 4) == true);
  ASSERT(!game_is_empty(g, 0, 5) == true);
  game_delete(g);
  return true;
}

// TEST GET NEXT SQUARE //

bool test_get_next_square(void) {
  game g = game_new_empty();  // Same idea as the previous test
  game_set_square(g, 0, 1, S_EMPTY);
  game_set_square(g, 1, 2, S_ZERO);
  game_set_square(g, 1, 0, S_ONE);
  game_set_square(g, 2, 1, S_IMMUTABLE_ZERO);
  ASSERT(game_get_next_square(g, 1, 1, DOWN, 1) == S_IMMUTABLE_ZERO);
  ASSERT(game_get_next_square(g, 1, 1, LEFT, 1) == S_ONE);
  ASSERT(game_get_next_square(g, 1, 1, RIGHT, 1) == S_ZERO);
  ASSERT(game_get_next_square(g, 1, 1, UP, 1) == S_EMPTY);
  ASSERT(game_get_next_square(g, 1, 1, UP, 2) == -1);
  ASSERT(game_get_next_square(g, 0, 1, UP, 0) == S_EMPTY);
  game_delete(g);

  g = game_new_empty_ext(6, 6, true, false);
  game_set_square(g, 0, 5, S_ZERO);
  game_set_square(g, 5, 3, S_IMMUTABLE_ZERO);
  if (game_is_wrapping(g) == true) {
    ASSERT(game_get_next_square(g, 0, 0, LEFT, 1) == S_ZERO);
    ASSERT(game_get_next_square(g, 0, 3, UP, 1) == S_IMMUTABLE_ZERO);
  }
  game_delete(g);
  return true;
}

// TEST GET NEXT NUMBER //

bool test_get_next_number(void) {
  game g = game_new_empty();  // Same idea as the previous test
  game_set_square(g, 0, 1, S_EMPTY);
  game_set_square(g, 1, 2, S_ZERO);
  game_set_square(g, 1, 0, S_ONE);
  game_set_square(g, 2, 1, S_IMMUTABLE_ZERO);
  ASSERT(game_get_next_number(g, 1, 1, DOWN, 1) == 0);
  ASSERT(game_get_next_number(g, 1, 1, LEFT, 1) == 1);
  ASSERT(game_get_next_number(g, 1, 1, RIGHT, 1) == 0);
  ASSERT(game_get_next_number(g, 1, 1, UP, 1) == -1);
  ASSERT(game_get_next_number(g, 1, 1, UP, 2) == -1);
  ASSERT(game_get_next_number(g, 0, 1, UP, 0) == -1);
  game_delete(g);
  return true;
}

// TEST UNDO //

bool test_undo(void) {
  game g = game_new_empty();  // We play a move and then undo it, so the square must be empty
  game_play_move(g, 0, 1, S_ONE);
  game_undo(g);
  ASSERT(game_get_square(g, 0, 1) == S_EMPTY);
  game_delete(g);
  return true;
}

// TEST REDO //

bool test_redo(void) {
  game g = game_new_empty();        // We play a move and then undo it, the square must be empty, then we redo it,
  game_play_move(g, 1, 2, S_ZERO);  // so the square must be same as in the begining
  game_undo(g);
  ASSERT(game_get_square(g, 1, 2) == S_EMPTY);
  game_redo(g);
  ASSERT(game_get_square(g, 1, 2) == S_ZERO);
  // ASSERT(queue_length(g->pile_undo) == 1);  // check si queue_undo avant redo == avec queue_undo après redo
  game_delete(g);
  return true;
}

// TEST LOAD //

bool test_load(void) {
  // Create a test file with the expected data
  FILE *f = fopen("test_game.txt", "w");
  fprintf(f, "6 8 1 0\neBebee\neeeeee\neeeeee\neeeeee\neeeeee\neeeweW\n");
  fclose(f);

  // Load the test file
  game g = game_load("test_game.txt");

  // Check the parameters of the loaded game
  ASSERT(game_nb_rows(g) == 6);
  ASSERT(game_nb_cols(g) == 8);
  ASSERT(game_is_wrapping(g) == 1);
  ASSERT(game_is_unique(g) == 0);

  // Check the squares of the loaded game
  ASSERT(game_get_square(g, 0, 1) == S_IMMUTABLE_ONE);
  ASSERT(game_get_square(g, 0, 3) == S_ONE);
  ASSERT(game_get_square(g, 3, 3) == S_EMPTY);
  ASSERT(game_get_square(g, 5, 3) == S_ZERO);
  ASSERT(game_get_square(g, 5, 5) == S_IMMUTABLE_ZERO);

  // Ending the test
  game_delete(g);
  return true;
}

// TEST SAVE //

bool test_save(void) {
  // Create a test file with the expected data
  game g = game_new_empty_ext(3, 3, true, true);
  game_set_square(g, 0, 0, S_ONE);
  game_set_square(g, 0, 1, S_IMMUTABLE_ONE);
  game_set_square(g, 0, 2, S_ONE);

  game_set_square(g, 1, 0, S_ZERO);
  game_set_square(g, 1, 1, S_IMMUTABLE_ZERO);
  game_set_square(g, 1, 2, S_ZERO);

  game_set_square(g, 2, 0, S_EMPTY);
  game_set_square(g, 2, 1, S_EMPTY);
  game_set_square(g, 2, 2, S_EMPTY);

  game_save(g, "test.txt");

  // Open the test file and check if it's right
  FILE *f = fopen("test.txt", "r");
  if (f == NULL) {
    exit(EXIT_FAILURE);
  }
  int i, j, wrapping, unique;
  fscanf(f, "%d %d %d %d\n", &i, &j, &wrapping, &unique);
  ASSERT(i == 3);
  ASSERT(j == 3);
  ASSERT(wrapping == 1);
  ASSERT(unique == 1);
  char square;
  fscanf(f, "%c", &square);
  ASSERT(square == 'b');
  fscanf(f, "%c", &square);
  ASSERT(square == 'B');
  fscanf(f, "%c", &square);
  ASSERT(square == 'b');
  fscanf(f, "%c", &square);  // In order to jump the \n and not have it in the scanf
  fscanf(f, "%c", &square);
  ASSERT(square == 'w');
  fscanf(f, "%c", &square);
  ASSERT(square == 'W');
  fscanf(f, "%c", &square);
  ASSERT(square == 'w');
  fscanf(f, "%c", &square);
  fscanf(f, "%c", &square);
  ASSERT(square == 'e');
  fscanf(f, "%c", &square);
  ASSERT(square == 'e');
  fscanf(f, "%c", &square);
  ASSERT(square == 'e');

  // Ending the test
  fclose(f);
  game_delete(g);
  return true;
}

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

/* ********** MAIN ROUTINE ********** */

int main(int argc, char *argv[]) {
  if (argc == 1) usage(argc, argv);

  // start test
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
  bool test_result = false;
  if (strcmp("dummy", argv[1]) == 0)
    test_result = test_dummy();
  else if (strcmp("game_get_square", argv[1]) == 0)
    test_result = test_get_square();
  else if (strcmp("game_default", argv[1]) == 0)
    test_result = test_default();
  else if (strcmp("game_get_number", argv[1]) == 0)
    test_result = test_get_number();
  else if (strcmp("game_is_immutable", argv[1]) == 0)
    test_result = test_is_immutable();
  else if (strcmp("game_is_empty", argv[1]) == 0)
    test_result = test_is_empty();
  else if (strcmp("game_get_next_square", argv[1]) == 0)
    test_result = test_get_next_square();
  else if (strcmp("game_get_next_number", argv[1]) == 0)
    test_result = test_get_next_number();
  else if (strcmp("game_undo", argv[1]) == 0)
    test_result = test_undo();
  else if (strcmp("game_redo", argv[1]) == 0)
    test_result = test_redo();
  else if (strcmp("game_load", argv[1]) == 0)
    test_result = test_load();
  else if (strcmp("game_save", argv[1]) == 0)
    test_result = test_save();
  else {
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  // print test result
  if (test_result) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}
