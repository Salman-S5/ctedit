#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define MAX_LINES 80
#define INITIAL_LINE_CAPACITY 100

int saveFile(const char *filename, char **lines, int lineCount) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Error opening file");
        return -1;
    }
    for (int i = 0; i < lineCount; i++) {
        if (lines[i]) {
            fprintf(fp, "%s\n", lines[i]);
        }
    }
    fclose(fp);
    return 0;
}



int editor_init(char *filename) {
    char *lines[MAX_LINES];
    int line_lengths[MAX_LINES];
    int line_capacity[MAX_LINES];

    for (int i = 0; i < MAX_LINES; i++) {
        lines[i] = malloc(INITIAL_LINE_CAPACITY);
        lines[i][0] = '\0';
        line_lengths[i] = 0;
        line_capacity[i] = INITIAL_LINE_CAPACITY;
    }

    int ch;
    int posY = 0;
    int posX = line_lengths[posY];

    initscr();
    raw();
    keypad(stdscr, true);
    noecho();

    while (1) {
        ch = getch();

        if (ch == 17) break; // Ctrl+Q


        else if (ch == 19) {  // Ctrl+S 
            if (filename == NULL || filename[0] == '\0') {
                mvprintw(MAX_LINES + 1, 0, "No filename specified. Save aborted.");
            } else {
                if (saveFile(filename, lines, posY + 1) == 0) {
                    mvprintw(MAX_LINES + 1, 0, "File saved.");
                } else {
                    mvprintw(MAX_LINES + 1, 0, "Error saving file.");
                }
            }
            clrtoeol();
            refresh();
            napms(1000);
            move(posY, posX);
            clrtoeol();
        }

        else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (posX > 0) {
                posX--;
                lines[posY][posX] = '\0';
                line_lengths[posY] = posX;
            }
            else if (posY > 0) {
                posY--;
                posX = line_lengths[posY];
                if (posX > 0) {
                    posX--;
                    lines[posY][posX] = '\0';
                    line_lengths[posY] = posX;
                }
            }

        }

        else if (ch == '\n') {
            posY++;
            posX = 0;
        }
        
        else if (ch >= 32 && ch <= 126) {
            if (line_lengths[posY] + 1 >= line_capacity[posY]) {
                line_capacity[posY] *= 2;
                lines[posY] = realloc(lines[posY], line_capacity[posY]);
            }

            lines[posY][posX++] = ch;
            lines[posY][posX] = '\0';
            line_lengths[posY] = posX;
        }

        clear();
        for (int i = 0; i <= posY; i++) {
            mvprintw(i, 0, "%s", lines[i]);
        }
        move(posY, posX);
        refresh();
    }

    for (int i = 0; i < MAX_LINES; i++) {
        free(lines[i]);
    }

    endwin();
    return 0;
}

