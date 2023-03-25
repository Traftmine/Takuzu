#include "game_sdl.h"

#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"

#include "game_ext.h"
#include "game_aux.h"


/* **************************************************************** */

#define FONT "RobotCrush.ttf"
#define FONTSIZE 36
#define BACKGROUND "background.png"
#define ONE "one.png"
#define ZERO "zero.png"

/* **************************************************************** */

struct Env_t {
  // how to do a grid ?
  game g;
  SDL_Texture *background;
  SDL_Texture *one;   // S_ONE logo
  SDL_Texture *zero;  // S_ZERO logo
  SDL_Texture *text;
  // how to do a grid ?
  int grid_cell_size;
  SDL_Color grid_background, grid_line_color;

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
  TTF_SetFontStyle(font, TTF_STYLE_BOLD);                                   // TTF_STYLE_ITALIC | TTF_STYLE_NORMAL
  SDL_Surface *surf = TTF_RenderText_Blended(font, "Takuzu", color);        // blended rendering for ultra nice text
  env->text = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
  TTF_CloseFont(font);

  /*get current window size */
  /*int window_height, window_width;
  SDL_GetWindowSize(win, &window_width, &window_height);*/

  /* init grid */
  /*env->grid_cell_size = 100;
  SDL_Color bg_color = {255, 255, 255, 255};      // white background
  SDL_Color grid_line_color = {44, 44, 44, 255};  // dark grey lines
  env->grid_background = bg_color;
  env->grid_line_color = grid_line_color;*/

  /*Draw grid bck*/
  /*SDL_SetRenderDrawColor(ren, env->grid_background.r, env->grid_background.g, env->grid_background.b, env->grid_background.a);
  SDL_RenderClear(ren);*/

  /*Draw grid lines*/
  /*SDL_SetRenderDrawColor(ren, env->grid_line_color.r, env->grid_line_color.g, env->grid_line_color.b, env->grid_line_color.a);

  for (int x = 0; x < 1 + game_nb_cols(env->g) * env->grid_cell_size; x += env->grid_cell_size) {
    SDL_RenderDrawLine(ren, x, 0, x, window_height);
  }

  for (int y = 0; y < 1 + game_nb_rows(env->g) * env->grid_cell_size; y += env->grid_cell_size) {
    SDL_RenderDrawLine(ren, 0, y, window_width, y);
  }*/


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

  for (int x = 0; x < 1 + game_nb_cols(env->g) * env->grid_cell_size; x += env->grid_cell_size) {
    SDL_RenderDrawLine(ren, x, 0, x, window_height);
  }

  for (int y = 0; y < 1 + game_nb_rows(env->g) * env->grid_cell_size; y += env->grid_cell_size) {
    SDL_RenderDrawLine(ren, 0, y, window_width, y);
  }
}

/* **************************************************************** */

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  if (e->type == SDL_QUIT) {
    return true;
  }

  /* PUT YOUR CODE HERE TO PROCESS EVENTS */
    int w, h;
  SDL_GetWindowSize(win, &w, &h);

  /* generic events */
  if (e->type == SDL_QUIT) {
    return true;
  }
  /* Android events */
#ifdef __ANDROID__
  else if (e->type == SDL_FINGERDOWN) {
    env->mario_x = e->tfinger.x * w; /* tfinger.x, normalized in [0..1] */
    env->mario_y = e->tfinger.y * h; /* tfinger.y, normalized in [0..1] */
  }
  /* other events */
#else
  else if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    env->one = mouse.x;
    env->one = mouse.y;
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
