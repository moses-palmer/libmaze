#include <math.h>

#include <GL/gl.h>

#include "maze-render.h"

/**
 * Defines a rectangle in 3D space.
 *
 * The normal will be calculated, but only for the triangle (1) - (2) - (4);
 * thus the points must be on a plane.
 *
 * @param x1, y1, z1
 *     The coordinates of the top left corner.
 * @param x2, y2, z2
 *     The coordinates of the bottom left corner.
 * @param x3, y3, z3
 *     The coordinates of the bottom right corner.
 * @param x4, y4, z4
 *     The coordinates of the top right corner.
 */
static inline void
rectangle(
    double x1, double y1, double z1,
    double x2, double y2, double z2,
    double x3, double y3, double z3,
    double x4, double y4, double z4)
{
    struct {
        double x, y, z;
    } u = {x2 - x1, y2 - y1, z2 - z1};
    struct {
        double x, y, z;
    } v = {x4 - x1, y4 - y1, z4 - z1};
    struct {
        double x, y, z;
    } n = {u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x};
    double d = 1.0 / sqrt(n.x * n.x + n.y * n.y + n.z * n.z);

    glNormal3f(d * n.x, d * n.y, d * n.z);
    glBegin(GL_TRIANGLES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);
    glVertex3f(x1, y1, z1);
    glVertex3f(x3, y3, z3);
    glVertex3f(x4, y4, z4);
    glEnd();
}

/**
 * Draws a wall.
 *
 * The wall is rendered in the bottom part of the frame buffer.
 *
 * @param wall_width
 *     The width of a wall.
 * @param slope_width
 *     The width of a slope.
 */
static void
draw_wall(double wall_width, double slope_width)
{
    /* The top */
    rectangle(
        0.0, wall_width, 1.0,
        0.0, 0.0, 1.0,
        1.0, 0.0, 1.0,
        1.0, wall_width, 1.0);

    /* The vertical part */
    rectangle(
        0.0, wall_width + slope_width, 0.0,
        0.0, wall_width, 1.0,
        1.0, wall_width, 1.0,
        1.0, wall_width + slope_width, 0.0);
}

/**
 * Draws a corner.
 *
 * The corner is rendered in the bottom left part of the frame buffer.
 *
 * @param wall_width
 *     The width of a wall.
 */
static void
draw_corner(double wall_width, double slope_width)
{
    /* The top */
    rectangle(
        0.0, wall_width, 1.0,
        0.0, 0.0, 1.0,
        wall_width, 0.0, 1.0,
        wall_width, wall_width, 1.0);

    /* The top vertical part */
    rectangle(
        0.0, wall_width + slope_width, 0.0,
        0.0, wall_width, 1.0,
        wall_width, wall_width, 1.0,
        wall_width + slope_width, wall_width + slope_width, 0.0);

    /* The right vertical part */
    rectangle(
        wall_width, wall_width, 1.0,
        wall_width, 0.0, 1.0,
        wall_width + slope_width, 0.0, 0.0,
        wall_width + slope_width, wall_width + slope_width, 0.0);
}

/**
 * Draws the outer edge of a wall.
 *
 * The edge is rendered in the bottom part of the frame buffer.
 *
 * @param wall_width
 *     The width of a wall.
 * @param is_outer
 *     Whether the wall is along the edge of the maze. If this is the case, an
 *     outer wall is drawn as well.
 */
static void
draw_edge(double wall_width, double slope_width, int is_open)
{
    if (is_open) {
        rectangle(
            0.0, 0.0, 1.0,
            0.0, 0.0, 0.0,
            wall_width + slope_width, 0.0, 0.0,
            wall_width, 0.0, 1.0);

        rectangle(
            1.0 - wall_width, 0.0, 1.0,
            1.0 - wall_width - slope_width, 0.0, 0.0,
            1.0, 0.0, 0.0,
            1.0, 0.0, 1.0);
    }
    else {
        rectangle(
            0.0, 0.0, 1.0,
            0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            1.0, 0.0, 1.0);
    }
}

