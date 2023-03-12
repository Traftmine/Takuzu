#include "model.h"

#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* **************************************************************** */

#define FONT "RobotCrush.ttf"
#define FONTSIZE 36
#define BACKGROUND "background.png"
#define ONE "one.png"
#define ZERO "zero.png"

/* **************************************************************** */

struct Env_t {
  SDL_Texture* background;
  SDL_Texture* one;         // S_ONE logo
  SDL_Texture* zero;        // S_ZERO logo
  // how to do a grid ?
};

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));

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
  SDL_Color color = {0, 0, 0, 0.8}; /* light balck color in RGBA */
  TTF_Font* font = TTF_OpenFont(FONT, FONTSIZE);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font, TTF_STYLE_BOLD);                                   // TTF_STYLE_ITALIC | TTF_STYLE_NORMAL
  SDL_Surface* surf = TTF_RenderText_Blended(font, "Hello World!", color);  // blended rendering for ultra nice text
  env->text = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
  TTF_CloseFont(font);

  return env;
}

/* **************************************************************** */

void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
    /* PUT YOUR CODE HERE TO RENDER TEXTURES, ... */
}

/* **************************************************************** */

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  if (e->type == SDL_QUIT) {
    return true;
  }

  /* PUT YOUR CODE HERE TO PROCESS EVENTS */

  return false;
}

/* **************************************************************** */

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  /* PUT YOUR CODE HERE TO CLEAN MEMORY */

  free(env);
}

/* **************************************************************** */
