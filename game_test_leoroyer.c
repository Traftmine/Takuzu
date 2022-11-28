#define __USE_GNU
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"

/* Fonction assert */
#define ASSERT(expr)                                                                  \
  do                                                                                  \
  {                                                                                   \
    if ((expr) == 0)                                                                  \
    {                                                                                 \
      fprintf(stderr, "[%s:%d] Assertion '%s' failed!\n", __FILE__, __LINE__, #expr); \
      abort();                                                                        \
    }                                                                                 \
  } while (0)

int test_dummy()
{
    return true;
}

/* Test si game_sget_square return bien une valeur */

bool test_get_square(void)
{
  game g = game_default();
  if (g == NULL)
  {
    return false;
  }
  int w = 0;
  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 6; j++)
    {
      if (game_get_square(g,i,j) == S_EMPTY ||game_get_square(g,i,j) == S_ZERO ||game_get_square(g,i,j) == S_ONE ||game_get_square(g,i,j) == S_IMMUTABLE_ONE ||game_get_square(g,i,j) == S_IMMUTABLE_ZERO)
      {
        w++;
      }
    }
  }
  if (w == 36)
  {
    return true;
  }
  else{
    return false;
  }
}

/* Test si game est crée */

bool test_default(void)
{
  game g = game_default();
  game g_solution = game_default_solution();
  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 6; j++)
    {
      if (game_is_immutable(g_solution,i,j)==false)
      {
        ASSERT(game_get_square(g,i,j)== S_EMPTY);
      }
      else
      {
        ASSERT(game_get_square(g,i,j)==game_get_square(g_solution,i,j));
      }
    }
  }
  game_delete(g);
  return true;
}

/* vérifie si on a bien un nombre avec game_get_number*/

bool test_get_number(void)
{
  game g = game_new_empty();
    game_set_square(g,0,0,S_EMPTY);
    game_set_square(g,0,1,S_ZERO);
    game_set_square(g,0,2,S_ONE);
    game_set_square(g,0,3,S_IMMUTABLE_ZERO);
    game_set_square(g,0,4,S_IMMUTABLE_ONE);
    ASSERT(game_get_number(g,0,0)==-1);
    ASSERT(game_get_number(g,0,1)==0);
    ASSERT(game_get_number(g,0,2)==1);
    ASSERT(game_get_number(g,0,3)==0);
    ASSERT(game_get_number(g,0,4)==1);
    game_delete(g);
    return true;

}

/* même chose que get square mais on remplace les S_ZERO etc par leur chiffre lors du test à optimiser pcq on test actuellement plusieurs cases identiques*/

/* Test si un square est immutable ou non */

bool test_is_immutable(void)
{
  game g = game_default();

  if (game_is_immutable(g,0,1) == true && game_is_immutable(g,2,1) == true
  && game_is_immutable(g,3,2) == true && game_is_immutable(g,4,5) == true
  && game_is_immutable(g,5,4) == false && game_is_immutable(g,0,0) == false)
  {
    return true;
  }
  else{
    return false;
  }
}

/* Test si une case est vide ou non */

bool test_is_empty(void)
{
  game g = game_new_empty();
  game_set_square(g,0,1,S_EMPTY);
  game_set_square(g,0,2,S_ZERO);
  game_set_square(g,0,3,S_ONE);
  game_set_square(g,0,4,S_IMMUTABLE_ZERO);
  game_set_square(g,0,5,S_IMMUTABLE_ONE);
  ASSERT(game_is_empty(g,0,1)==true);
  ASSERT(!game_is_empty(g,0,2)==true);
  ASSERT(!game_is_empty(g,0,3)==true);
  ASSERT(!game_is_empty(g,0,4)==true);
  ASSERT(!game_is_empty(g,0,5)==true);
  game_delete(g);
  return true;
}

/* test si get next square donne bien le suivant ou -1 si off grid*/
bool test_get_next_square(void)
{
  game g = game_new_empty();
  game_set_square(g,0,1,S_EMPTY);
  game_set_square(g,1,2,S_ZERO);
  game_set_square(g,1,0,S_ONE);
  game_set_square(g,2,1,S_IMMUTABLE_ZERO);
  ASSERT(game_get_next_square(g,1,1,DOWN,1)==S_IMMUTABLE_ZERO);
  ASSERT(game_get_next_square(g,1,1,LEFT,1)==S_ONE);
  ASSERT(game_get_next_square(g,1,1,RIGHT,1)==S_ZERO);
  ASSERT(game_get_next_square(g,1,1,UP,1)==S_EMPTY);
  ASSERT(game_get_next_square(g,1,1,UP,2)==-1);

  ASSERT(game_get_next_square(g,0,1,UP,0)==S_EMPTY);
  game_delete(g);
  return true;
}

/* test si next number donne un nombre */
bool test_get_next_number(void)
{
  game g = game_new_empty();
  game_set_square(g,0,1,S_EMPTY);
  game_set_square(g,1,2,S_ZERO);
  game_set_square(g,1,0,S_ONE);
  game_set_square(g,2,1,S_IMMUTABLE_ZERO);
  ASSERT(game_get_next_number(g,1,1,DOWN,1)==0);
  ASSERT(game_get_next_number(g,1,1,LEFT,1)==1);
  ASSERT(game_get_next_number(g,1,1,RIGHT,1)==0);
  ASSERT(game_get_next_number(g,1,1,UP,1)==-1);
  ASSERT(game_get_next_number(g,1,1,UP,2)==-1);
  ASSERT(game_get_next_number(g,0,1,UP,0)==-1);
  game_delete(g);
  return true;
}

void usage(int argc, char *argv[])
{
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

/* ********** MAIN ROUTINE ********** */

int main(int argc, char *argv[])
{
  if (argc == 1)
    usage(argc, argv);

  // start test
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
  bool test_result = false;
  if (strcmp("dummy", argv[1]) == 0)
    test_result = test_dummy();
  else if (strcmp("game_get_square", argv[1]) == 0)
    test_result = test_get_square();
  else if (strcmp("game_default", argv[1]) == 0)
    test_result = test_default();
  else if (strcmp("game_get_number", argv[1]) == 0)
    test_result = test_get_number();
  else if (strcmp("game_is_immutable", argv[1]) == 0)
    test_result = test_is_immutable();
  else if (strcmp("game_is_empty", argv[1]) == 0)
    test_result = test_is_empty();
  else if (strcmp("game_get_next_square", argv[1]) == 0)
    test_result = test_get_next_square();
  else if (strcmp("game_get_next_number", argv[1]) == 0)
    test_result = test_get_next_number();
  else
  {
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  // print test result
  if (test_result)
  {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  }
  else
  {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}
