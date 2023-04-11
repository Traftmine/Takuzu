#include "game_sdl.h"

#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

/* **************************************************************** */

#define FONT "RobotCrush.ttf"
#define FONTSIZE 36
#define BACKGROUND "background.png"
#define ONE "one.png"
#define ZERO "zero.png"

/* **************************************************************** */

/* Veuillez m'excuser pour la redondance de code svp, c'était pas évident de faire autant de boutons */

/* **************************************************************** */
struct Env_t {
  game g;
  SDL_Texture *background;
  SDL_Texture *one;   // S_ONE logo
  SDL_Texture *zero;  // S_ZERO logo
  SDL_Texture *text;
  int grid_cell_size_x;
  int grid_cell_size_y;
  SDL_Color grid_background, grid_line_color;
  int one_x, one_y;
  int zero_x, zero_y;
  int grid_rows, grid_cols;
  int unique, wrapping;
};

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));
  env->g = game_default();

  /* init rows and cols*/
  env->grid_rows = game_nb_rows(env->g);
  env->grid_cols = game_nb_cols(env->g);

  /* init unique and wrapping */
  env->unique = game_is_unique(env->g);
  env->wrapping = game_is_wrapping(env->g);

  /* init background texture from PNG image */
  env->background = IMG_LoadTexture(ren, BACKGROUND);
  if (!env->background) ERROR("IMG_LoadTexture: %s\n", BACKGROUND);

  /* init S_ONE texture from PNG image */
  env->one = IMG_LoadTexture(ren, ONE);
  if (!env->one) ERROR("IMG_LoadTexture: %s\n", ONE);

  /* init S_ZERO texture from PNG image */
  env->zero = IMG_LoadTexture(ren, ZERO);
  if (!env->zero) ERROR("IMG_LoadTexture: %s\n", ZERO);

  /* init text texture using Arial font */
  SDL_Color color = {0, 0, 0, 1}; /* light balck color in RGBA */
  TTF_Font *font = TTF_OpenFont(FONT, FONTSIZE);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font, TTF_STYLE_BOLD);                             // TTF_STYLE_ITALIC | TTF_STYLE_NORMAL
  SDL_Surface *surf = TTF_RenderText_Blended(font, "Takuzu", color);  // blended rendering for ultra nice text
  env->text = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
  TTF_CloseFont(font);

  /*get current window size */
  int window_height, window_width;
  SDL_GetWindowSize(win, &window_width, &window_height);

  /* init grid */
  env->grid_cell_size = 100;
  SDL_Color bg_color = {255, 255, 255, 255};      // white background
  SDL_Color grid_line_color = {44, 44, 44, 255};  // dark grey lines
  env->grid_background = bg_color;
  env->grid_line_color = grid_line_color;

  /*Draw grid bck*/
  SDL_SetRenderDrawColor(ren, env->grid_background.r, env->grid_background.g, env->grid_background.b, env->grid_background.a);
  SDL_RenderClear(ren);

  /*Draw grid lines*/
  SDL_SetRenderDrawColor(ren, env->grid_line_color.r, env->grid_line_color.g, env->grid_line_color.b, env->grid_line_color.a);

  for (int x = 0; x < 1 + env->g->nb_cols * env->grid_cell_size; x += env->grid_cell_size) {
    SDL_RenderDrawLine(ren, x, 0, x, window_height);
  }

  for (int y = 0; y < 1 + env->g->nb_rows * env->grid_cell_size; y += env->grid_cell_size) {
    SDL_RenderDrawLine(ren, 0, y, window_width, y);
  }

  return env;
}

/* **************************************************************** */

