#include "editor.h"
#include <stdio.h>
#include <ncurses.h>

void editor_init() {
    int ch;
    char buffer[1024] = {0};
    int posX = 0;

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
                buffer[posX] = '\0';
                move(1, 0);
                clrtoeol();
                printw("%s", buffer);
                move(1, posX);
            }
        }
        
        else if (ch >= 32 && ch <= 126) {
            if (posX < sizeof(buffer) - 1) {
                buffer[posX++] = ch;
                buffer[posX] = '\0';
                mvprintw(1, 0, "%s", buffer);
                move(1, posX);
            }
        }
        refresh();
    }


    endwin();
}

