#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <signal.h>

#define MAX_LINES 100
#define INITIAL_LINE_CAPACITY 100

#define CTRL_A ('a' & 0x1F)
#define CTRL_B ('b' & 0x1F)

volatile sig_atomic_t window_resized = 0;

void handle_winch(int sig) {
    window_resized = 1;
}

typedef struct {
    int pos_y, pos_x; // cursor positions
    int max_y, max_x; // window sizes
    int viewport_offset; // top line being displayed
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

void print(const char *format, ...) {
    va_list args;
    va_start(args, format);
    FILE *debug = fopen("debug.log", "a");
    if (debug == NULL) {
        perror("Failed to open file");
    }
    vfprintf(debug, format, args);
    fclose(debug);
    va_end(args);
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
    switch (ch) {
        case KEY_LEFT:
        case 104:
            print("x: %d, before left press", state->pos_x);
            if (state->pos_x > 0) state->pos_x--;
            print(" '\' KEY_LEFT Pressed x=%d\n", state->pos_x);
            break;
        case KEY_RIGHT:
        case 108:
            print("x: %d, before right press", state->pos_x);
            if (state->pos_x < state->length_of_lines[state->pos_y]-1) state->pos_x++; 
            print(" '\' KEY_RIGHT Pressed x=%d\n", state->pos_x);
            
            break;
        case KEY_DOWN:
        case 107:
            if (state->pos_y + 1 < MAX_LINES) {
                state->pos_y++;

                if (state->pos_y + 2 > state->max_y) {
                    state->max_y += 1;
                }
                print("pos_y: %d, max_y: %d\n", state->pos_y, state->max_y);

                if (state->pos_x > state->length_of_lines[state->pos_y]) {
                    state->pos_x = state->length_of_lines[state->pos_y];
                }
            }
            break;
        case KEY_UP:
        case 106:
            if (state->pos_y > 0) {
                state->pos_y--;

                print("pos_y: %d, max_y: %d\n", state->pos_y, state->max_y);

                if (state->pos_x > state->length_of_lines[state->pos_y]) {
                    state->pos_x = state->length_of_lines[state->pos_y];
                }
            }
            break;
    }
}

void handle_text_input(int ch, EditorState *state) {
    if (ch >= 32 && ch <= 126) {
        int y = state->pos_y;
        int x = state->pos_x;
        if (x >= state->total_lines[y] - 1) {
            state->total_lines[y] *= 2;
            state->lines[y] = realloc(state->lines[y], state->total_lines[y]);
        }

        
        for (int i=state->length_of_lines[y]; i>x; i--) {
            state->lines[y][i] = state->lines[y][i - 1];
        }
        state->lines[y][x] = ch;
        x++;
        state->pos_x = x;
        state->length_of_lines[y]++;
        
        state->max_line_with_content = handle_max_line_check(y, state->max_line_with_content);
    }
}

int handle_command(int ch, char *file_name, EditorState *state) {
    int y = state->pos_y;
    int x = state->pos_x;
    print("%d\n", ch);
    switch(ch) {
        case 17:
            print("Quitting...\n");
            return 1;
            break;
        case 19:
            print("Attempting file save...\n");
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
        case KEY_BACKSPACE:
        case 127:
        case 8:
            print("Attempting ch deletion...\n");
            if (x > 0) {
                if (x == state->length_of_lines[y]) {
                    state->lines[y][x-1] = '\0';
                    state->length_of_lines[y]--;
                    state->pos_x--;
                }
                else {
                    for (int i=x-1; i < state->length_of_lines[y]; i++) {
                        state->lines[y][i] = state->lines[y][i + 1];
                    }

                    // state->lines[y][x] = '\0';
                    
                    state->length_of_lines[y]--;
                    state->pos_x--;
                }
            }
            else if (y > 0) {
                y--;
                state->pos_y = y;
                x = state->length_of_lines[y];
                if (x > 0) {
                    state->lines[y][x] = '\0';
                    x--;
                    state->pos_x = x;
                    state->length_of_lines[y] = x;
                }
            }

            break;
        case '\n':
            print("Attempting next line...\n");
            state->max_line_with_content = handle_max_line_check(y, state->max_line_with_content);
            state->pos_y++;
            state->pos_x = 0;
            break;
        case 9:
            int spaces = 4;
            while (spaces--) {
                handle_text_input(32, state);
            }
            break;
        case CTRL_A:
            print("max_y: %d\n", state->max_y);
            break;
        case KEY_MOUSE:
            MEVENT event;
            if (getmouse(&event) == OK) {
                // print("bstate: %lu\n", event.bstate);
                if (event.bstate & BUTTON4_PRESSED) { // scroll up
                    if (state->viewport_offset > 0) {
                        state->viewport_offset--;
                        // print("scrolling up");
                    }
                }
                else if (event.bstate & BUTTON5_PRESSED) { // scroll down
                    if (state->viewport_offset < state->max_line_with_content + 5) {
                        state->viewport_offset++;
                        // print("scrolling down");
                    }
                }
            }
            break;
    }

    return 0;
}



void render_editor(EditorState *state) {
    // any reason for a base case?
    int screen_row = 0;
    for (int i=state->viewport_offset; i<state->viewport_offset+state->max_y-2; i++) {
        mvprintw(screen_row, 0, "%s", state->lines[i]);
        clrtoeol();
        screen_row++;
    }
    refresh();
}

int editor_init(char *file_name) {
    EditorState *state = malloc(sizeof(EditorState));
    if (state == NULL) return -1;

    int ch;
    // print("%s\n", ch);

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
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    keypad(stdscr, TRUE);

    getmaxyx(stdscr, state->max_y, state->max_x);
    
    
    refresh();
    
    while (1) {
        check_if_window_resized(state);
        
        ch = getch();
        
        // mvprintw(state->max_y - 1, 0, "%d", ch); 
        // clrtoeol();

        // print("first line index: %d\n", state->viewport_offset);
        // print("last line index: %d\n", state->viewport_offset+state->max_y-2);
        
        handle_cursor_movement(ch, state);
        handle_text_input(ch, state);
        if (handle_command(ch, file_name, state) == 1) break;
        render_editor(state);
        
        mvprintw(state->max_y - 1, 0, "x: %d y: %d", state->pos_x, state->pos_y); 
        clrtoeol();

        move(state->pos_y - state->viewport_offset, state->pos_x);    
        refresh();
    }

    free_mem(state);
    signal(SIGSEGV, handle_sigsegv);
    atexit(cleanup);
    return 0;
}