void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  SDL_Rect rect;

  /* get current window size */
  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  /* render background texture */
  SDL_RenderCopy(ren, env->background, NULL, NULL); /* stretch it */

  /* render text texture */
  SDL_QueryTexture(env->text, NULL, NULL, &rect.w, &rect.h);
  rect.x = w / 2 - rect.w / 2;
  rect.y = h / 2 - rect.h / 2;
  SDL_RenderCopy(ren, env->text, NULL, &rect);

  /*get current window size */
  int window_height, window_width;
  SDL_GetWindowSize(win, &window_width, &window_height);

  /* init grid */
  int begin_x = 0.3 * window_width, begin_y = 0.15 * window_height;
  int end_x = 0.7 * window_width, end_y = 0.55 * window_height;
  env->grid_cell_size_x = (end_x - begin_x) / game_nb_cols(env->g);
  env->grid_cell_size_y = (end_y - begin_y) / game_nb_rows(env->g);
  SDL_Color bg_color = {255, 255, 255, 255};      // white background
  SDL_Color grid_line_color = {44, 44, 44, 255};  // dark grey lines
  env->grid_background = bg_color;
  env->grid_line_color = grid_line_color;

  /*Draw grid bck*/
  SDL_SetRenderDrawColor(ren, env->grid_background.r, env->grid_background.g, env->grid_background.b, env->grid_background.a);
  SDL_RenderClear(ren);

  /*Draw grid lines*/
  SDL_SetRenderDrawColor(ren, env->grid_line_color.r, env->grid_line_color.g, env->grid_line_color.b, env->grid_line_color.a);
  for (int x = begin_x; x <= end_x; x += env->grid_cell_size_x) {
    SDL_RenderDrawLine(ren, x, begin_y, x, end_y);
  }
  for (int y = begin_y; y <= end_y; y += env->grid_cell_size_y) {
    SDL_RenderDrawLine(ren, begin_x, y, end_x, y);
  }

  /* Render game */
  for (int i = 0; i < game_nb_rows(env->g); i++) {
    for (int j = 0; j < game_nb_cols(env->g); j++) {
      SDL_Rect rect;
      rect.w = env->grid_cell_size_x;
      rect.h = env->grid_cell_size_y;
      rect.x = begin_x + j * env->grid_cell_size_x;
      rect.y = begin_y + i * env->grid_cell_size_y;

      switch (game_get_number(env->g, i, j)) {
        case -1:
          break;
        case 0:
          if (game_has_error(env->g, i, j)) {
            // Set the drawing color to red
            SDL_SetRenderDrawColor(ren, 255, 153, 153, 255);
            // Fill the rectangle with the current drawing color
            SDL_RenderFillRect(ren, &rect);
            SDL_RenderCopy(ren, env->zero, NULL, &rect);
          } else {
            SDL_RenderCopy(ren, env->zero, NULL, &rect);
          }
          break;
        case 1:
          if (game_has_error(env->g, i, j)) {
            // Set the drawing color to red
            SDL_SetRenderDrawColor(ren, 255, 153, 153, 255);
            // Fill the rectangle with the current drawing color
            SDL_RenderFillRect(ren, &rect);
            SDL_RenderCopy(ren, env->one, NULL, &rect);
          } else {
            SDL_RenderCopy(ren, env->one, NULL, &rect);
          }
          break;
      }
    }
  }
}

/* **************************************************************** */

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  if (e->type == SDL_QUIT) {
    return true;
  }

  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  SDL_Window *window_option = NULL;

  /* generic events */
  if (e->type == SDL_QUIT) {
    return true;
  }
  /* Android events */
#ifdef __ANDROID__
  // no need to code it for the computer version
  else if (e->type == SDL_FINGERDOWN) {
    env->mario_x = e->tfinger.x * w; /* tfinger.x, normalized in [0..1] */
    env->mario_y = e->tfinger.y * h; /* tfinger.y, normalized in [0..1] */
  }
  /* other events */
