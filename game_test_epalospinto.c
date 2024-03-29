#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

/* ASSERT FUNCTION */

#define ASSERT(expr)                                                                  \
  do {                                                                                \
    if ((expr) == 0) {                                                                \
      fprintf(stderr, "[%s:%d] Assertion '%s' failed!\n", __FILE__, __LINE__, #expr); \
      abort();                                                                        \
    }                                                                                 \
  } while (0)

// TEST DUMMY //

int test_dummy(void) { return EXIT_SUCCESS; }

// TEST PLAY MOVE //

bool test_game_play_move(void) {
  game g = game_new_empty();  // Playing a move and then checking if the right
                              // value is in the game
  game_play_move(g, 0, 0,
                 S_ZERO);  // We assume that get_square is well programmed
  game_play_move(g, 0, 1, S_ONE);
  ASSERT(game_get_square(g, 0, 0) == S_ZERO);
  ASSERT(game_get_square(g, 0, 1) == S_ONE);
  // ASSERT(game_queue_length(game_pile_undo(g)) > 1);
  game_delete(g);
  return true;
}

// TEST HAS ERROR //

bool test_game_has_error(void) {
  game g = game_new_empty();  // Setting three same squares which must be an error
  game_set_square(g, 0, 0, S_ZERO);
  game_set_square(g, 0, 1, S_ZERO);
  game_set_square(g, 0, 2, S_ZERO);
  ASSERT(game_has_error(g, 0, 2) != 0 && game_has_error(g, 0, 2) != false);
  game_set_square(g, 0, 2, S_ONE);
  ASSERT(game_has_error(g, 0, 2) == 0);
  game_delete(g);

  g = game_default_solution();  // Checking that the solution has no errors
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      ASSERT(game_has_error(g, i, j) == 0);
    }
  }
  game_set_square(g, 1, 3, S_ONE);  // Changing a square in the solution and
                                    // rechecking if there's an error
  ASSERT(game_has_error(g, 1, 4) != 0 && game_has_error(g, 1, 4) != false);
  ASSERT(game_has_error(g, 1, 3) != 0 && game_has_error(g, 1, 3) != false);
  ASSERT(game_has_error(g, 1, 2) != 0 && game_has_error(g, 1, 2) != false);
  ASSERT(game_has_error(g, 1, 1) != 0 && game_has_error(g, 1, 1) != false);
  game_delete(g);

  game g_error = game_new_empty_ext(4, 4, false, true);  // Checking with unique option true
  game_set_square(g_error, 0, 0, S_ZERO);                // column 0
  game_set_square(g_error, 0, 1, S_ONE);
  game_set_square(g_error, 0, 2, S_ONE);
  game_set_square(g_error, 0, 3, S_ZERO);

  game_set_square(g_error, 1, 0, S_ZERO);  // column 1 = column 0
  game_set_square(g_error, 1, 1, S_ONE);
  game_set_square(g_error, 1, 2, S_ONE);
  game_set_square(g_error, 1, 3, S_ZERO);

  game_set_square(g_error, 2, 0, S_ONE);  // column 2
  game_set_square(g_error, 2, 1, S_ZERO);
  game_set_square(g_error, 2, 2, S_ZERO);
  game_set_square(g_error, 2, 3, S_ONE);

  game_set_square(g_error, 3, 0, S_ONE);  // column 3 = column 2
  game_set_square(g_error, 3, 1, S_ZERO);
  game_set_square(g_error, 3, 2, S_ZERO);
  game_set_square(g_error, 3, 3, S_ONE);
  if (game_is_unique(g_error) == true) {
    ASSERT(game_has_error(g_error, 0, 1));
  }
  game_delete(g_error);

  return true;
}

// TEST RESTART //

bool test_game_restart(void) {
  game g = game_default();  // Creating two games but one had move played then
                            // restarted, it they aren't equal then it's false
  game g_copy = game_copy(g);
  game_play_move(g, 1, 1, S_ZERO);
  game_restart(g);
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      square s = game_get_square(g, i, j);
      square s_copy = game_get_square(g_copy, i, j);
      ASSERT(s == s_copy);
    }
  }
  // ASSERT(game_queue_is_empty(game_pile_redo(g)));
  // ASSERT(game_queue_is_empty(game_pile_undo(g)));
  game_delete(g);
  game_delete(g_copy);
  return true;
}

// TEST IS OVER //

