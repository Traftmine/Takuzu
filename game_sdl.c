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

/* **************************************************************** */

#define FONT "RobotCrush.ttf"
#define FONTSIZE 36
#define BACKGROUND "background.png"
#define ONE "one.png"
#define ZERO "zero.png"

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
};

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));
  env->g = game_default();

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
          SDL_RenderCopy(ren, env->zero, NULL, &rect);
          break;
        case 1:
          SDL_RenderCopy(ren, env->one, NULL, &rect);
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
      case SDLK_s:
        game_restart(env->g);
        break;
      case SDLK_c:
        if (game_is_over(env->g)) {
          // display victory
        } else {
          // display try again
        }
        break;
      case SDLK_o:
        // change the window to a new one
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