#else
  else if (e->type == SDL_MOUSEBUTTONDOWN) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    int row = (y - 0.15 * h) / env->grid_cell_size_y;
    int col = (x - 0.3 * w) / env->grid_cell_size_x;
    if (row >= 0 && row < game_nb_rows(env->g) && col >= 0 && col < game_nb_cols(env->g) && !game_is_immutable(env->g, row, col)) {
      if (game_get_square(env->g, row, col) == S_ONE) {  // 1 -> 0 -> Empty -> 1
        game_play_move(env->g, row, col, S_ZERO);
      } else if (game_get_square(env->g, row, col) == S_ZERO) {
        game_play_move(env->g, row, col, S_EMPTY);
      } else {
        game_play_move(env->g, row, col, S_ONE);
      }
    }
  } else if (e->type == SDL_KEYDOWN) {
    switch (e->key.keysym.sym) {
      case SDLK_u:
        game_undo(env->g);
        break;
      case SDLK_r:
        game_redo(env->g);
        break;
      case SDLK_p:
        game_restart(env->g);
        break;
      case SDLK_c:
        if (game_is_over(env->g)) {
          // display victory
          TTF_Init();
          TTF_Font *font = TTF_OpenFont(FONT, FONTSIZE);
          SDL_Color color = {0, 255, 0};
          SDL_Surface *text_surface = TTF_RenderText_Solid(font, "VICTORY", color);
          SDL_Texture *text_texture = SDL_CreateTextureFromSurface(ren, text_surface);
          int x = (w - text_surface->w) / 2, y = (h - text_surface->h) / 3;
          SDL_Rect text_rect = {x, y, text_surface->w, text_surface->h};
          SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
          SDL_RenderClear(ren);
          SDL_RenderCopy(ren, text_texture, NULL, &text_rect);
          SDL_RenderPresent(ren);
          SDL_Delay(500);
          SDL_FreeSurface(text_surface);
          TTF_CloseFont(font);
          SDL_DestroyTexture(text_texture);
          SDL_Delay(50);
        } else {
          // display try again and fade away text
          TTF_Init();
          TTF_Font *font = TTF_OpenFont(FONT, FONTSIZE);
          SDL_Color color = {255, 0, 0};
          SDL_Surface *text_surface = TTF_RenderText_Solid(font, "TRY AGAIN", color);
          SDL_Texture *text_texture = SDL_CreateTextureFromSurface(ren, text_surface);
          int x = (w - text_surface->w) / 2, y = (h - text_surface->h) / 3;
          SDL_Rect text_rect = {x, y, text_surface->w, text_surface->h};
          SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
          int alpha = 255;
          for (int i = 0; i < 50; i++) {  // fade out over 50 frames
            SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
            SDL_RenderClear(ren);
            SDL_SetTextureAlphaMod(text_texture, alpha);
            SDL_RenderCopy(ren, text_texture, NULL, &text_rect);
            SDL_RenderPresent(ren);
            SDL_Delay(50);  // delay for smoother animation
            alpha -= 5;     // decrease alpha by 5 each iteration
          }
          SDL_FreeSurface(text_surface);
          TTF_CloseFont(font);
          SDL_DestroyTexture(text_texture);
        }
        break;
      case SDLK_o:
        // Change the window to a new one
        window_option = SDL_CreateWindow("Options", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, SDL_WINDOW_SHOWN);
        if (window_option == NULL) {
          SDL_Log("Erreur lors de la création de la fenêtre : %s", SDL_GetError());
          return 1;
        }
        SDL_Renderer *renderer_option = SDL_CreateRenderer(window_option, -1, SDL_RENDERER_ACCELERATED);
        if (renderer_option == NULL) {
          SDL_Log("Erreur lors de la création du renderer : %s", SDL_GetError());
          return 1;
        }

        // Define the switch button rectangle
        SDL_Rect switch_rect1 = {475, 100, 50, 20};
        SDL_Rect switch_rect2 = {475, 200, 50, 20};
        // Cols
        SDL_Rect switch_rect2_Cols = {75, 300, 50, 20};
        SDL_Rect switch_rect4_Cols = {175, 300, 50, 20};
        SDL_Rect switch_rect6_Cols = {275, 300, 50, 20};
        SDL_Rect switch_rect8_Cols = {375, 300, 50, 20};
        SDL_Rect switch_rect10_Cols = {475, 300, 50, 20};
        // Rows
        SDL_Rect switch_rect2_Rows = {75, 400, 50, 20};
        SDL_Rect switch_rect4_Rows = {175, 400, 50, 20};
        SDL_Rect switch_rect6_Rows = {275, 400, 50, 20};
        SDL_Rect switch_rect8_Rows = {375, 400, 50, 20};
        SDL_Rect switch_rect10_Rows = {475, 400, 50, 20};
        // Default game button
        SDL_Rect Default_game = {275, 500, 50, 20};
        // Button state
        int switch_state1 = 0;  // default to off / false state
        int switch_state2 = 0;
        int switch_state_default_game = 0;
        int switch_state2_Cols = 0, switch_state4_Cols = 0, switch_state6_Cols = 0, switch_state8_Cols = 0, switch_state10_Cols = 0;
        int switch_state2_Rows = 0, switch_state4_Rows = 0, switch_state6_Rows = 0, switch_state8_Rows = 0, switch_state10_Rows = 0;

        while (1) {
          SDL_Event event_option;
          while (SDL_PollEvent(&event_option)) {
            if (event_option.type == SDL_QUIT) {
              SDL_DestroyRenderer(renderer_option);
              SDL_DestroyWindow(window_option);
              return 0;
            }
            if (event_option.type == SDL_KEYDOWN && event_option.key.keysym.sym == SDLK_q) {
              SDL_DestroyRenderer(renderer_option);
              SDL_DestroyWindow(window_option);
              return 0;
            }
            if (event_option.type == SDL_MOUSEBUTTONUP && event_option.button.button == SDL_BUTTON_LEFT) {
              // check if the mouse click is inside a switch button
              int mouse_x = event_option.button.x;
              int mouse_y = event_option.button.y;
              if (mouse_x >= switch_rect1.x && mouse_x <= switch_rect1.x + switch_rect1.w && mouse_y >= switch_rect1.y &&
                  mouse_y <= switch_rect1.y + switch_rect1.h) {
                switch_state1 = !switch_state1;  // toggle the switch state
                env->unique = switch_state1;
                env->g = game_new_empty_ext(env->grid_rows, env->grid_cols, env->wrapping, env->unique);
              }
              if (mouse_x >= switch_rect2.x && mouse_x <= switch_rect2.x + switch_rect2.w && mouse_y >= switch_rect2.y &&
                  mouse_y <= switch_rect2.y + switch_rect2.h) {
                switch_state2 = !switch_state2;
                env->wrapping = switch_state2;
                env->g = game_new_empty_ext(env->grid_rows, env->grid_cols, env->wrapping, env->unique);
              }
              // Default game button
              if (mouse_x >= Default_game.x && mouse_x <= Default_game.x + Default_game.w && mouse_y >= Default_game.y &&
                  mouse_y <= Default_game.y + Default_game.h) {
                switch_state_default_game = !switch_state_default_game;
                env->g = game_default();
              }
              // Buttons Cols
              if (mouse_x >= switch_rect2_Cols.x && mouse_x <= switch_rect2_Cols.x + switch_rect2_Cols.w && mouse_y >= switch_rect2_Cols.y &&
                  mouse_y <= switch_rect2_Cols.y + switch_rect2_Cols.h) {
                env->grid_cols = 2;  // setting col size
                switch_state2_Cols = !switch_state2_Cols;
                switch_state4_Cols = 0, switch_state6_Cols = 0, switch_state8_Cols = 0, switch_state10_Cols = 0;
                env->g = game_new_empty_ext(env->grid_rows, env->grid_cols, env->wrapping, env->unique);
              }
              if (mouse_x >= switch_rect4_Cols.x && mouse_x <= switch_rect4_Cols.x + switch_rect4_Cols.w && mouse_y >= switch_rect4_Cols.y &&
                  mouse_y <= switch_rect4_Cols.y + switch_rect4_Cols.h) {
                env->grid_cols = 4;  // setting col size
                switch_state4_Cols = !switch_state4_Cols;
                switch_state2_Cols = 0, switch_state6_Cols = 0, switch_state8_Cols = 0, switch_state10_Cols = 0;
                env->g = game_new_empty_ext(env->grid_rows, env->grid_cols, env->wrapping, env->unique);
              }
              if (mouse_x >= switch_rect6_Cols.x && mouse_x <= switch_rect6_Cols.x + switch_rect6_Cols.w && mouse_y >= switch_rect6_Cols.y &&
                  mouse_y <= switch_rect6_Cols.y + switch_rect6_Cols.h) {
                env->grid_cols = 6;  // setting col size
                switch_state6_Cols = !switch_state6_Cols;
                switch_state2_Cols = 0, switch_state4_Cols = 0, switch_state8_Cols = 0, switch_state10_Cols = 0;
                env->g = game_new_empty_ext(env->grid_rows, env->grid_cols, env->wrapping, env->unique);
              }
              if (mouse_x >= switch_rect8_Cols.x && mouse_x <= switch_rect8_Cols.x + switch_rect8_Cols.w && mouse_y >= switch_rect8_Cols.y &&
                  mouse_y <= switch_rect8_Cols.y + switch_rect8_Cols.h) {
                env->grid_cols = 8;  // setting col size
                switch_state8_Cols = !switch_state8_Cols;
                switch_state2_Cols = 0, switch_state4_Cols = 0, switch_state6_Cols = 0, switch_state10_Cols = 0;
                env->g = game_new_empty_ext(env->grid_rows, env->grid_cols, env->wrapping, env->unique);
              }
              if (mouse_x >= switch_rect10_Cols.x && mouse_x <= switch_rect10_Cols.x + switch_rect10_Cols.w && mouse_y >= switch_rect10_Cols.y &&
                  mouse_y <= switch_rect10_Cols.y + switch_rect10_Cols.h) {
                env->grid_cols = 10;  // setting col size
                switch_state10_Cols = !switch_state10_Cols;
                switch_state2_Cols = 0, switch_state4_Cols = 0, switch_state6_Cols = 0, switch_state8_Cols = 0;
                env->g = game_new_empty_ext(env->grid_rows, env->grid_cols, env->wrapping, env->unique);
              }
              // Buttons Rows
              if (mouse_x >= switch_rect2_Rows.x && mouse_x <= switch_rect2_Rows.x + switch_rect2_Rows.w && mouse_y >= switch_rect2_Rows.y &&
                  mouse_y <= switch_rect2_Rows.y + switch_rect2_Rows.h) {
                switch_state2_Rows = !switch_state2_Rows;
                switch_state4_Rows = 0, switch_state6_Rows = 0, switch_state8_Rows = 0, switch_state10_Rows = 0;
                env->grid_rows = 2;  // setting col size
                env->g = game_new_empty_ext(env->grid_rows, env->grid_cols, env->wrapping, env->unique);
              }
              if (mouse_x >= switch_rect4_Rows.x && mouse_x <= switch_rect4_Rows.x + switch_rect4_Rows.w && mouse_y >= switch_rect4_Rows.y &&
                  mouse_y <= switch_rect4_Rows.y + switch_rect4_Rows.h) {
                switch_state4_Rows = !switch_state4_Rows;
                switch_state2_Rows = 0, switch_state6_Rows = 0, switch_state8_Rows = 0, switch_state10_Rows = 0;
                env->grid_rows = 4;  // setting col size
                env->g = game_new_empty_ext(env->grid_rows, env->grid_cols, env->wrapping, env->unique);
              }
              if (mouse_x >= switch_rect6_Rows.x && mouse_x <= switch_rect6_Rows.x + switch_rect6_Rows.w && mouse_y >= switch_rect6_Rows.y &&
                  mouse_y <= switch_rect6_Rows.y + switch_rect6_Rows.h) {
                switch_state6_Rows = !switch_state6_Rows;
                switch_state2_Rows = 0, switch_state4_Rows = 0, switch_state8_Rows = 0, switch_state10_Rows = 0;
                env->grid_rows = 6;  // setting col size
                env->g = game_new_empty_ext(env->grid_rows, env->grid_cols, env->wrapping, env->unique);
              }
              if (mouse_x >= switch_rect8_Rows.x && mouse_x <= switch_rect8_Rows.x + switch_rect8_Rows.w && mouse_y >= switch_rect8_Rows.y &&
                  mouse_y <= switch_rect8_Rows.y + switch_rect8_Rows.h) {
                switch_state8_Rows = !switch_state8_Rows;
                switch_state2_Rows = 0, switch_state4_Rows = 0, switch_state6_Rows = 0, switch_state10_Rows = 0;
                env->grid_rows = 8;  // setting col size
                env->g = game_new_empty_ext(env->grid_rows, env->grid_cols, env->wrapping, env->unique);
              }
              if (mouse_x >= switch_rect10_Rows.x && mouse_x <= switch_rect10_Rows.x + switch_rect10_Rows.w && mouse_y >= switch_rect10_Rows.y &&
                  mouse_y <= switch_rect10_Rows.y + switch_rect10_Rows.h) {
                switch_state10_Rows = !switch_state10_Rows;
                switch_state2_Rows = 0, switch_state4_Rows = 0, switch_state6_Rows = 0, switch_state8_Rows = 0;
                env->grid_rows = 10;  // setting col size
                env->g = game_new_empty_ext(env->grid_rows, env->grid_cols, env->wrapping, env->unique);
              }
            }
          }
          // Set the background color to white
          SDL_SetRenderDrawColor(renderer_option, 255, 255, 255, 255);
          // Clear the renderer with the current draw color
          SDL_RenderClear(renderer_option);

          // Draw the switch buttons

          // Button Wrapping
          if (switch_state1) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &switch_rect1);

          // Button Unique
          if (switch_state2) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &switch_rect2);

          // Button Default game
          if (switch_state_default_game) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &Default_game);

          // Button Cols coloration
          if (switch_state2_Cols) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &switch_rect2_Cols);

          if (switch_state4_Cols) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &switch_rect4_Cols);

          if (switch_state6_Cols) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &switch_rect6_Cols);

          if (switch_state8_Cols) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &switch_rect8_Cols);

          if (switch_state10_Cols) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &switch_rect10_Cols);

          // Button Rows coloration

          if (switch_state2_Rows) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &switch_rect2_Rows);

          if (switch_state4_Rows) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &switch_rect4_Rows);

          if (switch_state6_Rows) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &switch_rect6_Rows);

          if (switch_state8_Rows) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &switch_rect8_Rows);

          if (switch_state10_Rows) {
            SDL_SetRenderDrawColor(renderer_option, 26, 166, 196, 1);  // blue color for on state
          } else {
            SDL_SetRenderDrawColor(renderer_option, 0, 0, 0, 1);  // black color for off state
          }
          SDL_RenderFillRect(renderer_option, &switch_rect10_Rows);
          // Update the window with the renderer
          SDL_RenderPresent(renderer_option);
        }
        break;
      case SDLK_h:
        // display help
        TTF_Init();
        TTF_Font *font = TTF_OpenFont(FONT, FONTSIZE);
        SDL_Color color = {255, 255, 255};
        const char *text =
            "> p to restart\n> u to undo your move\n> r to redo your move\n> c to check if victory\n> q to quit\n> s to solve\n> w to save\n> l to "
            "load";
        int maxWidth = w * 0.80;  // maximum width for each line
        SDL_Surface *text_surface = TTF_RenderText_Blended_Wrapped(font, text, color, maxWidth);
        SDL_Texture *text_texture = SDL_CreateTextureFromSurface(ren, text_surface);
        int x = (w - text_surface->w) / 2, y = (h - text_surface->h) / 3;
        SDL_Rect text_rect = {x, y, text_surface->w, text_surface->h};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        int alpha = 255;
        for (int i = 0; i < 50; i++) {  // fade out over 50 frames
          SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
          SDL_RenderClear(ren);
          SDL_SetTextureAlphaMod(text_texture, alpha);
          SDL_RenderCopy(ren, text_texture, NULL, &text_rect);
          SDL_RenderPresent(ren);
          SDL_Delay(150);  // delay for smoother animation
          alpha -= 5;      // decrease alpha by 5 each iteration
        }
        SDL_FreeSurface(text_surface);
        TTF_CloseFont(font);
        SDL_DestroyTexture(text_texture);
        break;
      case SDLK_s:
        env->g = game_default_solution();
        break;
      case SDLK_w:
        // display save
        SDL_SetRenderDrawColor(ren, 50, 205, 50, 255);  // light green color
        SDL_RenderClear(ren);
        TTF_Font *font_save = TTF_OpenFont(FONT, FONTSIZE);
        SDL_Color color_save = {255, 255, 255};  // white color
        SDL_Surface *text_surface_save = TTF_RenderText_Solid(font_save, "saved as game_saved.txt", color_save);
        SDL_Texture *text_texture_save = SDL_CreateTextureFromSurface(ren, text_surface_save);
        int x_save = (w - text_surface_save->w) / 2, y_save = (h - text_surface_save->h) / 3;
        SDL_Rect text_rect_save = {x_save, y_save, text_surface_save->w, text_surface_save->h};
        SDL_RenderCopy(ren, text_texture_save, NULL, &text_rect_save);
        SDL_RenderPresent(ren);
        SDL_Delay(1000);
        game_save(env->g, "game_saved.txt");
        break;
      case SDLK_q:
        return true;
        break;
    }
  }
#endif

  return false; /* don't quit */
}

/* **************************************************************** */

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  SDL_DestroyTexture(env->background);
  SDL_DestroyTexture(env->zero);
  SDL_DestroyTexture(env->one);
  SDL_DestroyTexture(env->text);

  free(env);
}

/* **************************************************************** */
