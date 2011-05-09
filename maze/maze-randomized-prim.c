#include <stdlib.h>
#include <string.h>

#include "maze.h"

/**
 * Calculates the number of RandomizedPrimData in the list.
 *
 * @param data
 *     A pointer to a list of RandomizedPrimData.
 * @return the number of items in the list
 */
static inline unsigned int
wall_count(RandomizedPrimData **data)
{
    unsigned int count;
    RandomizedPrimData *p;

    for (count = 0, p = *data;
        p;
        count++, p = p->next);

    return count;
}

/**
 * Picks a random wall from the list.
 *
 * The wall returned is removed from the list.
 *
 * @param data
 *     A pointer to a list of RandomizedPrimData.
 * @return an item that must be freed, or NULL if the list is empty
 */
static inline RandomizedPrimData*
wall_pick(RandomizedPrimData **data)
{
    unsigned int count, index;

    /* We cannot pick from an empty wall list */
    if (!*data) {
        return NULL;
    }

    /* Select a random item in the list by index */
    count = wall_count(data);
    index = rand() % count;

    if (index == 0) {
        /* If we remove the first item, we just have to change *data */
        RandomizedPrimData *result = *data;

        *data = (*data)->next;

        return result;
    }
    else {
        RandomizedPrimData *previous;
        RandomizedPrimData *result;

        /* Find the item before the index'th item */
        index--;
        for (previous = *data;
            index;
            index--, previous = previous->next);

        result = previous->next;
        previous->next = previous->next->next;

        return result;
    }
}

/**
 * Adds all valid walls of the room at (x, y) that are not already in the list.
 *
 * All walls that are not on the edge of the maze are valid.
 *
 * @param data
 *     A pointer to a list of RandomizedPrimData.
 * @param x
 *     The x coordinate of the room.
 * @param y
 *     The y coordinate of the room.
 * @param width
 *     The width of the maze.
 * @param height
 *     The height of the maze.
 */
static inline void
wall_add_all_new(RandomizedPrimData **data, unsigned int x, unsigned int y,
    unsigned int width, unsigned int height)
{
    unsigned int walls;
    RandomizedPrimData *p;
    unsigned int wall;

    /* Only try those walls that are valid */
    walls = 0
        | (x > 0
            ? MAZE_WALL_LEFT : 0)
        | (y > 0
            ? MAZE_WALL_UP : 0)
        | (x < width - 1
            ? MAZE_WALL_RIGHT : 0)
        | (y < height - 1
            ? MAZE_WALL_DOWN : 0);

    /* Remove any wall already present in the list */
    for (p = *data; p; p = p->next) {
        if (p->x == x && p->y == y) {
            walls &= ~p->wall;
        }
    }

    /* Add all remaining walls */
    for (wall = 1; wall & MAZE_WALL_ANY; wall <<= 1) {
        if (wall & walls) {
            RandomizedPrimData *current = malloc(sizeof(RandomizedPrimData));

            current->x = x;
            current->y = y;
            current->wall = wall;
            current->next = *data;
            *data = current;
        }
    }
}

void
maze_initialize_randomized_prim(Maze *maze, MazeInitializeCallback callback,
    void *context)
{
    RandomizedPrimData *walls, *wall;
    unsigned int start_x, start_y;

    /* Start with a random room and add its walls */
    start_x = rand() % maze->width;
    start_y = rand() % maze->height;
    walls = NULL;
    wall_add_all_new(&walls, start_x, start_y, maze->width, maze->height);

    /* Mark the room as part of the maze */
    maze_data_set(maze, start_x, start_y, (void*)1);

    while ((wall = wall_pick(&walls)) != NULL) {
        int x, y;

        x = wall->x;
        y = wall->y;
        if (maze_door_enter(maze, &x, &y, wall->wall, 0)
                && !maze_room_get(maze, x, y)) {
            /* Only proceed if the room has not been touched before */
            maze_door_open(maze, wall->x, wall->y, wall->wall);
            wall_add_all_new(&walls, x, y, maze->width, maze->height);

            if (callback) {
                maze_data_set(maze, x, y, callback(context, maze, x, y, walls));
            }
        }

        free(wall);
    }

    /* Set the data of the start room */
    maze_data_set(maze, start_x, start_y, callback
        ? callback(context, maze, start_x, start_y, NULL)
        : 0);
}
