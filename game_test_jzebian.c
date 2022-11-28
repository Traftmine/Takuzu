#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"

bool test_dummy(){
    return true;
}

bool test_game_new(){
  square* s = (square*) malloc (DEFAULT_SIZE*DEFAULT_SIZE * sizeof ( square ) );
  assert(s);

  game g = game_new(s);

  if(g == NULL ){
    fprintf(stderr,"game_new function did not do it's job: returned a NULL pointer to the game!\n");
    return false;
  }

  for(uint x=0; x < DEFAULT_SIZE ; x++){
    for(uint y=0; y< DEFAULT_SIZE ; y++){
      if(game_get_square(g,x,y) != s[(x* DEFAULT_SIZE) + y] ){
        fprintf(stderr,"game_new function did not set correctly the square of cell (%u,%u)!\n",x,y);
        return false;

      }
    }
  }
  free(s);

  return true;
}

bool test_game_new_empty(){

  game g = game_new_empty();

  if(g == NULL ){
    fprintf(stderr,"game_new_empty function did not do it's job: returned a NULL pointer to the game!\n");
    return false;
  }

  for(uint x = 0 ; x< DEFAULT_SIZE ; x++ ){
    for(uint y = 0; y< DEFAULT_SIZE ; y++){

      if( (game_get_square(g,x,y) != S_EMPTY ) ){
        fprintf(stderr,"game_new_empty function did not initialize an empty game!\n");
        return false;

      }
    }
  }

  return true;
}

bool test_game_copy(){

  cgame cg = game_default();

  if(cg == NULL ){
    fprintf(stderr,"game_new returned a NULL pointer to the game!\n");
    return false;
  }

  game g = game_copy(cg) ;

  if(g == NULL ){
    fprintf(stderr,"game_copy returned a NULL pointer to the game!\n");
    return false;
  }

  if(game_equal(cg,g) == false){
    fprintf(stderr,"something wrong with the function game_copy!\n");
    return false ;
  }

  if(game_get_square(g,3,2)!= S_ONE){
    game_set_square(g,3,2,S_ONE);
    if(game_get_square(cg,3,2)== S_ONE){
      fprintf(stderr,"Error: reuse of the pointer to the game in game_copy!\n");
    return false;
    }
  }
  
  else{
    game_set_square(g,3,2,S_ZERO);
    if(game_get_square(cg,3,2)== S_ZERO){
      fprintf(stderr,"Error: reuse of the pointer to the game in game_copy!\n");
    return false;
    }
  }

  return true;
}

bool test_game_equal(){

  cgame cg1 = game_default();

  game cg2 = game_default();

  if(cg1 == NULL || cg2 == NULL){
    fprintf(stderr,"Error: game_default returned a NULL pointer to the game in functon test_game_equal!\n");
    return false;
  }

  //a problem that can happen: having at least one pointer with a NULL address and game_equal returning true

  if ( ( cg1 == NULL || cg2 == NULL ) && ( game_equal(cg1,cg2) == true ) ){
    fprintf(stderr,"Error: at least one of the 2 game pointers is NULL while the function game_equal returned true!\n") ;
    return false;
  }

  for(uint x = 0 ; x< DEFAULT_SIZE ; x++ ){
    for(uint y = 0; y< DEFAULT_SIZE ; y++){

      if(game_get_number(cg1,x,y) == 0 && game_get_number(cg2,x,y) == 0){
        game_set_square(cg2,x,y,S_ZERO);
      }

      else if(game_get_number(cg1,x,y) == 1 && game_get_number(cg2,x,y) == 1){
        game_set_square(cg2,x,y,S_ONE);
      }
      
      if( (game_get_number(cg1,x,y) == 0 && game_get_square(cg1,x,y) == S_IMMUTABLE_ZERO ) && ( game_get_number(cg2,x,y) == 0 && game_get_square(cg2,x,y) == S_ZERO ) && game_equal(cg1,cg2) == true ){
        fprintf(stderr,"Error: game_equal function returned true while the squares in cell (%u,%u) are different in the 2 games given as params!\n",x,y);
        return false;

      }

      else if( (game_get_number(cg1,x,y) == 1 && game_get_square(cg1,x,y) == S_IMMUTABLE_ONE ) && ( game_get_number(cg2,x,y) == 1 && game_get_square(cg2,x,y) == S_ONE ) && game_equal(cg1,cg2) == true ) {
        fprintf(stderr,"Error: game_equal function returned true while the squares in cell (%u,%u) are different in the 2 games given as params!\n",x,y);
        return false;

      }

    }
  }
  
  return true;
}

bool test_game_delete() {
  square* s = (square *) malloc( DEFAULT_SIZE*DEFAULT_SIZE * sizeof ( square )  );
  assert(s);

  game g = game_new(s);

  if(g == NULL ){
    fprintf(stderr,"Error: game_new pointer is NULL !\n") ;
    return false ;
  }

  game_delete(g);

  free(s);

  return true;
}

bool test_game_set_square(){
  square* p_s = (square *) malloc( DEFAULT_SIZE*DEFAULT_SIZE * sizeof ( square ) );
  assert(p_s);
  

  game g = game_new(p_s);

  for (uint x =0; x<DEFAULT_SIZE ; x++){
    for (uint y = 0; y<DEFAULT_SIZE ; y++){
      
      game_set_square(g,x,y,*p_s);

      if(game_get_square(g,x,y) != *p_s ){
        fprintf(stderr,"Error: game_set_square did not set the cell (%u,%u) at the value of a given square!\n", x, y);
        return false;
      }

    }
  }

  free(p_s);

  return true;
}

bool test_game_print(){
  game g = game_default();
  game_print(g);
  game_delete(g);
  return true;
}


int main(int argc, char* argv[]){
  if (argc != 2)
  {
    fprintf(stderr,"Error: Only the excutable and the test name are required\n");
    fprintf(stderr,"Example: $./game_test_jzebian game_new\n");
    return EXIT_FAILURE ;
  }

  bool ok = false;

  if (strcmp("dummy", argv[1]) == 0){
      ok = test_dummy();
    }
  else if(strcmp("game_new", argv[1]) == 0){
    ok = test_game_new();
  }
  else if(strcmp("game_new_empty", argv[1]) == 0){
    ok = test_game_new_empty();
  }
  else if(strcmp("game_copy", argv[1]) == 0){
    ok = test_game_copy();
  }
  else if (strcmp("game_equal", argv[1]) == 0){
    ok = test_game_equal();
  }
  else if (strcmp("game_delete", argv[1]) == 0){
    ok = test_game_delete();
  }
  else if (strcmp("game_set_square", argv[1]) == 0){
    ok = test_game_set_square();
  }
  else if (strcmp("game_print", argv[1]) == 0){
    ok = test_game_print();
  }
  else{
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    return EXIT_FAILURE;

  }
  if (ok){
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  }
  else{
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}