#define HANDLE_WALL(corner, wall, is_edge) \
    do { \
        int is_open = maze_is_open_##wall(maze, x, y); \
        if (!is_open) { \
            draw_wall(wall_width, slope_width); \
        } \
        else if (maze_is_corner_##corner##_out(maze, x, y)) { \
            draw_corner(wall_width, slope_width); \
        } \
        if (is_edge) { \
            draw_edge(wall_width, slope_width, is_open); \
        } \
    } while (0)

#define NEXT_WALL() \
    glTranslatef(0.5, 0.5, 0.0); \
    glRotatef(90.0, 0.0, 0.0, -1.0); \
    glTranslatef(-0.5, -0.5, 0.0)

static void
draw_room(Maze *maze, unsigned int x, unsigned int y, double wall_width,
    double slope_width)
{
    HANDLE_WALL(down_left, down, y == maze->height - 1);
    NEXT_WALL();

    HANDLE_WALL(up_left, left, x == 0);
    NEXT_WALL();

    HANDLE_WALL(up_right, up, y == 0);
    NEXT_WALL();

    HANDLE_WALL(down_right, right, x == maze->width - 1);
    NEXT_WALL();
}

static void
draw_floor(Maze *maze, unsigned int x, unsigned int y, double floor_width)
{
    /* The top part */
    rectangle(
        0.0, 1.0, 0.0,
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 1.0, 0.0);

    /* The bottom part */
    rectangle(
        0.0, 1.0, -floor_width,
        1.0, 1.0, -floor_width,
        1.0, 0.0, -floor_width,
        0.0, 0.0, -floor_width);

    /* Is there a left edge? */
    if (x == 0) {
        rectangle(
            0.0, 1.0, 0.0,
            0.0, 1.0, -floor_width,
            0.0, 0.0, -floor_width,
            0.0, 0.0, 0.0);
    }

    /* Is there an up edge? */
    if (y == 0) {
        rectangle(
            0.0, 1.0, 0.0,
            1.0, 1.0, 0.0,
            1.0, 1.0, -floor_width,
            0.0, 1.0, -floor_width);
    }

    /* Is there a right edge? */
    if (x == maze->width - 1) {
        rectangle(
            1.0, 1.0, 0.0,
            1.0, 0.0, 0.0,
            1.0, 0.0, -floor_width,
            1.0, 1.0, -floor_width);
    }

    /* Is there a down edge? */
    if (y == maze->height - 1) {
        rectangle(
            0.0, 0.0, 0.0,
            0.0, 0.0, -floor_width,
            1.0, 0.0, -floor_width,
            1.0, 0.0, 0.0);
    }
}

int
maze_render_gl(Maze *maze, double wall_width, double slope_width,
    double floor_thickness, int cx, int cy, unsigned int d, int flags)
{
    int x, y;

    /* Verify input parameters */
    if (!maze || wall_width < 0.0 || slope_width < 0.0 || floor_thickness < 0.0
            || wall_width + slope_width > 0.5 || floor_thickness > 1.0
            || flags & ~MAZE_RENDER_GL_MASK) {
        return 0;
    }

    /* Draw every requested room */
    for (y = cy - (int)d; y <= cy + (int)d; y++) {
        if (y < 0 || y >= maze->height) {
            continue;
        }

        for (x = cx - (int)d; x <= cx + (int)d; x++) {
            if (x < 0 || x >= maze->width) {
                continue;
            }

            glPushMatrix();
            glTranslatef(x, maze->height - 1 - y, 0.0);
            if (flags & MAZE_RENDER_GL_WALLS) {
                draw_room(maze, x, y, wall_width, slope_width);
            }
            if (flags & MAZE_RENDER_GL_FLOOR) {
                draw_floor(maze, x, y, floor_thickness);
            }
            glPopMatrix();
        }
    }

    return 1;
}
