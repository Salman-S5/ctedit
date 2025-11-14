# Todo List – Terminal Text Editor (ncurses, C)

## Main Priority
- ## Core Features
  - [x] Initialize ncurses
  - [x] Accept user input and display it
  - [x] File loading (`open` command or startup filename arg)
  - [x] File saving (with Ctrl+S)
  - [ ] Prompt warning on quit (Ctrl+Q)
  - [ ] Basic editing (insert, delete, backspace)
  - [x] Handle cursor movement (arrow keys / hjkl)
  - [ ] Scrolling (when text overflows terminal)

## Extras
- ## Enhancements
  - [ ] Line numbers on the side
  - [ ] Status bar (mode, filename, cursor position)
  - [ ] Support for different modes (Insert / Command, Vim-like)
  - [ ] Undo / redo stack
  - [ ] Syntax highlighting (basic, e.g., keywords, comments)

- ## Quality of Life
  - [ ] Configurable settings (tab size, color scheme)
  - [ ] Search (`/pattern` like in Vim)
  - [ ] On command (Ctrl+Shift+S), save + close file and open current directory in terminal 

- ## Optional / Future Features
  - [ ] Split view or multiple buffers
  - [ ] Plugin system or scripting hooks
  - [ ] UTF-8/multibyte character support