bool test_game_is_over(void) {
  square s_one[6][6];  // Creating two games, one only with zeros and another
                       // with only ones
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      s_one[i][j] = S_ONE;
    }
  }
  square s_zero[6][6];
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      s_zero[i][j] = S_ZERO;
    }
  }

  game g_one = game_new(*s_one);
  game g_zero = game_new(*s_zero);
  ASSERT(!game_is_over(g_one));  // Both mustn't be over
  ASSERT(!game_is_over(g_zero));
  game_delete(g_one);
  game_delete(g_zero);

  game g_solution = game_default_solution();  // Checking if solution game is over
  ASSERT(game_is_over(g_solution));
  game_play_move(g_solution, 1, 1,
                 S_ZERO);  // Playing a move and now it mustn't be over because not right
  ASSERT(!game_is_over(g_solution));
  game_set_square(g_solution, 1, 1, S_EMPTY);
  game_set_square(g_solution, 5, 1, S_ONE);
  ASSERT(!game_is_over(g_solution));
  game_delete(g_solution);

  game g = game_new_empty_ext(4, 4, false, true);  // Checking with the new game_new with the same ideas
  game_set_square(g, 0, 0, S_ZERO);                // colonne 0
  game_set_square(g, 0, 1, S_ONE);
  game_set_square(g, 0, 2, S_ONE);
  game_set_square(g, 0, 3, S_ZERO);

  game_set_square(g, 1, 0, S_ZERO);  // colonne 1 identique à la colonne 0
  game_set_square(g, 1, 1, S_ONE);
  game_set_square(g, 1, 2, S_ONE);
  game_set_square(g, 1, 3, S_ZERO);

  game_set_square(g, 2, 0, S_ONE);  // colonne 2
  game_set_square(g, 2, 1, S_ZERO);
  game_set_square(g, 2, 2, S_ZERO);
  game_set_square(g, 2, 3, S_ONE);

  game_set_square(g, 3, 0, S_ONE);  // colonne 3 identique à la colonne 2
  game_set_square(g, 3, 1, S_ZERO);
  game_set_square(g, 3, 2, S_ZERO);
  game_set_square(g, 3, 3, S_ONE);
  if (game_is_unique(g) == true) {
    ASSERT(game_is_over(g) == false);
  }
  game_delete(g);
  return true;
}

// TEST DEFAULT SOLUTION //

bool test_game_default_solution(void) {
  game g = game_default();  // Checking if the pointer isn't NULL else it didn't work
  if (g == NULL) {
    return false;
  }
  game_delete(g);

  g = game_default_solution();  // Same with default_solution
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      if (game_has_error(g, i, j) == true) {
        return false;
      }
    }
  }
  if (game_is_over(g) != true) {  // The game must be over since it's the solution
    return false;
  }
  game_delete(g);
  return true;
}

// TEST CHECK MOVE //

bool test_game_check_move(void) {
  game g = game_new_empty();  // Setting immutables in a game, we shouldn't be
                              // able to play on them
  ASSERT(game_check_move(g, 0, 0, S_IMMUTABLE_ZERO) == false);
  ASSERT(game_check_move(g, 0, 1, S_IMMUTABLE_ONE) == false);
  ASSERT(game_check_move(g, 0, 7, S_ONE) == false);
  game_set_square(g, 0, 0, S_ONE);
  game_set_square(g, 0, 1, S_IMMUTABLE_ONE);
  ASSERT(game_check_move(g, 0, 1, S_ONE) == false);
  ASSERT(game_check_move(g, 0, 0, S_ZERO) == true);
  game_delete(g);
  return true;
}

void usage(int argc, char *argv[]) {
  fprintf(stderr, "You need to give one argument");
  exit(EXIT_FAILURE);
}

/* ********** MAIN ROUTINE ********** */

int main(int argc, char *argv[]) {
  if (argc == 1) {
    usage(argc, argv);
  }
  if (strcmp("dummy", argv[1]) == 0) {
    test_dummy();
  }
  if (strcmp("play_move", argv[1]) == 0) {
    test_game_play_move();
  }
  if (strcmp("game_restart", argv[1]) == 0) {
    test_game_restart();
  }
  if (strcmp("game_is_over", argv[1]) == 0) {
    test_game_is_over();
  }
  if (strcmp("game_has_error", argv[1]) == 0) {
    test_game_has_error();
  }
  if (strcmp("game_default_solution", argv[1]) == 0) {
    test_game_default_solution();
  }
  if (strcmp("game_check_move", argv[1]) == 0) {
    test_game_check_move();
  }
}
