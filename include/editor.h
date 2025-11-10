#ifndef EDITOR_H
#define EDITOR_H

// Helper functions
void handle_winch(int sig);
int saveFile(const char *filename, char **lines, int lineCount);
void cleanup();
void handle_sigsegv(int sig);
int handle_max_line_check(int pos_y, int max_line_with_content);
void display_message(int pos_y, int pos_x, char *message);

int editor_init(char *filename);


#endif