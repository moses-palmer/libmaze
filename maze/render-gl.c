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
 * @param tx1, ty1
 *     The texture coordinates to set for the first coordinate. This is only
 *     used if texture is non-zero.
 * @param x2, y2, z2
 *     The coordinates of the bottom left corner.
 * @param tx2, ty2
 *     The texture coordinates to set for the second coordinate. This is only
 *     used if texture is non-zero.
 * @param x3, y3, z3
 *     The coordinates of the bottom right corner.
 * @param tx3, ty3
 *     The texture coordinates to set for the third coordinate. This is only
 *     used if texture is non-zero.
 * @param x4, y4, z4
 *     The coordinates of the top right corner.
 * @param tx4, ty4
 *     The texture coordinates to set for the fourth coordinate. This is only
 *     used if texture is non-zero.
 * @param render_texture
 *     Whether to specify texture coordinates and apply a texture.
 */
static inline void
rectangle(
    double x1, double y1, double z1,
    double tx1, double ty1,
    double x2, double y2, double z2,
    double tx2, double ty2,
    double x3, double y3, double z3,
    double tx3, double ty3,
    double x4, double y4, double z4,
    double tx4, double ty4,
    int render_texture)
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
    if (render_texture) glTexCoord2f(tx1, ty1);
    glVertex3f(x1, y1, z1);
    if (render_texture) glTexCoord2f(tx2, ty2);
    glVertex3f(x2, y2, z2);
    if (render_texture) glTexCoord2f(tx3, ty3);
    glVertex3f(x3, y3, z3);
    if (render_texture) glTexCoord2f(tx1, ty1);
    glVertex3f(x1, y1, z1);
    if (render_texture) glTexCoord2f(tx3, ty3);
    glVertex3f(x3, y3, z3);
    if (render_texture) glTexCoord2f(tx4, ty4);
    glVertex3f(x4, y4, z4);
    glEnd();
}

/**
 * Defines the vertices of a wall.
 *
 * It is added to the bottom of the current view.
 *
 * @param down
 *     The name of the direction that should be rendered as the bottom wall.
 * @param left
 *     The name of the direction one step clockwise to down.
 */
#define HANDLE_WALL(down, left, right) \
    do { \
        int is_open = maze_is_open_##down(maze, x, y); \
        if (!is_open) { \
            int is_lcorner = !maze_is_open_##left(maze, x, y); \
            int is_rcorner = !maze_is_open_##right(maze, x, y); \
            rectangle( \
                is_lcorner ? wall_width + slope_width : 0.0, \
                    wall_width + slope_width, 0.0, \
                is_lcorner ? 1.0 - wall_width - slope_width : 1.0, 0.0, \
                0.0, wall_width, 1.0, \
                1.0, 1.0, \
                1.0, wall_width, 1.0, \
                0.0, 1.0, \
                is_rcorner ? 1.0 - wall_width - slope_width : 1.0, \
                    wall_width + slope_width, 0.0, \
                is_rcorner ? wall_width + slope_width : 0.0, 0.0, \
                render_texture); \
        } \
        else if (maze_is_corner_##down##_##left##_out(maze, x, y)) { \
            rectangle( \
                0.0, wall_width + slope_width, 0.0, \
                1.0, 0.0, \
                0.0, wall_width, 1.0, \
                1.0, 1.0, \
                wall_width, wall_width, 1.0, \
                1.0 - wall_width, 1.0, \
                wall_width + slope_width, wall_width + slope_width, 0.0, \
                1.0 - wall_width - slope_width, 0.0, \
                render_texture); \
            rectangle( \
                wall_width, wall_width, 1.0, \
                wall_width, 1.0, \
                wall_width, 0.0, 1.0, \
                0.0, 1.0, \
                wall_width + slope_width, 0.0, 0.0, \
                0.0, 0.0, \
                wall_width + slope_width, wall_width + slope_width, 0.0, \
                wall_width + slope_width, 0.0, \
                render_texture); \
        } \
    } while (0)

/**
 * Translates the matrix so that we may handle the next wall clockwise.
 */
#define NEXT_WALL() \
    glTranslatef(0.5, 0.5, 0.0); \
    glRotatef(90.0, 0.0, 0.0, -1.0); \
    glTranslatef(-0.5, -0.5, 0.0)

static void
define_walls(Maze *maze, int x, int y, double wall_width, double slope_width,
    int render_texture)
{
    HANDLE_WALL(down, left, right);
    NEXT_WALL();

    HANDLE_WALL(left, up, down);
    NEXT_WALL();

    HANDLE_WALL(up, right, left);
    NEXT_WALL();

    HANDLE_WALL(right, down, up);
    NEXT_WALL();
}

