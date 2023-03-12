#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

// TEST DUMMY //

bool test_dummy() { return true; }

// TEST GAME NEW //

bool test_game_new() {  // Given a random array of squares it just checks if game
                        // new really gives each cell it's square value
  square *s = (square *)malloc(DEFAULT_SIZE * DEFAULT_SIZE * sizeof(square));
  assert(s);

  game g = game_new(s);

  if (g == NULL) {
    fprintf(stderr,
            "game_new function did not do it's job: returned a NULL "
            "pointer to the game!\n");
    game_delete(g);
    return false;
  }

  for (uint x = 0; x < DEFAULT_SIZE; x++) {
    for (uint y = 0; y < DEFAULT_SIZE; y++) {
      if (game_get_square(g, x, y) != s[(x * DEFAULT_SIZE) + y]) {
        fprintf(stderr,
                "game_new function did not set correctly the square of cell "
                "(%u,%u)!\n",
                x, y);
        game_delete(g);
        return false;
      }
    }
  }
  game_delete(g);
  free(s);
  return true;
}

// TEST GAME NEW EXT //

bool test_game_new_ext() {  // Same as the previous test but with the extention
                            // version so additional params
  square s[24] = {S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY,
                  S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY, S_EMPTY};
  uint rows = 4;
  uint cols = 6;
  bool wrapping = false;
  bool unique = false;
  game g = game_new_ext(rows, cols, s, wrapping, unique);
  if (g == NULL) {
    fprintf(stderr,
            "game_new_ext function did not do it's job: returned a NULL "
            "pointer to the game!\n");
    game_delete(g);
    return false;
  }
  if (game_nb_rows(g) == cols && game_nb_cols(g) == rows) {
    fprintf(stderr,
            "Error: in game_new_empty, function inversed g->nb_rows "
            "and g->nb_cols\n");
    game_delete(g);
    return false;
  }

  else {
    if (game_nb_cols(g) != cols) {
      fprintf(stderr, "Error in game_new_ext nb_cols isn't set to the right value\n");
      game_delete(g);
      return false;
    }
    if (game_nb_rows(g) != rows) {
      fprintf(stderr, "Error in game_new_ext nb_rows isn't set to the right value\n");
      game_delete(g);
      return false;
    }
  }
  if (game_is_wrapping(g) != wrapping) {
    fprintf(stderr, "Error in game_new_ext wrapping isn't set to the right value\n");
    game_delete(g);
    return false;
  }

  if (game_is_unique(g) != unique) {
    fprintf(stderr, "Error in game_new_ext unique isn't set to the right value\n");
    game_delete(g);
    return false;
  }

  for (uint x = 0; x < rows; x++) {
    for (uint y = 0; y < cols; y++) {
      if (game_get_square(g, x, y) != S_EMPTY) {
        fprintf(stderr,
                "game_new_ext function did not set correctly the square of "
                "cell (%u,%u)!\n",
                x, y);
        game_delete(g);
        return false;
      }
    }
  }
  game_delete(g);

  return true;
}

// TEST GAME NEW EMPTY //

bool test_game_new_empty() {  // Checks if the function returns a game where all
                              // it's square are set to empty
  game g = game_new_empty();

  if (g == NULL) {
    fprintf(stderr,
            "game_new_empty function did not do it's job: returned a NULL "
            "pointer to the game!\n");
    return false;
  }

  for (uint x = 0; x < DEFAULT_SIZE; x++) {
    for (uint y = 0; y < DEFAULT_SIZE; y++) {
      if ((game_get_square(g, x, y) != S_EMPTY)) {
        fprintf(stderr, "game_new_empty function did not initialize an empty game!\n");
        game_delete(g);
        return false;
      }
    }
  }
  game_delete(g);
  return true;
}

// TEST GAME NEW EMPTY EXT//

