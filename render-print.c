#include <stdio.h>

#include "maze-render.h"

void
maze_render_print(Maze *maze, unsigned int room_width, unsigned int room_height,
    char wall_char, char floor_char)
{
    int x, y;

    for (y = 0; y < maze->height * room_height; y++) {
        for (x = 0; x < maze->width * room_width; x++) {
            int rx = x / room_width;
            int ry = y / room_height;
            int dx = x % room_width;
            int dy = y % room_height;
            char c;

            if (dy == 0) {
                if (dx == 0 || dx == room_width - 1) {
                    c = wall_char;
                }
                else {
                    c = maze_is_open_up(maze, rx, ry)
                        ? floor_char
                        : wall_char;
                }
            }
            else if (dy == room_height - 1) {
                if (dx == 0 || dx == room_width - 1) {
                    c = wall_char;
                }
                else {
                    c = maze_is_open_down(maze, rx, ry)
                        ? floor_char
                        : wall_char;
                }
            }
            else {
                if (dx == 0) {
                    c = maze_is_open_left(maze, rx, ry)
                        ? floor_char
                        : wall_char;
                }
                else if (dx == room_width - 1) {
                    c = maze_is_open_right(maze, rx, ry)
                        ? floor_char
                        : wall_char;
                }
                else {
                    c = floor_char;
                }
            }
            printf("%c", c);
        }

        printf("\n");
    }
}
