#include "game_tools.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_ext.h"

game game_load(char* filename) {
  // Opening the file and checking if it was opened
  FILE* f = fopen(filename, "r");
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

void game_save(cgame g, char* filename) {
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  uint i = game_nb_rows(g);
  uint j = game_nb_cols(g);
  FILE* fichier = NULL;
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

void game_nb_solutions_rec(uint i, uint j, game g, uint * counter){

  if(i == game_nb_cols(g) && j == game_nb_rows(g) ){
    if( game_is_over(g) ){
      (*counter) ++;
    }
    return ;
  }
  // first case where the square is 0
  game_play_move(g, i, j, S_ZERO);
  if( i == game_nb_cols(g) ){
    j= j+1;
  }
  game_nb_solutions_rec(i+1, j , g, counter);

  //second case where the sqaure is 1
  game_play_move(g, i, j, S_ONE);
  if( i == game_nb_cols(g) ){
      j= j+1;
    }
  game_nb_solutions_rec(i+1,j, g, counter);
  
}

uint game_nb_solutions(cgame g){
  uint i = 0, j = 0;
  uint counter = 0;
  game g_nb_sols = game_copy(g); // copy the game because it's unchangeable so that I can change it when calling recursive version
  game_nb_solutions_rec(i,j,g_nb_sols,&counter);
  return counter;
}

bool game_solve_rec(uint i, uint j, game g, uint counter){
  if(i == game_nb_cols(g) && j == game_nb_rows(g) ){
    if( game_is_over(g) ){
      return true;
    }
    else{
      if(counter == game_nb_solutions(g)){
        game_restart(g);
        return false;
      }
      else{
      counter++;
      game_restart(g);
      game_solve_rec(0,0,g,counter);
      }
    }
  }
  // first case where the square is 0
  game_play_move(g, i, j, S_ZERO);
  if( i == game_nb_cols(g) ){
    j= j+1;
  }
  game_solve_rec(i+1, j , g,counter);

  //second case where the sqaure is 1
  game_play_move(g, i, j, S_ONE);
  if( i == game_nb_cols(g) ){
      j= j+1;
    }
  game_solve_rec(i+1,j, g,counter);
}

bool game_solve(game g){
  uint i = 0, j = 0, counter = 0;

  if (game_nb_solutions(g) == 0){
    return false;
  }
  else{
    return game_solve_rec(i,j,g,counter);
  } 
}