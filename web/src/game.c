/**
 * @file game.c
 * @copyright University of Bordeaux. All rights reserved, 2022.
 **/

#include "game.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_ext.h"
#include "game_private.h"
#include "game_struct.h"
#include "queue.h"

/* ************************************************************************** */
/*                                 GAME BASIC                                 */
/* ************************************************************************** */

game game_new(square* squares) { return game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, squares, false, false); }

/* ************************************************************************** */

game game_new_empty(void) { return game_new_empty_ext(DEFAULT_SIZE, DEFAULT_SIZE, false, false); }

/* ************************************************************************** */

game game_copy(cgame g)
{
  game gg = game_new_ext(g->nb_rows, g->nb_cols, g->squares, g->wrapping, g->unique);
  return gg;
}

/* ************************************************************************** */

bool game_equal(cgame g1, cgame g2)
{
  assert(g1 && g2);

  if (g1->nb_rows != g2->nb_rows) return false;
  if (g1->nb_cols != g2->nb_cols) return false;

  for (uint i = 0; i < g1->nb_rows; i++)
    for (uint j = 0; j < g1->nb_cols; j++) {
      if (SQUARE(g1, i, j) != SQUARE(g2, i, j)) return false;
    }

  if (g1->wrapping != g2->wrapping) return false;
  if (g1->unique != g2->unique) return false;

  return true;
}

/* ************************************************************************** */

void game_delete(game g)
{
  free(g->squares);
  queue_free_full(g->undo_stack, free);
  queue_free_full(g->redo_stack, free);
  free(g);
}

/* ************************************************************************** */

void game_set_square(game g, uint i, uint j, square s)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  SQUARE(g, i, j) = s;
}

/* ************************************************************************** */

square game_get_square(cgame g, uint i, uint j)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  return SQUARE(g, i, j);
}

/* ************************************************************************** */

/* Nota Bene: https://en.cppreference.com/w/c/language/array_initialization */
static int i_offset[] = {[UP] = -1, [DOWN] = +1, [LEFT] = 0, [RIGHT] = 0};
static int j_offset[] = {[UP] = 0, [DOWN] = 0, [LEFT] = -1, [RIGHT] = 1};

int game_get_next_square(cgame g, uint i, uint j, direction dir, uint dist)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  assert(dist <= 2);

  // move to the next square in a given direction
  int ii = i + dist * i_offset[dir];
  int jj = j + dist * j_offset[dir];

  if (game_is_wrapping(g)) {
    ii = (ii + game_nb_rows(g)) % game_nb_rows(g);
    jj = (jj + game_nb_cols(g)) % game_nb_cols(g);
  }

  // check if next square at (ii,jj) is out of grid
  if (ii < 0 || ii >= (int)g->nb_rows) return -1;
  if (jj < 0 || jj >= (int)g->nb_cols) return -1;

  return SQUARE(g, ii, jj);
}

/* ************************************************************************** */

bool game_is_empty(cgame g, uint i, uint j)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  square s = SQUARE(g, i, j);
  if (s == S_EMPTY) return true;
  return false;
}

/* ************************************************************************** */

int game_get_number(cgame g, uint i, uint j)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  square s = SQUARE(g, i, j);
  if ((s == S_ZERO) || (s == S_IMMUTABLE_ZERO)) return 0;
  if ((s == S_ONE) || (s == S_IMMUTABLE_ONE)) return 1;
  return -1;
}

/* ************************************************************************** */

int game_get_next_number(cgame g, uint i, uint j, direction dir, uint dist)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  assert(dist <= 2);

  int s = game_get_next_square(g, i, j, dir, dist);
  if ((s == S_ZERO) || (s == S_IMMUTABLE_ZERO)) return 0;
  if ((s == S_ONE) || (s == S_IMMUTABLE_ONE)) return 1;
  return -1;  // if EMPTY or INVALID
}

/* ************************************************************************** */

bool game_is_immutable(cgame g, uint i, uint j)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  square s = SQUARE(g, i, j);
  if ((s == S_IMMUTABLE_ZERO) || (s == S_IMMUTABLE_ONE)) return true;
  return false;
}

/* ************************************************************************** */

