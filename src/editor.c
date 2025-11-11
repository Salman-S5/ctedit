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

typedef struct {
    int pos_y, pos_x;
    int max_y, max_x;
    char *lines[MAX_LINES];
    int length_of_lines[MAX_LINES];
    int total_lines[MAX_LINES];
    int max_line_with_content;
} EditorState;

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

void free_mem(EditorState *state) {
    for (size_t i = 0; i < sizeof(LINES); i++) {
        free(state->lines[i]);
    }
    free(state);
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

void check_if_window_resized(EditorState *state) {
    if (window_resized) {
        refresh();
        clear();
        getmaxyx(stdscr, state->max_y, state->max_x);
        // debug
        FILE *debug = fopen("debug.log", "a");
        fprintf(debug, "window resized: %d x %d\n", state->max_y, state->max_x);
        fclose(debug);
        window_resized = 0;
    }
}

void handle_cursor_movement(int ch, EditorState *state) {
    int y = state->pos_y;
    int x = state->pos_x;
    switch (ch) {
        case KEY_LEFT:
            if (state->pos_x > 0) (x)--;
            break;
        case KEY_RIGHT:
            if (x < state->length_of_lines[y]) (x)++; 
            break;
        case KEY_DOWN:
            if (y + 1 < MAX_LINES) {
                (y)++;

                if (y + 3 > state->max_y) {
                    state->max_y += 1;
                }
                // debug
                FILE *debug = fopen("debug.log", "a");
                fprintf(debug, "pos_y: %d, max_y: %d\n", y, state->max_y);
                fclose(debug);

                if (x > state->length_of_lines[y]) {
                    x = state->length_of_lines[y];
                }
            }
            break;
        case KEY_UP:
            if (y > 0) {
                (y)--;

                if (x > state->length_of_lines[y]) {
                    x = state->length_of_lines[y];
                }
            }
            break;
    }
}

int handle_command(int ch, char *file_name, EditorState *state) {
    // TODO: Convert to switch statement
    int y = state->pos_y;
    int x = state->pos_x;
    switch(ch) {
        case 17:
            return 1;
            break;
        case 19:
            int msg_pos_x = 0;
            int msg_pos_y = state->max_y - 1;
            if (file_name == NULL || file_name[0] == '\0') {
                display_message(msg_pos_y, msg_pos_x, "No filename specified. Save aborted.");
            } else {
                if (saveFile(file_name, state->lines, y + 1) == 0) {
                    display_message(msg_pos_y, msg_pos_x, "File saved");
                } else {
                    display_message(msg_pos_y, msg_pos_x, "Error saving file");
                }
            }
        
            napms(1000);
            move(y, 0);
            clrtoeol();
            move(y, x);  
            
            break;
        case KEY_BACKSPACE || 127 || 8:
            if (x > 0) {
                x--;
                state->lines[y][x] = '\0';
                state->length_of_lines[y] = x;
            }
            else if (y > 0) {
                (y)--;
                x = state->length_of_lines[y];
                if (x > 0) {
                    state->lines[y][x] = '\0';
                    x--;
                    state->length_of_lines[y] = x;
                }
            }

            break;
        case '\n':
            state->max_line_with_content = handle_max_line_check(y, state->max_line_with_content);
            y++;
            x = 0;
    }

    return 0;
}

void handle_text_input(int ch, EditorState *state) {
    if (ch >= 32 && ch <= 126) {
        int y = state->pos_y;
        int x = state->pos_x;
        if (x >= state->total_lines[y] - 1) {
            state->total_lines[y] *= 2;
            state->lines[y] = realloc(state->lines[y], state->total_lines[y]);
        }

        state->lines[y][x] = ch;
        x++;
        state->lines[y][x] = '\0';
        state->length_of_lines[y] = x;

        state->max_line_with_content = handle_max_line_check(y, state->max_line_with_content);
    }
}

void render_editor(EditorState *state) {
    // Draws lines
    if (state->pos_x < state->max_y) {
        for (int i = 0; i <= state->max_line_with_content; i++) {
            mvprintw(i, 0, "%s", state->lines[i]);
            clrtoeol();
        }
    } else {
        for (int i = state->pos_x - state->max_y; i <= state->max_line_with_content; i++) {
            mvprintw(i, 0, "%s", state->lines[i]);
            clrtoeol();
        }
    }
}

int editor_init(char *file_name) {
    EditorState *state = malloc(sizeof(EditorState));
    if (state == NULL) return -1;
    
    char *lines[MAX_LINES];
    int length_of_lines[MAX_LINES];
    int total_lines[MAX_LINES];
    int ch;

    state->max_line_with_content = 0;
    state->pos_y = 0;
    state->pos_x = 0;

    for (int i = 0; i < MAX_LINES; i++) {
        state->lines[i] = malloc(INITIAL_LINE_CAPACITY);
        state->lines[i][0] = '\0';
        state->total_lines[i] = INITIAL_LINE_CAPACITY;
        state->length_of_lines[i] = 0;
    }

    initscr();
    signal(SIGWINCH, handle_winch);
    raw();
    noecho();
    keypad(stdscr, TRUE);

    getmaxyx(stdscr, state->max_y, state->max_x);

    refresh();

    while (1) {
        check_if_window_resized(state);

        ch = getch();

        mvprintw(state->max_y - 1, 0, "%d", ch); 
        
        handle_cursor_movement(ch, state);
        handle_text_input(ch, state);
        handle_command(ch, file_name, state);
        render_editor(state);

        move(state->pos_y, state->pos_x);    
        refresh();
    }

    free_mem(state);
    signal(SIGSEGV, handle_sigsegv);
    atexit(cleanup);
    return 0;
}