static void
define_floor(Maze *maze, int x, int y, double floor_width, int render_texture)
{
    /* The top part */
    rectangle(
        0.0, 1.0, 0.0,
        0.0, 1.0,
        0.0, 0.0, 0.0,
        0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0, 0.0,
        1.0, 1.0,
        render_texture);

    /* The bottom part */
    rectangle(
        0.0, 1.0, -floor_width,
        0.0, 1.0,
        1.0, 1.0, -floor_width,
        1.0, 1.0,
        1.0, 0.0, -floor_width,
        1.0, 0.0,
        0.0, 0.0, -floor_width,
        0.0, 0.0,
        render_texture);

    /* Is there a left edge? */
    if (x == -1) {
        rectangle(
            0.0, 1.0, 0.0,
            1.0, 1.0,
            0.0, 1.0, -floor_width,
            1.0 - floor_width, 1.0,
            0.0, 0.0, -floor_width,
            1.0 - floor_width, 0.0,
            0.0, 0.0, 0.0,
            1.0, 0.0,
            render_texture);
    }

    /* Is there an up edge? */
    if (y == -1) {
        rectangle(
            0.0, 1.0, 0.0,
            0.0, 0.0,
            1.0, 1.0, 0.0,
            1.0, 0.0,
            1.0, 1.0, -floor_width,
            1.0, floor_width,
            0.0, 1.0, -floor_width,
            0.0, floor_width,
            render_texture);
    }

    /* Is there a right edge? */
    if (x == maze->width) {
        rectangle(
            1.0, 1.0, 0.0,
            0.0, 1.0,
            1.0, 0.0, 0.0,
            0.0, 0.0,
            1.0, 0.0, -floor_width,
            floor_width, 0.0,
            1.0, 1.0, -floor_width,
            floor_width, 1.0,
            render_texture);
    }

    /* Is there a down edge? */
    if (y == maze->height) {
        rectangle(
            0.0, 0.0, 0.0,
            0.0, 1.0,
            0.0, 0.0, -floor_width,
            0.0, 1.0 - floor_width,
            1.0, 0.0, -floor_width,
            1.0, 1.0 - floor_width,
            1.0, 0.0, 0.0,
            1.0, 1.0,
            render_texture);
    }
}

static void
define_top(Maze *maze, int x, int y, double wall_width, double slope_width,
    int render_texture)
{
    double ty = maze_is_open_up(maze, x, y) ? 1.0 : 1.0 - wall_width;
    double by = maze_is_open_down(maze, x, y) ? 0.0 : wall_width;

    /* The top */
    if (!maze_is_open_up(maze, x, y)) {
        rectangle(
            0.0, 1.0, 1.0,
            0.0, 1.0,
            0.0, 1.0 - wall_width, 1.0,
            0.0, 1.0 - wall_width,
            1.0, 1.0 - wall_width, 1.0,
            1.0, 1.0 - wall_width,
            1.0, 1.0, 1.0,
            1.0, 1.0,
            render_texture);
    }

    /* The bottom */
    if (!maze_is_open_down(maze, x, y)) {
        rectangle(
            0.0, wall_width, 1.0,
            0.0, wall_width,
            0.0, 0.0, 1.0,
            0.0, 0.0,
            1.0, 0.0, 1.0,
            1.0, 0.0,
            1.0, wall_width, 1.0,
            1.0, wall_width,
            render_texture);
    }

    /* The left */
    if (!maze_is_open_left(maze, x, y)) {
        rectangle(
            0.0, ty, 1.0,
            0.0, ty,
            0.0, by, 1.0,
            0.0, by,
            wall_width, by, 1.0,
            wall_width, by,
            wall_width, ty, 1.0,
            wall_width, ty,
            render_texture);
    }

    /* The right */
    if (!maze_is_open_right(maze, x, y)) {
        rectangle(
            1.0 - wall_width, ty, 1.0,
            1.0 - wall_width, ty,
            1.0 - wall_width, by, 1.0,
            1.0 - wall_width, by,
            1.0, by, 1.0,
            1.0, by,
            1.0, ty, 1.0,
            1.0, ty,
            render_texture);
    }

    /* The top left */
    if (maze_is_corner_up_left_out(maze, x, y)) {
        rectangle(
            0.0, 1.0, 1.0,
            0.0, 1.0,
            0.0, 1.0 - wall_width, 1.0,
            0.0, 1.0 - wall_width,
            wall_width, 1.0 - wall_width, 1.0,
            wall_width, 1.0 - wall_width,
            wall_width, 1.0, 1.0,
            wall_width, 1.0,
            render_texture);
    }

    /* The top right */
    if (maze_is_corner_up_right_out(maze, x, y)) {
        rectangle(
            1.0 - wall_width, 1.0, 1.0,
            1.0 - wall_width, 1.0,
            1.0 - wall_width, 1.0 - wall_width, 1.0,
            1.0 - wall_width, 1.0 - wall_width,
            1.0, 1.0 - wall_width, 1.0,
            1.0, 1.0 - wall_width,
            1.0, 1.0, 1.0,
            1.0, 1.0,
            render_texture);
    }

    /* The bottom left */
    if (maze_is_corner_down_left_out(maze, x, y)) {
        rectangle(
            0.0, wall_width, 1.0,
            0.0, wall_width,
            0.0, 0.0, 1.0,
            0.0, 0.0,
            wall_width, 0.0, 1.0,
            wall_width, 0.0,
            wall_width, wall_width, 1.0,
            wall_width, wall_width,
            render_texture);
    }

    /* The bottom right */
    if (maze_is_corner_down_right_out(maze, x, y)) {
        rectangle(
            1.0 - wall_width, wall_width, 1.0,
            1.0 - wall_width, wall_width,
            1.0 - wall_width, 0.0, 1.0,
            1.0 - wall_width, 0.0,
            1.0, 0.0, 1.0,
            1.0, 0.0,
            1.0, wall_width, 1.0,
            1.0, wall_width,
            render_texture);
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
        for (x = cx - (int)d; x <= cx + (int)d; x++) {
            glPushMatrix();
            glTranslatef(x, (int)maze->height - 1 - y, 0.0);
            if (flags & MAZE_RENDER_GL_WALLS) {
                define_walls(maze, x, y, wall_width, slope_width,
                    flags & MAZE_RENDER_GL_TEXTURE);
            }
            if (flags & MAZE_RENDER_GL_FLOOR
                    && maze_edge_contains(maze, x, y)) {
                define_floor(maze, x, y, floor_thickness,
                    flags & MAZE_RENDER_GL_TEXTURE);
            }
            if (flags & MAZE_RENDER_GL_TOP) {
                define_top(maze, x, y, wall_width, slope_width,
                    flags & MAZE_RENDER_GL_TEXTURE);
            }
            glPopMatrix();
        }
    }

    return 1;
}
