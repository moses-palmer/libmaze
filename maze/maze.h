#ifndef MAZE_H
#define MAZE_H

#include <stdlib.h>

/**
 * The bit masks used for the walls.
 */
enum {
    MAZE_WALL_LEFT  = 1 << 0,
    MAZE_WALL_UP    = 1 << 1,
    MAZE_WALL_RIGHT = 1 << 2,
    MAZE_WALL_DOWN  = 1 << 3,

    MAZE_WALL_ANY   = (1 << 4) - 1
};

/**
 * The bit mask for the top left corner.
 */
#define MAZE_CORNER_UP_LEFT (MAZE_WALL_UP | MAZE_WALL_LEFT)

/**
 * The bit mask for the top right corner.
 */
#define MAZE_CORNER_UP_RIGHT (MAZE_WALL_UP | MAZE_WALL_RIGHT)

/**
 * The bit mask for the bottom left corner.
 */
#define MAZE_CORNER_DOWN_LEFT (MAZE_WALL_DOWN | MAZE_WALL_LEFT)

/**
 * The bit mask for the bottom right corner.
 */
#define MAZE_CORNER_DOWN_RIGHT (MAZE_WALL_DOWN | MAZE_WALL_RIGHT)

/**
 * Returns the wall opposite to wall.
 *
 * The value of this macro is undefined for invalid input data.
 *
 * @param wall
 *     The bit mask of the wall.
 */
#define maze_wall_opposite(wall) \
    ((wall > MAZE_WALL_UP) ? (wall) >> 2 : (wall) << 2)


/**
 * A room.
 *
 * A maze is a matrix of rooms.
 */
typedef struct {
    /** The bit mask corresponding to the walls; a wall is open if its bit is
        set */
    unsigned char walls;

    /** The data of the room */
    void *data;
} Room;

/**
 * The structure of a maze instance.
 */
typedef struct {
    /** The width of the maze */
    unsigned int width;

    /** The height of the maze */
    unsigned int height;

    /** The actual maze data; its size is width * height */
    Room *data;
} Maze;


/**
 * Creates a maze of the specific dimensions.
 *
 * Initially, no walls are open.
 *
 * @param width
 *     The width of the maze.
 * @param height
 *     The height of the maze.
 * @return a new maze with no walls open
 */
Maze*
maze_create(unsigned int width, unsigned int height);

/**
 * Frees all resources allocated by the maze.
 *
 * @param maze
 *     The maze to release.
 */
void
maze_free(Maze *maze);

/**
 * Opens a door in a room.
 *
 * If the room on the opposite side of the wall is within the maze, its
 * corresponding wall is opened as well.
 *
 * @param maze
 *     The maze to manipulate.
 * @param x, y
 *     The coordinates of the room.
 * @param wall
 *     The wall to open.
 * @return whether the door was actually opened.
 */
int
maze_door_open(Maze *maze, int x, int y, unsigned char wall);

/**
 * Calculates the coordinates of the room that lies on the other side of wall.
 *
 * @param maze
 *     The maze.
 * @param x, y
 *     The coordinates of the room. These parameters are overwritten.
 * @param wall
 *     The wall to enter.
 * @param only_if_open
 *     Whether to only enter walls that are open. If this parameter is non-zero,
 *     x and y will not be modified if wall is not an open wall in the room.
 * @return whether x or y were modified
 */
int
maze_door_enter(Maze *maze, int *x, int *y, unsigned char wall,
    int only_if_open);

/**
 * The function signature of a maze initialisation callback function.
 *
 * @param context
 *     The user specified context of the maze initialisation.
 * @param maze
 *     The maze that is being initialised.
 * @param x, y
 *     The coordinates of the room.
 * @param initialize_data
 *     Data specific to the maze initialisation algorithm.
 * @return the data value that will be applied to the room
 */
typedef void* (*MazeInitializeCallback)(void *context, Maze *maze, int x, int y,
    void *initialize_data);


/**
 * Data used by the Randomised Prim algorithm.
 */
typedef struct RandomizedPrimData RandomizedPrimData;
struct RandomizedPrimData {
    /** The coordinates of the room */
    int x, y;

