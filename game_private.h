/**
 * @file game_private.h
 * @brief Private Game Functions.
 * @copyright University of Bordeaux. All rights reserved, 2022.
 **/

#ifndef __GAME_PRIVATE_H__
#define __GAME_PRIVATE_H__

#include <stdbool.h>

#include "game.h"
#include "game_struct.h"
#include "queue.h"

/* ************************************************************************** */
/*                             Water Mark                                     */
/* ************************************************************************** */

extern const int __PT2_UBX_TEACHER__;

/* ************************************************************************** */
/*                             DATA TYPES                                     */
/* ************************************************************************** */

/**
 * @brief Move structure.
 * @details This structure is used to save the game history.
 */
struct move_s {
  uint i, j;
  square old, new;
};

typedef struct move_s move;

/* ************************************************************************** */
/*                                MACRO                                       */
/* ************************************************************************** */

#define MAX(x, y) ((x > (y)) ? (x) : (y))

/* ************************************************************************** */
/*                             STACK ROUTINES                                 */
/* ************************************************************************** */

/** push a move in the stack */
void _stack_push_move(queue* q, move m);

/** pop a move from the stack */
move _stack_pop_move(queue* q);

/** test if the stack is empty */
bool _stack_is_empty(queue* q);

/** clear all the stack */
void _stack_clear(queue* q);

/* ************************************************************************** */
/*                                  AUXILIARY                                 */
/* ************************************************************************** */

int _count_in_row(cgame g, int nb, uint i);
int _count_in_col(cgame g, int nb, uint j);
bool _three_consecutive_row(cgame g, uint i, uint j);
bool _three_consecutive_col(cgame g, uint i, uint j);
bool _unique_row(cgame g, uint i, bool forward);
bool _unique_col(cgame g, uint j, bool forward);

/* ************************************************************************** */
/*                                MISC                                        */
/* ************************************************************************** */

/** convert a square into its char representation */
char _square2str(square s);

/** convert a char into its square representation */
int _str2square(char c);

#endif  // __GAME_PRIVATE_H__