int game_has_error(cgame g, uint i, uint j)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);

  int num = game_get_number(g, i, j);

  // no error occurs if the square is empty
  if (num == -1) return 0;

  // check number of ones and zeros in row i
  uint nb_zeros = _count_in_row(g, 0, i);
  uint nb_ones = _count_in_row(g, 1, i);
  if ((num == 0) && (nb_zeros > (g->nb_cols / 2))) return 1;
  if ((num == 1) && (nb_ones > (g->nb_cols / 2))) return 1;

  // check number of ones and zeros in col j
  nb_zeros = _count_in_col(g, 0, j);
  nb_ones = _count_in_col(g, 1, j);
  if ((num == 0) && (nb_zeros > (g->nb_rows / 2))) return 2;
  if ((num == 1) && (nb_ones > (g->nb_rows / 2))) return 2;

  // check three consecutive in row i
  bool cons_row = _three_consecutive_row(g, i, j);
  if (cons_row) return 3;

  // check three consecutive in col j
  bool cons_col = _three_consecutive_col(g, i, j);
  if (cons_col) return 4;

  if (g->unique) {
    // check row i is unique
    bool uniq_row = _unique_row(g, i, false);
    if (!uniq_row) return 5;

    // check col j is unique
    bool uniq_col = _unique_col(g, j, false);
    if (!uniq_col) return 6;
  }

  return 0;
}

/* ************************************************************************** */

bool game_check_move(cgame g, uint i, uint j, square s)
{
  assert(g);
  if (i >= g->nb_rows) return false;
  if (j >= g->nb_cols) return false;

  if (s != S_EMPTY && s != S_ZERO && s != S_ONE) return false;
  bool immutable = game_is_immutable(g, i, j);
  if (immutable) return false;

  return true;  // regular move
}

/* ************************************************************************** */

void game_play_move(game g, uint i, uint j, square s)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);

  // check move
  assert(s == S_EMPTY || s == S_ZERO || s == S_ONE);
  bool immutable = game_is_immutable(g, i, j);
  assert(!immutable);

  square cs = SQUARE(g, i, j);  // save current square
  SQUARE(g, i, j) = s;          // update with new square

  // save history
  _stack_clear(g->redo_stack);
  move m = {i, j, cs, s};
  _stack_push_move(g->undo_stack, m);
}

/* ************************************************************************** */

bool game_is_over(cgame g)
{
  assert(g);

  // check number of ones and zeros in rows
  for (uint i = 0; i < g->nb_rows; i++) { /* row i */
    uint nb_zeros = _count_in_row(g, 0, i);
    uint nb_ones = _count_in_row(g, 1, i);
    if (nb_zeros != (g->nb_cols / 2)) return false;
    if (nb_ones != (g->nb_cols / 2)) return false;
  }

  // check number of ones and zeros in cols
  for (uint j = 0; j < g->nb_cols; j++) { /* col j */
    uint nb_zeros = _count_in_col(g, 0, j);
    uint nb_ones = _count_in_col(g, 1, j);
    if (nb_zeros != (g->nb_rows / 2)) return false;
    if (nb_ones != (g->nb_rows / 2)) return false;
  }

  // check three consecutive in cols & rows
  for (uint i = 0; i < g->nb_rows; i++) {
    for (uint j = 0; j < g->nb_cols; j++) {
      bool cons_row = _three_consecutive_row(g, i, j);
      bool cons_col = _three_consecutive_col(g, i, j);
      if (cons_row || cons_col) return false;
    }
  }

  if (g->unique) {
    // check each row is unique
    for (uint i = 0; i < g->nb_rows; i++) { /* row i */
      bool uniq_row = _unique_row(g, i, true);
      if (!uniq_row) return false;
    }

    // check each col is unique
    for (uint j = 0; j < g->nb_cols; j++) { /* col j */
      bool uniq_col = _unique_col(g, j, true);
      if (!uniq_col) return false;
    }
  }

  return true;
}

/* ************************************************************************** */

void game_restart(game g)
{
  assert(g);

  for (uint i = 0; i < g->nb_rows; i++)
    for (uint j = 0; j < g->nb_cols; j++) {
      square s = SQUARE(g, i, j);
      if ((s == S_ONE) || (s == S_ZERO)) SQUARE(g, i, j) = S_EMPTY;
    }

  // reset history
  _stack_clear(g->undo_stack);
  _stack_clear(g->redo_stack);
}

/* ************************************************************************** */
