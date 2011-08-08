#include <math.h>

#include <GL/gl.h>

#include "maze-render.h"

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
    GLfloat d;

    /* The top */
    glNormal3f(
        0.0,
        0.0,
        1.0);
    glBegin(GL_QUADS);
    glVertex3f(
        0.0,
        wall_width,
        1.0);
    glVertex3f(
        0.0,
        0.0,
        1.0);
    glVertex3f(
        1.0,
        0.0,
        1.0);
    glVertex3f(
        1.0,
        wall_width,
        1.0);
    glEnd();

    /* The vertical part */
    d = 1.0 / sqrt(1 + slope_width * slope_width);
    glNormal3f(
        0.0,
        d,
        slope_width * d);
    glBegin(GL_QUADS);
    glVertex3f(
        0.0,
        wall_width,
        1.0);
    glVertex3f(
        1.0,
        wall_width,
        1.0);
    glVertex3f(
        1.0,
        wall_width + slope_width,
        0.0);
    glVertex3f(
        0.0,
        wall_width + slope_width,
        0.0);
    glEnd();
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
    GLfloat d;

    /* The top */
    glNormal3f(
        0.0,
        0.0,
        1.0);
    glBegin(GL_QUADS);
    glVertex3f(
        0.0,
        wall_width,
        1.0);
    glVertex3f(
        0.0,
        0.0,
        1.0);
    glVertex3f(
        wall_width,
        0.0,
        1.0);
    glVertex3f(
        wall_width,
        wall_width,
        1.0);
    glEnd();

    /* The top vertical part */
    d = 1.0 / sqrt(1 + slope_width * slope_width);
    glNormal3f(
        0.0,
        d,
        slope_width * d);
    glBegin(GL_QUADS);
    glVertex3f(
        0.0,
        wall_width,
        1.0);
    glVertex3f(
        wall_width,
        wall_width,
        1.0);
    glVertex3f(
        wall_width + slope_width,
        wall_width + slope_width,
        0.0);
    glVertex3f(
        0.0,
        wall_width + slope_width,
        0.0);
    glEnd();

    /* The right vertical part */
    d = 1.0 / sqrt(1 + slope_width * slope_width);
    glNormal3f(
        d,
        0.0,
        slope_width * d);
    glBegin(GL_QUADS);
    glVertex3f(
        wall_width,
        wall_width,
        1.0);
    glVertex3f(
        wall_width,
        0.0,
        1.0);
    glVertex3f(
        wall_width + slope_width,
        0.0,
        0.0);
    glVertex3f(
        wall_width + slope_width,
        wall_width + slope_width,
        0.0);
    glEnd();
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
        glNormal3f(
            0.0,
            -1.0,
            0.0);
        glBegin(GL_QUADS);
        glVertex3f(
            0.0,
            0.0,
            1.0);
        glVertex3f(
            0.0,
            0.0,
            0.0);
        glVertex3f(
            wall_width + slope_width,
            0.0,
            0.0);
        glVertex3f(
            wall_width,
            0.0,
            1.0);
        glEnd();

        glNormal3f(
            0.0,
            -1.0,
            0.0);
        glBegin(GL_QUADS);
        glVertex3f(
            1.0 - wall_width,
            0.0,
            1.0);
        glVertex3f(
            1.0 - wall_width - slope_width,
            0.0,
            0.0);
        glVertex3f(
            1.0,
            0.0,
            0.0);
        glVertex3f(
            1.0,
            0.0,
            1.0);
        glEnd();
    }
    else {
        glNormal3f(
            0.0,
            -1.0,
            0.0);
        glBegin(GL_QUADS);
        glVertex3f(
            0.0,
            0.0,
            1.0);
        glVertex3f(
            0.0,
            0.0,
            0.0);
        glVertex3f(
            1.0,
            0.0,
            0.0);
        glVertex3f(
            1.0,
            0.0,
            1.0);
        glEnd();
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
    glNormal3f(
        0.0,
        0.0,
        1.0);
    glBegin(GL_QUADS);
    glVertex3f(
        0.0,
        1.0,
        0.0);
    glVertex3f(
        0.0,
        0.0,
        0.0);
    glVertex3f(
        1.0,
        0.0,
        0.0);
    glVertex3f(
        1.0,
        1.0,
        0.0);
    glEnd();

    /* The bottom part */
    glNormal3f(
        0.0,
        0.0,
        -1.0);
    glBegin(GL_QUADS);
    glVertex3f(
        0.0,
        1.0,
        -floor_width);
    glVertex3f(
        1.0,
        1.0,
        -floor_width);
    glVertex3f(
        1.0,
        0.0,
        -floor_width);
    glVertex3f(
        0.0,
        0.0,
        -floor_width);
    glEnd();

    /* Is there a left edge? */
    if (x == 0) {
        glNormal3f(
            -1.0,
            0.0,
            0.0);
        glBegin(GL_QUADS);
        glVertex3f(
            0.0,
            1.0,
            0.0);
        glVertex3f(
            0.0,
            1.0,
            -floor_width);
        glVertex3f(
            0.0,
            0.0,
            -floor_width);
        glVertex3f(
            0.0,
            0.0,
            0.0);
        glEnd();
    }

    /* Is there an up edge? */
    if (y == 0) {
        glNormal3f(
            0.0,
            1.0,
            0.0);
        glBegin(GL_QUADS);
        glVertex3f(
            0.0,
            1.0,
            0.0);
        glVertex3f(
            1.0,
            1.0,
            0.0);
        glVertex3f(
            1.0,
            1.0,
            -floor_width);
        glVertex3f(
            0.0,
            1.0,
            -floor_width);
        glEnd();
    }

    /* Is there a right edge? */
    if (x == maze->width - 1) {
        glNormal3f(
            1.0,
            0.0,
            0.0);
        glBegin(GL_QUADS);
        glVertex3f(
            1.0,
            1.0,
            0.0);
        glVertex3f(
            1.0,
            0.0,
            0.0);
        glVertex3f(
            1.0,
            0.0,
            -floor_width);
        glVertex3f(
            1.0,
            1.0,
            -floor_width);
        glEnd();
    }

    /* Is there a down edge? */
    if (y == maze->height - 1) {
        glNormal3f(
            0.0,
            -1.0,
            0.0);
        glBegin(GL_QUADS);
        glVertex3f(
            0.0,
            0.0,
            0.0);
        glVertex3f(
            0.0,
            0.0,
            -floor_width);
        glVertex3f(
            1.0,
            0.0,
            -floor_width);
        glVertex3f(
            1.0,
            0.0,
            0.0);
        glEnd();
    }
}

int
maze_render_gl(Maze *maze, double wall_width, double slope_width,
    int cx, int cy, unsigned int d, int flags)
{
    int x, y;

    /* Verify input parameters */
    if (!maze || wall_width < 0.0 || slope_width < 0.0
            || wall_width + slope_width > 0.5
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
                draw_floor(maze, x, y, 0.1);
            }
            glPopMatrix();
        }
    }

    return 1;
}
