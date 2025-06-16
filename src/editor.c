#include "editor.h"
#include <stdio.h>
#include <ncurses.h>

void editor_init() {
    int ch;

    initscr();
    raw();
    keypad(stdscr, true);
    noecho();

    printw("type to make bold");
    ch = getch();

    if (ch == KEY_F(1)) 
        printw("F1 key pressed");

    else {
        printw("The pressed key is ");
        attron(A_BOLD);
        printw("%c\n", ch);
        attroff(A_BOLD);
    }

    refresh();
    getch();

    endwin();
}

