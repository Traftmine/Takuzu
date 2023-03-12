#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

int main(int argc, char *argv[]) {
  // Creating and printing the game
  game g;
  if (argc == 2) {
    g = game_load(argv[1]);
  } else if (argc == 1) {
    g = game_default();
  } else {
    printf(
        "Error: Please either only pass <./game_text> as your argument or "
        "add a valid filename exp: $./game_text default.txt");
  }
  game_print(g);
  printf("> type h for help\n");
  while (game_is_over(g) != true) {
    // Printing the error's list
    for (int i = 0; i < game_nb_cols(g); i++) {
      for (int j = 0; j < game_nb_rows(g); j++) {
        if (game_has_error(g, i, j) != 0) {
          printf("Il y a une erreur en (%d,%d)\n", i, j);
        }
      }
    }
    // Reading player's will
    char c;
    scanf("%c", &c);

    if (c == 'h') {
      printf(
          "\n---------------------\n> r to restart\n> y to undo your "
          "move\n> z to redo your move\n> s <filename> to "
          "save\n---------------------\nVous devez "
          "jouer des coups dans des "
          "cases de coordonées (i,j) ; vous avez le choix entre b (black), "
          "w (white) ou e (vide)\nLorsque vous écrivez vous devez indiquer "
          "w, b ou e puis les "
          "coordonées ainsi 'w 1 2' par exemple\n---------------------\n");
    } else if (c == 'r') {
      game_restart(g);
      game_print(g);

    } else if (c == 'q') {
      printf("shame\n");
      game_delete(g);
      return EXIT_SUCCESS;

    } else if (c == 'z') {
      printf("You've redone your last move\n");
      game_redo(g);
      game_print(g);

    } else if (c == 'y') {
      printf("You've undone your last move\n");
      game_undo(g);
      game_print(g);

    } else if (c == 's') {
      scanf("%s", &c);
      printf("You've saved your game as : %s\n", &c);
      game_save(g, &c);

    } else if (c == 'b' || c == 'w' || c == 'e') {
      // Actually playing
      int i = 0, j = 0;
      scanf("%d %d", &i, &j);
      if (c == 'w') {
        if (game_check_move(g, i, j, S_ZERO)) {
          game_play_move(g, i, j, S_ZERO);
          game_print(g);
        }
      } else if (c == 'b') {
        if (game_check_move(g, i, j, S_ONE)) {
          game_play_move(g, i, j, S_ONE);
          game_print(g);
        }
      } else if (c == 'e') {
        if (game_check_move(g, i, j, S_EMPTY)) {
          game_play_move(g, i, j, S_EMPTY);
          game_print(g);
        }
      } else {
        printf("Your move is impossible, try a new one\n\n");
      }
      printf("> type h for help\n");
    }
  }
  printf("congratulation\n");
  game_print(g);
  return EXIT_SUCCESS;
}