bool test_game_new_empty_ext() {
  uint nb_rows = 6, nb_cols = 6;
  bool wrapping = true, unique = false;

  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping, unique);
  if (g == NULL) {
    fprintf(stderr,
            "game_new_empty_ext function did not do it's job: returned "
            "a NULL pointer to the game!\n");
    game_delete(g);
    return false;
  }
  if (game_is_wrapping(g) != wrapping) {  // checking the wrapping field
    fprintf(stderr,
            "game_new_empty_ext function did not put the wrapping "
            "field of the structure to the right value!\n");
    game_delete(g);
    return false;
  }
  if (game_is_unique(g) != unique) {  // checking the unique field
    fprintf(stderr,
            "game_new_empty_ext function did not put the unique field "
            "of the structure to the right value!\n");
    game_delete(g);
    return false;
  }

  if (game_nb_cols(g) != nb_cols) {  // checking the nb_cols field
    fprintf(stderr,
            "game_new_empty_ext function did not put the nb_cols field "
            "of the structure to the right value!\n");
    game_delete(g);
    return false;
  }
  if (game_nb_rows(g) != nb_rows) {  // checking the nb_rows field
    fprintf(stderr,
            "game_new_empty_ext function did not put the nb_rows field "
            "of the structure to the right value!\n");
    game_delete(g);
    return false;
  }

  // checking square value
  for (uint x = 0; x < game_nb_rows(g); x++) {
    for (uint y = 0; y < game_nb_cols(g); y++) {
      if ((game_get_square(g, x, y) != S_EMPTY)) {
        fprintf(stderr, "game_new_empty_ext function did not initialize an empty game!\n");
        game_delete(g);
        return false;
      }
    }
  }

  game_delete(g);
  return true;
}

// TEST GAME COPY//

bool test_game_copy() {
  game g = game_new_empty_ext(6, 6, true, true);
  game_play_move(g, 0, 0, S_ONE);
  game g_copy = game_copy(g);
  if (game_get_square(g, 0, 0) != game_get_square(g_copy, 0, 0)) {  // Checking the square value if it's the
                                                                    // same as the one of copied game
    game_delete(g);
    game_delete(g_copy);
    return false;
  }

  // checking if the extention options are copied too
  if (game_is_unique(g) != game_is_unique(g_copy) || game_is_wrapping(g) != game_is_wrapping(g_copy)) {
    game_delete(g);
    game_delete(g_copy);
    return false;
  }
  game_delete(g);
  game_delete(g_copy);
  return true;
}

// TEST GAME EQUAL //

bool test_game_equal() {
  cgame cg = game_default();
  game g = game_default();

  if (cg == NULL || g == NULL) {
    fprintf(stderr,
            "Error: game_default returned a NULL pointer to the game "
            "in functon test_game_equal!\n");
    game_delete(g);
    return false;
  }

  // a problem that can happen: having at least one pointer with a NULL address
  // and game_equal returning true

  if ((cg == NULL || g == NULL) && (game_equal(cg, g) == true)) {
    fprintf(stderr,
            "Error: at least one of the 2 game pointers is NULL while "
            "the function game_equal returned true!\n");
    game_delete(g);
    return false;
  }

  for (uint x = 0; x < DEFAULT_SIZE; x++) {
    for (uint y = 0; y < DEFAULT_SIZE; y++) {
      if (game_get_number(cg, x, y) == 0 && game_get_number(g, x, y) == 0) {
        game_set_square(g, x, y, S_ZERO);
      }

      else if (game_get_number(cg, x, y) == 1 && game_get_number(g, x, y) == 1) {
        game_set_square(g, x, y, S_ONE);
      }

      if ((game_get_number(cg, x, y) == 0 && game_get_square(cg, x, y) == S_IMMUTABLE_ZERO) &&
          (game_get_number(g, x, y) == 0 && game_get_square(g, x, y) == S_ZERO) && game_equal(cg, g) == true) {
        fprintf(stderr,
                "Error: game_equal function returned true while the squares in "
                "cell (%u,%u) are different in the 2 games given as params!\n",
                x, y);
        game_delete(g);
        return false;

      }

      else if ((game_get_number(cg, x, y) == 1 && game_get_square(cg, x, y) == S_IMMUTABLE_ONE) &&
               (game_get_number(g, x, y) == 1 && game_get_square(g, x, y) == S_ONE) && game_equal(cg, g) == true) {
        fprintf(stderr,
                "Error: game_equal function returned true while the squares in "
                "cell (%u,%u) are different in the 2 games given as params!\n",
                x, y);
        game_delete(g);
        return false;
      }
    }
  }
  game_delete(g);
  return true;
}

