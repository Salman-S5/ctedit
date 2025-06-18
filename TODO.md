# Todo List – Terminal Text Editor (ncurses, C)

## Main Priority
- ## Core Features
  - [x] Initialize ncurses
  - [x] Accept user input and display it
  - [x] File loading (`open` command or startup filename arg)
  - [x] File saving (with Ctrl+S)
  - [ ] Basic editing (insert, delete, backspace)
  - [ ] Handle cursor movement (arrow keys / hjkl)
  - [ ] Scrolling (when text overflows terminal)

## Extras
- ## Enhancements
  - [ ] Line numbers on the side
  - [ ] Status bar (mode, filename, cursor position)
  - [ ] Support for different modes (Insert / Command, Vim-like)
  - [ ] Undo / redo stack
  - [ ] Syntax highlighting (basic, e.g., keywords, comments)

- ## Quality of Life
  - [ ] Keybindings (Ctrl+Q to quit, Ctrl+S to save, etc.)
  - [ ] Configurable settings (tab size, color scheme)
  - [ ] Mouse support (optional via `ncurses` if terminal supports it)
  - [ ] Search (`/pattern` like in Vim)

- ## Optional / Future Features
  - [ ] Split view or multiple buffers
  - [ ] Plugin system or scripting hooks
  - [ ] Macro recording & replay
  - [ ] UTF-8/multibyte character support
