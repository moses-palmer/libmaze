#include <math.h>

#include "maze.h"

int
maze_move_point(Maze *maze, double *x, double *y, double dx, double dy,
    double mx, double my)
{
    /* Verify that the parameters are correct */
    if (!maze
            || !x || !y
            || fabs(dx) > 1.0 || fabs(dy) > 1.0
            || mx < 0.0 || mx >= 0.5 || my < 0.0 || my >= 0.5) {
        return MAZE_WALL_ANY;
    }

    /* Retrieve the old room location and the position within the room */
    int ox, oy;
    double ofx, ofy;
    ox = (int)floor(*x);
    oy = (int)floor(*y);
    ofx = *x - ox;
    ofy = *y - oy;

    /* Calculate the new coordinates */
    *x += dx;
    *y += dy;

    /* Retrieve the new room location and the position within the room */
    int cx, cy;
    double cfx, cfy;
    cx = (int)floor(*x);
    cy = (int)floor(*y);
    cfx = *x - cx;
    cfy = *y - cy;

    /* Cache the inverted margins */
    double imx = 1.0 - mx;
    double imy = 1.0 - my;

    /* Determine what edges we have moved into */
    int edges = 0
        | (cfx < mx ? MAZE_WALL_LEFT : 0)
        | (cfx > imx ? MAZE_WALL_RIGHT : 0)
        | (cfy < my ? MAZE_WALL_UP : 0)
        | (cfy > imy ? MAZE_WALL_DOWN : 0);

    int result = 0;

    /* Handle bumping into walls of this room */
    if (edges & MAZE_WALL_LEFT && !maze_is_open_left(maze, cx, cy)) {
        if (ox == cx) {
            *x = cx + mx;
            cfx = mx;
            edges &= ~MAZE_WALL_LEFT;
            result |= MAZE_WALL_LEFT;
        }
        else {
            *x = ox + imx;
            cfx = imx;
            edges &= ~MAZE_WALL_LEFT;
            result |= MAZE_WALL_RIGHT;
        }
    }
    else if (edges & MAZE_WALL_RIGHT && !maze_is_open_right(maze, cx, cy)) {
        if (ox == cx) {
            *x = cx + imx;
            cfx = imx;
            edges &= ~MAZE_WALL_RIGHT;
            result |= MAZE_WALL_RIGHT;
        }
        else {
            *x = ox + mx;
            cfx = mx;
            edges &= ~MAZE_WALL_RIGHT;
            result |= MAZE_WALL_LEFT;
        }
    }
    if (edges & MAZE_WALL_UP && !maze_is_open_up(maze, cx, cy)) {
        if (oy == cy) {
            *y = cy + my;
            cfy = my;
            edges &= ~MAZE_WALL_UP;
            result |= MAZE_WALL_UP;
        }
        else {
            *y = oy + imy;
            cfy = imy;
            edges &= ~MAZE_WALL_UP;
            result |= MAZE_WALL_DOWN;
        }
    }
    else if (edges & MAZE_WALL_DOWN && !maze_is_open_down(maze, cx, cy)) {
        if (oy == cy) {
            *y = cy + imy;
            cfy = imy;
            edges &= ~MAZE_WALL_DOWN;
            result |= MAZE_WALL_DOWN;
        }
        else {
            *y = oy + my;
            cfy = my;
            edges &= ~MAZE_WALL_DOWN;
            result |= MAZE_WALL_UP;
        }
    }

    /* Handle bumping into corners */
    if ((edges & MAZE_CORNER_UP_LEFT) == MAZE_CORNER_UP_LEFT
            && maze_is_corner_up_left_out(maze, cx, cy)) {
        if (cfx > cfy) {
            *x = cx + mx;
            edges &= ~MAZE_WALL_LEFT;
        }
        else {
            *y = cy + my;
            edges &= ~MAZE_WALL_UP;
        }
    }
    else if ((edges & MAZE_CORNER_UP_RIGHT) == MAZE_CORNER_UP_RIGHT
            && maze_is_corner_up_right_out(maze, cx, cy)) {
        if (1.0 - cfx > cfy) {
            *x = cx + imx;
            edges &= ~MAZE_WALL_RIGHT;
        }
        else {
            *y = cy + my;
            edges &= ~MAZE_WALL_UP;
        }
    }
    else if ((edges & MAZE_CORNER_DOWN_LEFT) == MAZE_CORNER_DOWN_LEFT
            && maze_is_corner_down_left_out(maze, cx, cy)) {
        if (1.0 - cfx < cfy) {
            *x = cx + mx;
            edges &= ~MAZE_WALL_LEFT;
        }
        else {
            *y = cy + imy;
            edges &= ~MAZE_WALL_DOWN;
        }
    }
    else if ((edges & MAZE_CORNER_DOWN_RIGHT) == MAZE_CORNER_DOWN_RIGHT
            && maze_is_corner_down_right_out(maze, cx, cy)) {
        if (cfx < cfy) {
            *x = cx + imx;
            edges &= ~MAZE_WALL_RIGHT;
        }
        else {
            *y = cy + imy;
            edges &= ~MAZE_WALL_DOWN;
        }
    }

    return result;
}