// TEST GAME DELETE //

bool test_game_delete() {
  square *s = (square *)malloc(DEFAULT_SIZE * DEFAULT_SIZE * sizeof(square));
  assert(s);

  game g = game_new(s);

  if (g == NULL) {
    fprintf(stderr, "Error: game_new pointer is NULL !\n");
    free(s);
    return false;
  }
  game_delete(g);
  free(s);
  return true;
}

// TEST GAME SET SQUARE //

bool test_game_set_square() {
  square *p_s = (square *)malloc(DEFAULT_SIZE * DEFAULT_SIZE * sizeof(square));
  assert(p_s);

  game g = game_new(p_s);

  for (uint x = 0; x < DEFAULT_SIZE; x++) {
    for (uint y = 0; y < DEFAULT_SIZE; y++) {
      game_set_square(g, x, y, *p_s);

      if (game_get_square(g, x, y) != *p_s) {
        fprintf(stderr,
                "Error: game_set_square did not set the cell (%u,%u) at the "
                "value of a given square!\n",
                x, y);
        free(p_s);
        game_delete(g);
        return false;
      }
    }
  }

  free(p_s);
  game_delete(g);
  return true;
}

// TEST GAME PRINT //

bool test_game_print() {
  game g = game_default();
  game_print(g);
  game_delete(g);
  return true;
}

// TEST GAME NB ROWS //

bool test_game_nb_rows() {
  game g = game_new_empty_ext(4, 8, true, false);
  assert(game_nb_rows(g) == 4);
  game_delete(g);
  return true;
}

// TEST GAME NB COLS //

bool test_game_nb_cols() {
  game g = game_new_empty_ext(4, 6, true, false);
  assert(game_nb_cols(g) == 6);
  game_delete(g);
  return true;
}

// TEST GAME IS WRAPPING //

bool test_game_is_wrapping() {
  game g = game_new_empty_ext(6, 6, true, false);
  assert(game_is_wrapping(g) == true);
  assert(game_is_wrapping(g) != false);
  game_delete(g);
  return true;
}

// TEST GAME IS UNIQUE //

bool test_game_is_unique() {
  game g = game_new_empty_ext(6, 6, true, false);
  assert(game_is_unique(g) == false);
  assert(game_is_unique(g) != true);
  game_delete(g);
  return true;
}

/* ********** MAIN ROUTINE ********** */

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Error: Only the excutable and the test name are required\n");
    fprintf(stderr, "Example: $./game_test_jzebian game_new\n");
    return EXIT_FAILURE;
  }

  bool ok = false;

  if (strcmp("dummy", argv[1]) == 0) {
    ok = test_dummy();
  } else if (strcmp("game_new", argv[1]) == 0) {
    ok = test_game_new();
  } else if (strcmp("game_new_ext", argv[1]) == 0) {
    ok = test_game_new_ext();
  } else if (strcmp("game_new_empty", argv[1]) == 0) {
    ok = test_game_new_empty();
  } else if (strcmp("game_new_empty_ext", argv[1]) == 0) {
    ok = test_game_new_empty_ext();
  } else if (strcmp("game_copy", argv[1]) == 0) {
    ok = test_game_copy();
  } else if (strcmp("game_equal", argv[1]) == 0) {
    ok = test_game_equal();
  } else if (strcmp("game_delete", argv[1]) == 0) {
    ok = test_game_delete();
  } else if (strcmp("game_set_square", argv[1]) == 0) {
    ok = test_game_set_square();
  } else if (strcmp("game_print", argv[1]) == 0) {
    ok = test_game_print();
  } else if (strcmp("game_nb_rows", argv[1]) == 0) {
    ok = test_game_nb_rows();
  } else if (strcmp("game_nb_cols", argv[1]) == 0) {
    ok = test_game_nb_cols();
  } else if (strcmp("game_is_wrapping", argv[1]) == 0) {
    ok = test_game_is_wrapping();
  } else if (strcmp("game_is_unique", argv[1]) == 0) {
    ok = test_game_is_unique();
  } else {
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    return EXIT_FAILURE;
  }
  if (ok) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}
