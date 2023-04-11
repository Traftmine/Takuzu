/**
 * @file game_private.c
 * @copyright University of Bordeaux. All rights reserved, 2022.
 **/

#include "game_private.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_ext.h"
#include "game_struct.h"
#include "queue.h"

/* ************************************************************************** */
/*                             STACK ROUTINES                                 */
/* ************************************************************************** */

void _stack_push_move(queue* q, move m)
{
  assert(q);
  move* pm = malloc(sizeof(move));
  assert(pm);
  *pm = m;
  queue_push_head(q, pm);
}

/* ************************************************************************** */

move _stack_pop_move(queue* q)
{
  assert(q);
  move* pm = queue_pop_head(q);
  assert(pm);
  move m = *pm;
  free(pm);
  return m;
}

/* ************************************************************************** */

bool _stack_is_empty(queue* q)
{
  assert(q);
  return queue_is_empty(q);
}

/* ************************************************************************** */

void _stack_clear(queue* q)
{
  assert(q);
  queue_clear_full(q, free);
  assert(queue_is_empty(q));
}

/* ************************************************************************** */
/*                                  AUXILIARY                                 */
/* ************************************************************************** */

int _count_in_row(cgame g, int nb, uint i)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(nb == 0 || nb == 1);
  uint count = 0;
  for (uint jj = 0; jj < g->nb_cols; jj++) {
    int val = game_get_number(g, i, jj);
    if (val == nb) count++;
  }
  return count;
}

/* ************************************************************************** */

int _count_in_col(cgame g, int nb, uint j)
{
  assert(g);
  assert(j < g->nb_cols);
  assert(nb == 0 || nb == 1);
  uint count = 0;
  for (uint ii = 0; ii < g->nb_rows; ii++) {
    int val = game_get_number(g, ii, j);
    if (val == nb) count++;
  }
  return count;
}

/* ************************************************************************** */

bool _three_consecutive_row(cgame g, uint i, uint j)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);

  int h0 = game_get_number(g, i, j);
  if (h0 == -1) return false;

  int l1 = game_get_next_number(g, i, j, LEFT, 1);
  int l2 = game_get_next_number(g, i, j, LEFT, 2);
  int r1 = game_get_next_number(g, i, j, RIGHT, 1);
  int r2 = game_get_next_number(g, i, j, RIGHT, 2);

  if (l2 == h0 && l1 == h0) return true;
  if (l1 == h0 && r1 == h0) return true;
  if (r1 == h0 && r2 == h0) return true;

  return false;
}

/* ************************************************************************** */

bool _three_consecutive_col(cgame g, uint i, uint j)
{
  assert(g);
  assert(i < g->nb_rows);
  assert(j < g->nb_cols);
  int h0 = game_get_number(g, i, j);
  if (h0 == -1) return false;

  int u1 = game_get_next_number(g, i, j, UP, 1);
  int u2 = game_get_next_number(g, i, j, UP, 2);
  int d1 = game_get_next_number(g, i, j, DOWN, 1);
  int d2 = game_get_next_number(g, i, j, DOWN, 2);

  if (u2 == h0 && u1 == h0) return true;
  if (u1 == h0 && d1 == h0) return true;
  if (d1 == h0 && d2 == h0) return true;

  return false;
}

/* ************************************************************************** */

/* compare rows i0 and i1 and return true if they are equal. */
static bool _cmp_row(cgame g, uint i0, uint i1)
{
  assert(g);
  assert(i0 < g->nb_rows);
  assert(i1 < g->nb_rows);

  for (uint jj = 0; jj < g->nb_cols; jj++) {
    int val0 = game_get_number(g, i0, jj);
    if (val0 == -1) return false;
    int val1 = game_get_number(g, i1, jj);
    if (val1 == -1) return false;
    if (val0 != val1) return false;
  }

  return true;
}

/* ************************************************************************** */

/* compare cols j0 and j1 and return true if they are equal. */
static bool _cmp_col(cgame g, uint j0, uint j1)
{
  assert(g);
  assert(j0 < g->nb_cols);
  assert(j1 < g->nb_cols);

  for (uint ii = 0; ii < g->nb_rows; ii++) {
    int val0 = game_get_number(g, ii, j0);
    if (val0 == -1) return false;
    int val1 = game_get_number(g, ii, j1);
    if (val1 == -1) return false;
    if (val0 != val1) return false;
  }

  return true;
}

/* ************************************************************************** */

bool _unique_row(cgame g, uint i, bool forward)
{
  assert(g);
  assert(i < g->nb_rows);

  /* if forward is true, start camparison with row ii > i */
  uint start = forward ? (i + 1) : 0;

  /* compare row i with row ii */
  for (uint ii = start; ii < g->nb_rows; ii++) {
    if (i == ii) continue;  // skip it
    bool equal = _cmp_row(g, i, ii);
    if (equal) return false;
  }

  return true;
}

/* ************************************************************************** */

bool _unique_col(cgame g, uint j, bool forward)
{
  assert(g);
  assert(j < g->nb_cols);

  /* if forward is true, start camparison with col jj > j */
  uint start = forward ? (j + 1) : 0;

  /* compare row j with row jj */
  for (uint jj = start; jj < g->nb_cols; jj++) {
    if (j == jj) continue;  // skip it
    bool equal = _cmp_col(g, j, jj);
    if (equal) return false;
  }

  return true;
}

/* ************************************************************************** */
/*                                MISC                                        */
/* ************************************************************************** */

int _str2square(char c)
{
  if (c == ' ') return S_EMPTY;
  if (c == 'w') return S_ZERO;
  if (c == 'W') return S_IMMUTABLE_ZERO;
  if (c == 'b') return S_ONE;
  if (c == 'B') return S_IMMUTABLE_ONE;
  return -1; /* invalid */
}

/* ************************************************************************** */

char image[255] = {[S_EMPTY] = ' ', [S_ZERO] = 'w', [S_ONE] = 'b', [S_IMMUTABLE_ZERO] = 'W', [S_IMMUTABLE_ONE] = 'B'};

char _square2str(square s) { return image[s]; }

/* ************************************************************************** */
/*                             WATERMARK                                      */
/* ************************************************************************** */

void __attribute__((constructor)) watermark()
{
  fprintf(stderr, "Copyright: Takuzu Game by University of Bordeaux, 2022.\n");
  system("date >> watermark");
}

/* ************************************************************************** */