    /** One of the MAZE_WALL_* macros */
    unsigned char wall;

    /** The next wall entry */
    RandomizedPrimData *next;
};

/**
 * Initialises the maze with the Randomised Prim algorithm.
 *
 * When this function is used, the initialize_data sent to the callback will be
 * a RandomizedPrimData* list that contains all walls waiting to be opened.
 *
 * @param maze
 *     The maze to initialise.
 * @param callback
 *     The callback function to use. Its return value is used as the data bits
 *     for the room at (x, y). This may be NULL, in which case all data fields
 *     will be NULL.
 * @param context
 *     The user context passed to the callback function.
 */
void
maze_initialize_randomized_prim(Maze *maze, MazeInitializeCallback callback,
    void *context);


/**
 * Determines whether a maze contains a room.
 *
 * @param maze
 *     The maze on which to operate.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the room is inside the maze and 0 otherwise
 */
static inline int
maze_contains(Maze *maze, int x, int y)
{
    return x >= 0 && x < (int)maze->width && y >= 0 && y < (int)maze->height;
}

/**
 * Retrieves the wall value of a room.
 *
 * @param maze
 *     The maze on which to operate.
 * @param x, y
 *     The coordinates of the room.
 * @return the value of the room, or 0 if the room is invalid
 */
static inline unsigned char
maze_room_get(Maze *maze, int x, int y)
{
    if (maze_contains(maze, x, y)) {
        return maze->data[y * maze->width + x].walls;
    }
    else if (x == -1 && maze_contains(maze, 0, y)) {
        return MAZE_WALL_ANY
            & ~(maze->data[y * maze->width + 0].walls
                    & MAZE_WALL_LEFT
                ? 0
                : MAZE_WALL_RIGHT);
    }
    else if (x == maze->width && maze_contains(maze, maze->width - 1, y)) {
        return MAZE_WALL_ANY
            & ~(maze->data[y * maze->width + maze->width - 1].walls
                    & MAZE_WALL_RIGHT
                ? 0
                : MAZE_WALL_LEFT);
    }
    else if (y == -1 && maze_contains(maze, x, 0)) {
        return MAZE_WALL_ANY
            & ~(maze->data[0 * maze->width + x].walls
                    & MAZE_WALL_UP
                ? 0
                : MAZE_WALL_DOWN);
    }
    else if (y == maze->height && maze_contains(maze, x, maze->height - 1)) {
        return MAZE_WALL_ANY
            & ~(maze->data[(maze->height - 1) * maze->width + x].walls
                    & MAZE_WALL_DOWN
                ? 0
                : MAZE_WALL_UP);
    }

    return MAZE_WALL_ANY;
}

/**
 * Retrieves the room data.
 *
 * @param maze
 *     The maze on which to operate.
 * @param x, y
 *     The coordinates of the room.
 * @return the room data, or 0 if the room is invalid
 */
static inline void*
maze_data_get(Maze *maze, int x, int y)
{
    if (maze_contains(maze, x, y)) {
        return maze->data[y * maze->width + x].data;
    }

    return NULL;
}

/**
 * Sets the data of a room.
 *
 * @param maze
 *     The maze on which to operate.
 * @param x, y
 *     The coordinates of the room.
 * @param data
 *     The room data to set.
 * @return whether the data was set
 */
static inline int
maze_data_set(Maze *maze, int x, int y, void *data)
{
    if (maze_contains(maze, x, y)) {
        maze->data[y * maze->width + x].data = data;
        return 1;
    }

    return 0;
}

/**
 * Determines whether the left door is open.
 *
 * If the point is on the edge of the maze, this macro will determine whether
 * there is an entrance to the maze. If the point is completely outside of the
 * maze, this macro will always be 0.
 *
 * @param maze
 *     The maze to check.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the door is open
 */
#define maze_is_open_left(maze, x, y) \
    (maze_room_get(maze, x, y) & MAZE_WALL_LEFT)

/**
 * Determines whether the up door is open.
 *
 * If the point is on the edge of the maze, this macro will determine whether
 * there is an entrance to the maze. If the point is completely outside of the
 * maze, this macro will always be 0.
 *
 * @param maze
 *     The maze to check.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the door is open
 */
