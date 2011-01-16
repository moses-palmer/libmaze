#ifndef MAZE_RENDER_H
#define MAZE_RENDER_H

#include "maze.h"

/**
 * Prints a maze to stdout.
 *
 * @param maze
 *     The maze to print.
 * @param room_width
 *     The width, in characters, of a room.
 * @param room_height
 *     The height, in characters, of a room.
 * @param wall_char
 *     The character to use for walls.
 * @param floor_char
 *     The character to use for the floor.
 */
void
maze_render_print(Maze *maze, unsigned int room_width, unsigned int room_height,
    char wall_char, char floor_char);

/**
 * Renders a maze to the current frame buffer.
 *
 * The rooms will be placed as if the room at (0, 0) is rendered at (0, 0, 0).
 * Every room will be 1.0 * 1.0 * 1.0 dimension units in size.
 *
 * @param maze
 *     The maze to render.
 * @param wall_width
 *     The width of a wall. This is a value less than 1.0 and greater than 0.0
 *     that is interpreted as the portion of a half room that is wall.
 * @param draw_floor
 *     Whether to draw the floor.
 * @param cx
 *     The x-coordinate of the centre room.
 * @param cy
 *     The y-coordinate of the centre room.
 * @param d
 *     The number of rooms to render in each direction. The maximum number of
 *     rooms that are rendered is (2 * d) * (2 * d).
 */
void
maze_render_gl(Maze *maze, double wall_width, int draw_floor,
    unsigned int cx, unsigned int cy, unsigned int d);

#endif
