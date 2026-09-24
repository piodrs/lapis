#include <stdio.h>
#include <curses.h>

#include "defs.h"
#include "editor.h"
#include "screen.h"

int screen_input(Editor *ed, int ch)
{
	switch (ch) {
	case KEY_LEFT:
		if (ed->col > 0)
			--ed->col;
		else if (ed->row > 0)
			ed->col = ed->rows[--ed->row].len;
		break;
	case KEY_RIGHT:
		if (ed->col < ed->rows[ed->row].len)
			++ed->col;
		else if (ed->row + 1 < ed->len) {
			++ed->row;
			ed->col = 0;
		}
		break;
	case KEY_UP:
		if (ed->row > 0)
			--ed->row;
		break;
	case KEY_DOWN:
		if (ed->row + 1 < ed->len)
			++ed->row;
		break;
	case KEY_BACKSPACE:
	case KEY_DEL:
	case KEY_CTRL('h'):
		return editor_backspace(ed);
	case KEY_DC:
		return editor_delete(ed);
	case '\r':
	case '\n':
	case KEY_ENTER:
		return editor_split(ed);
	default:
		if (ch >= ' ' && ch <= '~')
			return editor_insert(ed, ch);
		break;
	}
	if (ed->col > ed->rows[ed->row].len)
		ed->col = ed->rows[ed->row].len;
	return 0;
}

int screen_draw(const Editor *ed)
{
	int rows;
	int cols;
	int row;
	int col;
	int i;

	getmaxyx(stdscr, rows, cols);
	if (erase() == ERR)
		return 1;
	for (i = 0; i < rows && (size_t)i < ed->len; ++i) {
		if (mvaddnstr(i, 0, ed->rows[i].text, cols - 1) == ERR)
			return 1;
	}
	row = ed->row < (size_t)rows ? (int)ed->row : rows - 1;
	col = ed->col < (size_t)cols ? (int)ed->col : cols - 1;
	return move(row, col) == ERR || refresh() == ERR;
}

const char *screen_run(void)
{
	SCREEN *screen;
	Editor ed;
	const char *error;
	int ch;

	if (editor_init(&ed))
		return "cannot allocate editor";
	screen = newterm(NULL, stdout, stdin);
	if (screen == NULL) {
		editor_free(&ed);
		return "cannot initialize terminal";
	}
	error = NULL;
	if (raw() == ERR || noecho() == ERR || keypad(stdscr, TRUE) == ERR) {
		error = "cannot configure terminal";
	} else {
		for (;;) {
			if (screen_draw(&ed)) {
				error = "cannot draw editor";
				break;
			}
			ch = getch();
			if (ch == KEY_CTRL('q'))
				break;
			if (ch == ERR) {
				error = "cannot read input";
				break;
			}
			if (screen_input(&ed, ch)) {
				error = "cannot grow text";
				break;
			}
		}
	}
	if (endwin() == ERR)
		error = "cannot restore terminal";
	delscreen(screen);
	editor_free(&ed);
	return error;
}
