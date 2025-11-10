#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <signal.h>

#define MAX_LINES 100
#define INITIAL_LINE_CAPACITY 100

volatile sig_atomic_t window_resized = 0;

void handle_winch(int sig) {
    window_resized = 1;
}

// TODO: Create struct for EditorState data to reduce 

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

int handle_max_line_check(int pos_y, int max_line_with_content) {
    if (pos_y > max_line_with_content) {
        return pos_y;
    } else {
        return max_line_with_content;
    }
}

void display_message(int pos_y, int pos_x, char *message) {
    mvprintw(pos_y, pos_x, "%s", message);
    clrtoeol();
    refresh();
}

void check_if_window_resized(int max_x, int max_y) {
    if (window_resized) {
        refresh();
        clear();
        getmaxyx(stdscr, max_y, max_x);
        // debug
        FILE *debug = fopen("debug.log", "a");
        fprintf(debug, "window resized: %d x %d\n", max_y, max_x);
        fclose(debug);
        window_resized = 0;
    }
}

void handle_cursor_movement(int ch, int *pos_x, int *pos_y, int *length_of_lines, int *max_y) {
    switch (ch) {
        case KEY_LEFT:
            if (*pos_x > 0) (*pos_x)--;

            break;
        case KEY_RIGHT:
            if (*pos_x < length_of_lines[*pos_y]) (*pos_x)++; 
            break;
        case KEY_DOWN:
            if (*pos_y + 1 < MAX_LINES) {
                (*pos_y)++;

                if (*pos_y + 3 > *max_y) {
                    *max_y += 1;
                }

                // debug
                FILE *debug = fopen("debug.log", "a");
                fprintf(debug, "pos_y: %d, max_y: %d\n", *pos_y, *max_y);
                fclose(debug);


                if (*pos_x > length_of_lines[*pos_y]) {
                    *pos_x = length_of_lines[*pos_y];
                }
            }
            break;
        case KEY_UP:
            if (*pos_y > 0) {
                (*pos_y)--;

                if (*pos_x > length_of_lines[*pos_y]) {
                    *pos_x = length_of_lines[*pos_y];
                }
            }
            break;
    }
}


int handle_command(int ch, char *file_name, int *pos_x, int *pos_y, int *max_y, char **lines, int *max_line_with_content, int *length_of_lines) {
    // TODO: Convert to switch statement
    
    if (ch == 17) return 1; // Ctrl+Q

    else if (ch == 19) { // Ctrl + S 
        int msg_pos_x = 0;
        int msg_pos_y = *max_y - 1;
        if (file_name == NULL || file_name[0] == '\0') {
            display_message(msg_pos_y, msg_pos_x, "No filename specified. Save aborted.");
        } else {
            if (saveFile(file_name, lines, *pos_y + 1) == 0) {
                display_message(msg_pos_y, msg_pos_x, "File saved");
            } else {
                display_message(msg_pos_y, msg_pos_x, "Error saving file");
            }
        }

        napms(1000);
        move(*pos_y, 0);
        clrtoeol();
        move(*pos_y, *pos_x);  
    }

    else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
        if (*pos_x > 0) {
            (*pos_x)--;
            lines[*pos_y][*pos_x] = '\0';
            length_of_lines[*pos_y] = *pos_x;
        }
        else if (*pos_y > 0) {
            (*pos_y)--;
            *pos_x = length_of_lines[*pos_y];
            if (pos_x > 0) {
                lines[*pos_y][*pos_x] = '\0';
                (*pos_x)--;
                length_of_lines[*pos_y] = *pos_x;
            }
        }
    }

    else if (ch == '\n') {
        *max_line_with_content = handle_max_line_check(*pos_y, *max_line_with_content);
        (*pos_y)++;
        *pos_x = 0;
    }
}

void handle_text_input(
    int ch, 
    int *pos_x, 
    int *pos_y, 
    int *total_lines, 
    int *length_of_lines, 
    char **lines, 
    int *max_line_with_content
) {
    if (ch >= 32 && ch <= 126) {
        if (*pos_x >= total_lines[*pos_y] - 1) {
            total_lines[*pos_y] *= 2;
            lines[*pos_y] = realloc(lines[*pos_y], total_lines[*pos_y]);
        }

        lines[*pos_y][*pos_x] = ch;
        (*pos_x)++;
        lines[*pos_y][*pos_x] = '\0';
        length_of_lines[*pos_y] = *pos_x;

        *max_line_with_content = handle_max_line_check(*pos_y, *max_line_with_content);
    }
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
    int pos_y = 0;
    int pos_x = 0;
    
    initscr();
    signal(SIGWINCH, handle_winch);
    raw();
    noecho();
    keypad(stdscr, TRUE);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    refresh();

    while (1) {
        check_if_window_resized(max_x, max_y);

        ch = getch();

        mvprintw(max_y - 1, 0, "%d", ch); 
        
        handle_cursor_movement(ch, &pos_x, &pos_y, length_of_lines, &max_y);



        handle_text_input(ch, &pos_x, &pos_y, total_lines, length_of_lines, lines, &max_line_with_content);

        // Draws lines
        if (pos_x < max_y) {
            for (int i = 0; i <= max_line_with_content; i++) {
                mvprintw(i, 0, "%s", lines[i]);
                clrtoeol();
            }
        } else {
            for (int i = pos_x - max_y; i <= max_line_with_content; i++) {
                mvprintw(i, 0, "%s", lines[i]);
                clrtoeol();
            }
        }

        move(pos_y, pos_x);    
        refresh();
    }

    for (int i = 0; i < MAX_LINES; i++) {
        free(lines[i]);
    }
    signal(SIGSEGV, handle_sigsegv);
    atexit(cleanup);
    return 0;
}

