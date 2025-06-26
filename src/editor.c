#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <signal.h>

#define MAX_LINES 100
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

void cleanup() {
    endwin();
}

void handle_sigsegv(int sig) {
    cleanup();
    exit(1);
}

int handle_max_line_check(int posY, int max_line_with_content) {
    if (posY > max_line_with_content) {
        return posY;
    } else {
        return max_line_with_content;
    }
}

void display_message(int posY, int posX, char *message) {
    mvprintw(posY, posX, "%s", message);
    clrtoeol();
    refresh();
}

int editor_init(char *filename) {
    char *lines[MAX_LINES];
    int length_of_lines[MAX_LINES];
    int total_lines[MAX_LINES];
    int max_line_with_content = 0;

    for (int i = 0; i < MAX_LINES; i++) {
        lines[i] = malloc(INITIAL_LINE_CAPACITY);
        lines[i][0] = '\0';
        total_lines[i] = INITIAL_LINE_CAPACITY;
        length_of_lines[i] = 0;
    }

    int ch;
    int posY = 0;
    int posX = 0;


    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    refresh();

    while (1) {
        ch = getch();
        mvprintw(max_y - 1, 0, "%d", ch); 
        

        switch (ch) {
            case KEY_LEFT:
                if (posX > 0) posX--;

                break;
            case KEY_RIGHT:
                if (posX < length_of_lines[posY]) posX++; 
                break;
            case KEY_DOWN:
                if (posY + 1 < MAX_LINES) {
                    posY++;

                    if (posX > length_of_lines[posY]) {
                        posX = length_of_lines[posY];
                    }
                }
                break;
            case KEY_UP:
                if (posY > 0) {
                    posY--;

                    if (posX > length_of_lines[posY]) {
                        posX = length_of_lines[posY];
                    }
                }
                break;
        }

        if (ch == 17) break; // Ctrl+Q

        else if (ch == 19) {  // Ctrl+S 
            if (filename == NULL || filename[0] == '\0') {
                display_message(max_y - 1, 0, "No filename specified. Save aborted.");
            } else {
                if (saveFile(filename, lines, posY + 1) == 0) {
                    display_message(max_y - 1, 0, "File saved");
                } else {
                    display_message(max_y - 1, 0, "Error saving file");
                }
            }

            napms(1000);
            move(posY, 0);
            clrtoeol();
            move(posY, posX);  
        }


        else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) { // Backspace
            if (posX > 0) {
                posX--;
                lines[posY][posX] = '\0';
                length_of_lines[posY] = posX;
            }
            else if (posY > 0) {
                posY--;
                posX = length_of_lines[posY];
                if (posX > 0) {
                    lines[posY][posX] = '\0';
                    posX--;
                    length_of_lines[posY] = posX;
                }
            }
        }


        else if (ch == '\n') { // Enter
            max_line_with_content = handle_max_line_check(posY, max_line_with_content);
            posY++;
            posX = 0;
        }
        

        else if (ch >= 32 && ch <= 126) { // Text
            if (posX >= total_lines[posY] - 1) {
                total_lines[posY] *= 2;
                lines[posY] = realloc(lines[posY], total_lines[posY]);
            }

            lines[posY][posX] = ch;
            posX++;
            lines[posY][posX] = '\0';
            length_of_lines[posY] = posX;

            max_line_with_content = handle_max_line_check(posY, max_line_with_content);
        }


        for (int i = 0; i <= max_line_with_content; i++) { // Draw lines
            mvprintw(i, 0, "%s", lines[i]);
            clrtoeol();
        }


        move(posY, posX);    
        refresh();
    }

    for (int i = 0; i < MAX_LINES; i++) {
        free(lines[i]);
    }
    
    signal(SIGSEGV, handle_sigsegv);
    atexit(cleanup);
    return 0;
}

