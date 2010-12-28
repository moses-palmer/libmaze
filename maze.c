#include <stdlib.h>
#include <string.h>

#include "maze.h"

Maze*
maze_create(unsigned int width, unsigned int height)
{
    Maze *result = malloc(sizeof(Maze) + width * height);

    result->width = width;
    result->height = height;
    result->data = (unsigned char*)(result + 1);
    memset(result->data, 0, width * height);

    return result;
}

void
maze_free(Maze *maze)
{
    free(maze);
}

int
maze_door_open(Maze *maze, unsigned int x, unsigned int y,
    unsigned char wall)
{
    /* The room lies outside of the maze */
    if (x >= maze->width || y >= maze->height) {
        return 0;
    }

    maze->data[y * maze->width + x] |= wall;
    if (maze_door_enter(maze, &x, &y, wall, 0)) {
        maze->data[y * maze->width + x] |= maze_wall_opposite(wall);
    }

    return 1;
}

int
maze_door_enter(Maze *maze, unsigned int *x, unsigned int *y,
    unsigned char wall, int only_if_open)
{
    /* The room lies outside of the maze */
    if (*x >= maze->width || *y >= maze->height) {
        return 0;
    }

    /* Do not enter a closed wall if only_if_open is set */
    if (only_if_open && !(maze_room_get(maze, *x, *y) & wall)) {
        return 0;
    }

    if (wall & MAZE_WALL_LEFT && *x > 0) {
        (*x)--;
    }
    else if (wall & MAZE_WALL_UP && *y > 0) {
        (*y)--;
    }
    else if (wall & MAZE_WALL_RIGHT && *x < maze->width - 1) {
        (*x)++;
    }
    else if (wall & MAZE_WALL_DOWN && *y < maze->height - 1) {
        (*y)++;
    }

    return 1;
}
