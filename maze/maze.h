#ifndef MAZE_H
#define MAZE_H

enum {
    MAZE_WALL_LEFT  = 1 << 0,
    MAZE_WALL_UP    = 1 << 1,
    MAZE_WALL_RIGHT = 1 << 2,
    MAZE_WALL_DOWN  = 1 << 3,
    MAZE_WALL_ANY   = (1 << 4) - 1,
    MAZE_WALL_DATA  = 0xff & ~((1 << 4) - 1)
};

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
 * @param x
 *     The x coordinate of the room.
 * @param y
 *     The y coordingate of the room.
 * @return whether the door was actually opened.
 */
int
maze_door_open(Maze *maze, int x, int y, unsigned char wall);

/**
 * Calculates the coordinates of the room that lies on the other side of wall.
 *
 * If the room lies outside of the maze, x and y are not modified.
 *
 * @param maze
 *     The maze.
 * @param x
 *     The x coordinate of the room. This parameter is overwritten.
 * @param y
 *     The y coordinate of the room. This parameter is overwritten.
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
 * @param x
 *     The x coordinate of the current room.
 * @param y
 *     The y coordinate of the current room.
 * @param initialize_data
 *     Data specific to the maze initialisation algorithm.
 * @return the data value that will be applied to the room
 */
typedef void* (*MazeInitializeCallback)(void *context, Maze *maze,
    unsigned int x, unsigned int y, void *initialize_data);


/**
 * Data used by the Randomised Prim algorithm.
 */
typedef struct RandomizedPrimData RandomizedPrimData;
struct RandomizedPrimData {
    /* The coordinates of the room */
    unsigned int x, y;

    /* One of the MAZE_WALL_* macros */
    unsigned char wall;

    /* The next wall entry */
    RandomizedPrimData *next;
};

/**
 * Initialises the maze with the Randomised Prim algorithm.
 *
 * When this function is used, the initialize_data send to the callback will be
 * a RandomizedPrimData* list that contains all walls waiting to be opened.
 *
 * @param maze
 *     The maze to initialise.
 * @param callback
 *     The callback function to use. Its return value is used as the data bits
 *     for the room at (x, y). This may be NULL, in which case all data fields
 *     will be 0.
 * @param context
 *     The user context passed to the callback function.
 */
void
maze_initialize_randomized_prim(Maze *maze, MazeInitializeCallback callback,
    void *context);


/**
 * Retrieves the value of a room.
 *
 * @param maze
 *     The maze on which to operate.
 * @param x
 *     The x coordinate of the room.
 * @param y
 *     The y coordinate of the room.
 * @return the value of the room, or 0 if the room is invalid
 */
static inline unsigned char
maze_room_get(Maze *maze, int x, int y)
{
    if (x >= 0 && x < maze->width && y >= 0 && y < maze->height) {
        return maze->data[y * maze->width + x].walls;
    }

    return 0;
}

/**
 * Retrieves the room data.
 *
 * @param maze
 *     The maze on which to operate.
 * @param x
 *     The x coordinate of the room.
 * @param y
 *     The y coordinate of the room.
 * @return the room data, or 0 if the room is invalid
 */
static inline void*
maze_data_get(Maze *maze, int x, int y)
{
    if (x >= 0 && x < maze->width && y >= 0 && y < maze->height) {
        return maze->data[y * maze->width + x].data;
    }

    return 0;
}

/**
 * Sets the data of a room.
 *
 * @param maze
 *     The maze on which to operate.
 * @param x
 *     The x coordinate of the room.
 * @param y
 *     The y coordinate of the room.
 * @param data
 *     The room data to set.
 * @return whether the data was set
 */
static inline int
maze_data_set(Maze *maze, int x, int y, void *data)
{
    if (x >= 0 && x < maze->width && y >= 0 && y < maze->height) {
        maze->data[y * maze->width + x].data = data;
        return 1;
    }

    return 0;
}

/**
 * Macros to determine whether walls are open.
 */
#define maze_is_open_left(maze, x, y) \
    (maze_room_get(maze, x, y) & MAZE_WALL_LEFT)

#define maze_is_open_up(maze, x, y) \
    (maze_room_get(maze, x, y) & MAZE_WALL_UP)

#define maze_is_open_right(maze, x, y) \
    (maze_room_get(maze, x, y) & MAZE_WALL_RIGHT)

#define maze_is_open_down(maze, x, y) \
    (maze_room_get(maze, x, y) & MAZE_WALL_DOWN)

/**
 * Macros to determine whether rooms have corners in different directions.
 */
#define maze_is_corner_ul_out(maze, x, y) (1 \
    && maze_is_open_left(maze, x, y) \
    && maze_is_open_up(maze, x, y))
#define maze_is_corner_ul_in(maze, x, y) (1 \
    && !maze_is_open_left(maze, x, y) \
    && !maze_is_open_up(maze, x, y))

#define maze_is_corner_ur_out(maze, x, y) (1 \
    && maze_is_open_up(maze, x, y) \
    && maze_is_open_right(maze, x, y))
#define maze_is_corner_ur_in(maze, x, y) (1 \
    && !maze_is_open_up(maze, x, y) \
    && !maze_is_open_right(maze, x, y))

#define maze_is_corner_dr_out(maze, x, y) (1 \
    && maze_is_open_right(maze, x, y) \
    && maze_is_open_down(maze, x, y))
#define maze_is_corner_dr_in(maze, x, y) (1 \
    && !maze_is_open_right(maze, x, y) \
    && !maze_is_open_down(maze, x, y))

#define maze_is_corner_dl_out(maze, x, y) (1 \
    && maze_is_open_down(maze, x, y) \
    && maze_is_open_left(maze, x, y))
#define maze_is_corner_dl_in(maze, x, y) (1 \
    && !maze_is_open_down(maze, x, y) \
    && !maze_is_open_left(maze, x, y))

#endif
