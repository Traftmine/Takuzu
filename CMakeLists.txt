cmake_minimum_required(VERSION 3.2)
project(takuzu C)

## testing
include(CTest)
enable_testing()

## find SDL2
include(SDL2.cmake)
message(STATUS "SDL2 include dir: ${SDL2_ALL_INC}")
message(STATUS "SDL2 all libraries: ${SDL2_ALL_LIBS}")

## compilation flags
set(CMAKE_C_FLAGS "-std=c99 -g -Wall")
link_directories(${CMAKE_SOURCE_DIR})

## executable text interface
add_library(game game.c game_aux.c game_ext.c queue.c game_tools.c game_private.c)
add_executable(game_text game_text.c)
target_link_libraries(game_text game)

## executable solve
add_executable(game_solve game_solve.c)
target_link_libraries(game_solve game)

## executable graphic interface
include_directories(${SDL2_ALL_INC})
add_executable(game_sdl game_sdl.c main.c)
target_link_libraries(game_sdl game ${SDL2_ALL_LIBS} m)

## copy useful data files in build dir
foreach(file "background.png" "one.png" "zero.png" "RobotCrush.ttf")
  file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach(file)

## tests
add_executable(game_test_leoroyer game_test_leoroyer.c)
target_link_libraries(game_test_leoroyer game)
add_test(test_leoroyer_dummy ./game_test_leoroyer dummy)
add_test(test_leoroyer_test_default ./game_test_leoroyer game_default)
add_test(test_leoroyer_test_get_square ./game_test_leoroyer game_get_square)
add_test(test_leoroyer_test_get_number ./game_test_leoroyer game_get_number)
add_test(test_leoroyer_test_is_immutable ./game_test_leoroyer game_is_immutable)
add_test(test_leoroyer_test_is_empty ./game_test_leoroyer game_is_empty)
add_test(test_leoroyer_test_get_next_square ./game_test_leoroyer game_get_next_square)
add_test(test_leoroyer_test_get_next_number ./game_test_leoroyer game_get_next_number)
add_test(test_leoroyer_test_undo ./game_test_leoroyer game_undo)
add_test(test_leoroyer_test_redo ./game_test_leoroyer game_redo)
add_test(test_leoroyer_test_load ./game_test_leoroyer game_load)
add_test(test_leoroyer_test_save ./game_test_leoroyer game_save)
add_test(test_leoroyer_test_solve ./game_test_leoroyer game_solve)
add_test(test_leoroyer_test_nb_solutions ./game_test_leoroyer game_nb_solve)

add_executable(game_test_jzebian game_test_jzebian.c)
target_link_libraries(game_test_jzebian game)
add_test(test_jzebian_dummy ./game_test_jzebian dummy)
add_test(test_jzebian_game_new ./game_test_jzebian game_new)
add_test(test_jzebian_game_new_empty ./game_test_jzebian game_new_empty)
add_test(test_jzebian_game_copy ./game_test_jzebian game_copy)
add_test(test_jzebian_game_equal ./game_test_jzebian game_equal)
add_test(test_jzebian_game_delete ./game_test_jzebian game_delete)
add_test(test_jzebian_game_set_square ./game_test_jzebian game_set_square)
add_test(test_jzebian_game_print ./game_test_jzebian game_print)
add_test(test_jzebian_game_new_ext ./game_test_jzebian game_new_ext)
add_test(test_jzebian_game_new_empty_ext ./game_test_jzebian game_new_empty_ext)
add_test(test_jzebian_game_nb_rows ./game_test_jzebian game_nb_rows)
add_test(test_jzebian_game_nb_cols ./game_test_jzebian game_nb_cols)
add_test(test_jzebian_game_is_wrapping ./game_test_jzebian game_is_wrapping)
add_test(test_jzebian_game_is_unique ./game_test_jzebian game_is_unique)

add_executable(game_test_epalospinto game_test_epalospinto.c)
target_link_libraries(game_test_epalospinto game)
add_test(test_epalospinto_dummy ./game_test_epalospinto dummy)
add_test(test_epalospinto_play_move ./game_test_epalospinto play_move)
add_test(test_epalospinto_game_restart ./game_test_epalospinto game_restart)
add_test(test_epalospinto_game_is_over ./game_test_epalospinto game_is_over)
add_test(test_epalospinto_game_has_error ./game_test_epalospinto game_has_error)
add_test(test_epalospinto_game_default_solution ./game_test_epalospinto game_default_solution)
add_test(test_epalospinto_game_check_move ./game_test_epalospinto game_check_move)