#define maze_is_open_up(maze, x, y) \
    (maze_room_get(maze, x, y) & MAZE_WALL_UP)

/**
 * Determines whether the right door is open.
 *
 * If the point is on the edge of the maze, this macro will determine whether
 * there is an entrance to the maze. If the point is completely outside of the
 * maze, this macro will always be 0.
 *
 * @param maze
 *     The maze to check.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the door is open
 */
#define maze_is_open_right(maze, x, y) \
    (maze_room_get(maze, x, y) & MAZE_WALL_RIGHT)

/**
 * Determines whether the down door is open.
 *
 * If the point is on the edge of the maze, this macro will determine whether
 * there is an entrance to the maze. If the point is completely outside of the
 * maze, this macro will always be 0.
 *
 * @param maze
 *     The maze to check.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the door is open
 */
#define maze_is_open_down(maze, x, y) \
    (maze_room_get(maze, x, y) & MAZE_WALL_DOWN)

/**
 * Determines whether the room has a corner in the top left part.
 *
 * @param maze
 *     The maze to check.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the room has a corner
 */
#define maze_is_corner_up_left(maze, x, y) (1 \
    && !maze_is_open_up(maze, x, y) \
    && !maze_is_open_left(maze, x, y))

/**
 * Determines whether the room has a corner in the top right part.
 *
 * @param maze
 *     The maze to check.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the room has a corner
 */
#define maze_is_corner_up_right(maze, x, y) (1 \
    && !maze_is_open_up(maze, x, y) \
    && !maze_is_open_right(maze, x, y))

/**
 * Determines whether the room has a corner in the bottom left part.
 *
 * @param maze
 *     The maze to check.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the room has a corner
 */
#define maze_is_corner_down_left(maze, x, y) (1 \
    && !maze_is_open_down(maze, x, y) \
    && !maze_is_open_left(maze, x, y))

/**
 * Determines whether the room has a corner in the bottom right part.
 *
 * @param maze
 *     The maze to check.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the room has a corner
 */
#define maze_is_corner_down_right(maze, x, y) (1 \
    && !maze_is_open_down(maze, x, y) \
    && !maze_is_open_right(maze, x, y))

/**
 * Determines whether the room has a protruding corner in the top left part.
 *
 * @param maze
 *     The maze to check.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the room has a protruding corner
 */
#define maze_is_corner_up_left_out(maze, x, y) (1 \
    && maze_is_open_up(maze, x, y) \
    && maze_is_open_left(maze, x, y) \
    && (0 \
        || !maze_is_open_up(maze, x - 1, y) \
        || !maze_is_open_left(maze, x, y - 1)))

/**
 * Determines whether the room has a protruding corner in the top right part.
 *
 * @param maze
 *     The maze to check.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the room has a protruding corner
 */
#define maze_is_corner_up_right_out(maze, x, y) (1 \
    && maze_is_open_up(maze, x, y) \
    && maze_is_open_right(maze, x, y) \
    && (0 \
        || !maze_is_open_up(maze, x + 1, y) \
        || !maze_is_open_right(maze, x, y - 1)))

/**
 * Determines whether the room has a protruding corner in the bottom left part.
 *
 * @param maze
 *     The maze to check.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the room has a protruding corner
 */
#define maze_is_corner_down_left_out(maze, x, y) (1 \
    && maze_is_open_down(maze, x, y) \
    && maze_is_open_left(maze, x, y) \
    && (0 \
        || !maze_is_open_down(maze, x - 1, y) \
        || !maze_is_open_left(maze, x, y + 1)))

/**
 * Determines whether the room has a protruding corner in the bottom right part.
 *
 * @param maze
 *     The maze to check.
 * @param x, y
 *     The coordinates of the room.
 * @return non-zero if the room has a protruding corner
 */
#define maze_is_corner_down_right_out(maze, x, y) (1 \
    && maze_is_open_down(maze, x, y) \
    && maze_is_open_right(maze, x, y) \
    && (0 \
        || !maze_is_open_down(maze, x + 1, y) \
        || !maze_is_open_right(maze, x, y + 1)))

#endif
