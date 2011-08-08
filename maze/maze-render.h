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
 * Flags for maze_render_gl.
 */
enum {
    /** Render the walls */
    MAZE_RENDER_GL_WALLS = 1 << 0,

    /** Render the floor */
    MAZE_RENDER_GL_FLOOR = 1 << 1,

    /** Render the top of walls */
    MAZE_RENDER_GL_TOP = 1 << 2,

    /** Whether to apply texture coordinates to the maze vertices */
    MAZE_RENDER_GL_TEXTURE = 1 << 3,

    MAZE_RENDER_GL_LAST
};

/**
 * The mask of valid flags for maze_render_gl.
 */
#define MAZE_RENDER_GL_MASK (((MAZE_RENDER_GL_LAST - 1) << 1) - 1)

/**
 * Renders a maze to the current frame buffer.
 *
 * The rooms will be placed as if the room at (0, 0) is rendered at (0, 0, 0).
 * Every room will be 1.0 * 1.0 * 1.0 dimension units in size.
 *
 * The floor, if rendered, will be placed below the walls and thus protrude in
 * the negative z-axis.
 *
 * @param maze
 *     The maze to render.
 * @param wall_width
 *     The width of a wall. This is a value less than 1.0 and greater than 0.0
 *     that is interpreted as the portion of a half room that is wall.
 * @param slope_width
 *     The width of the slope from the top to the floor.
 * @param floor_thickness
 *     The thickness of the floor, if drawn.
 * @param cx
 *     The x-coordinate of the centre room.
 * @param cy
 *     The y-coordinate of the centre room.
 * @param d
 *     The number of rooms to render in each direction. The maximum number of
 *     rooms that are rendered is (2 * d) * (2 * d).
 * @param flags
 *     Flags that affect the operation. See the MAZE_RENDER_GL_* constants for
 *     more information.
 * @return 0 if a parameter is incorrect and non-zero otherwise
 */
int
maze_render_gl(Maze *maze, double wall_width, double slope_width,
    double floor_thickness, int cx, int cy, unsigned int d, int flags);

#endif
