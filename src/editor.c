#include "editor.h"
#include <stdio.h>
#include <ncurses.h>

#define INITIAL_LINE_CAPACITY 80
#define INITIAL_LINE_CAPACITY 100


void editor_init() {

    char *lines[INITIAL_LINE_CAPACITY];
    lines[0] = malloc(80);

    int line_lengths[INITIAL_LINE_CAPACITY];
    int line_capacity[INITIAL_LINE_CAPACITY];

    int ch;
    int posY = 0;
    int posX = line_lengths[posY];

    initscr();
    raw();
    keypad(stdscr, true);
    noecho();

    while (1) {
        ch = getch();

        if (ch == 17) break;

        else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (posX > 0) {
                posX--;
                lines[posY][posX] = '\0';
                move(posY, 0);
                clrtoeol();
                for (int i = 0; i < sizeof(lines); i++) {
                    printw("%s", lines[i]);
                }
                move(posY, posX);
            }
        }

        else if (ch == '\n') {
            posY++;
            move(posY, 0);
        }
        
        else if (ch >= 32 && ch <= 126) {
            if (posX < sizeof(lines[posX]) - 1) {
                lines[posY][posX++] = ch;
                lines[posY][posX+1] = '\0';
                for (int i = 0; i < sizeof(lines); i++) {
                    mvprintw(0, 0, "%s", lines[i]);
                }
                move(posY, posX);
            }
        }

        refresh();
    }


    endwin();
}

