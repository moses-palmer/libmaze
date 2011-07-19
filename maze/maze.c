#include <stdlib.h>
#include <string.h>

#include "maze.h"

Maze*
maze_create(unsigned int width, unsigned int height)
{
    Maze *result = malloc(sizeof(Maze) + sizeof(Room) * width * height);

    result->width = width;
    result->height = height;
    result->data = (Room*)(result + 1);
    memset(result->data, 0, sizeof(Room) * width * height);

    return result;
}

void
maze_free(Maze *maze)
{
    free(maze);
}

int
maze_door_open(Maze *maze, int x, int y, unsigned char wall)
{
    /* The room lies outside of the maze */
    if (!maze_contains(maze, x, y)) {
        return 0;
    }

    maze->data[y * maze->width + x].walls |= wall;

    /* Open the opposite door in the other room if it lies within the maze */
    if (maze_door_enter(maze, &x, &y, wall, 0) && maze_contains(maze, x, y)) {
        maze->data[y * maze->width + x].walls |= maze_wall_opposite(wall);
    }

    return 1;
}

int
maze_door_enter(Maze *maze, int *x, int *y, unsigned char wall,
    int only_if_open)
{
    /* The room lies outside of the maze */
    if (!maze_contains(maze, *x, *y)) {
        return 0;
    }

    /* Do not enter a closed wall if only_if_open is set */
    if (only_if_open && !(maze_room_get(maze, *x, *y) & wall)) {
        return 0;
    }

    if (wall & MAZE_WALL_LEFT) {
        (*x)--;
    }
    else if (wall & MAZE_WALL_UP) {
        (*y)--;
    }
    else if (wall & MAZE_WALL_RIGHT) {
        (*x)++;
    }
    else if (wall & MAZE_WALL_DOWN) {
        (*y)++;
    }

    return 1;
}
