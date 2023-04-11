/**
 * @file game_aux.c
 * @copyright University of Bordeaux. All rights reserved, 2022.
 **/

#include "game_aux.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_ext.h"
#include "game_private.h"
#include "game_struct.h"

/* ************************************************************************** */

#define SE S_EMPTY
#define S0 S_ZERO
#define S1 S_ONE
#define SI0 S_IMMUTABLE_ZERO
#define SI1 S_IMMUTABLE_ONE

/* ************************************************************************** */

static square grid_default[] = {
    SE, SI1, SI0, SE, SE,  SE,  /* row 0 */
    SE, SE,  SE,  SE, SE,  SE,  /* row 1 */
    SE, SI0, SE,  SE, SI0, SE,  /* row 2 */
    SE, SI0, SI1, SE, SE,  SE,  /* row 3 */
    SE, SE,  SI1, SE, SE,  SI0, /* row 4 */
    SE, SE,  SE,  SE, SE,  SI0, /* row 5 */
};

/* ************************************************************************** */

static square grid_default_sol[] = {
    S0, SI1, SI0, S1, S0,  S1,  /* row 0 */
    S0, S1,  S1,  S0, S1,  S0,  /* row 1 */
    S1, SI0, S0,  S1, SI0, S1,  /* row 2 */
    S1, SI0, SI1, S0, S0,  S1,  /* row 3 */
    S0, S1,  SI1, S0, S1,  SI0, /* row 4 */
    S1, S0,  S0,  S1, S1,  SI0, /* row 5 */
};

/* ************************************************************************** */

game game_default(void) { return game_new(grid_default); }

/* ************************************************************************** */

game game_default_solution(void) { return game_new(grid_default_sol); }

/* ************************************************************************** */

void game_print(cgame g)
{
  assert(g);
  printf("   ");
  for (uint j = 0; j < game_nb_cols(g); j++) printf("%d", j);
  printf("\n");
  printf("   ");
  for (uint j = 0; j < game_nb_cols(g); j++) printf("-");
  printf("\n");
  for (uint i = 0; i < game_nb_rows(g); i++) {
    printf("%d |", i);
    for (uint j = 0; j < game_nb_cols(g); j++) {
      square s = game_get_square(g, i, j);
      char c = _square2str(s);
      printf("%c", c);
    }
    printf("|\n");
  }
  printf("   ");
  for (uint j = 0; j < game_nb_cols(g); j++) printf("-");
  printf("\n");
}

/* ************************************************************************** */
