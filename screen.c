#include <stdio.h>
#include <curses.h>

#include "screen.h"

#define KEY_CTRL(key) ((key)&0x1f)

const char *screen_run(void)
{
	SCREEN *screen;
	const char *error;
	int ch;
	int row;
	int col;
	int rows;
	int cols;

	screen = newterm(NULL, stdout, stdin);
	if (screen == NULL)
		return "cannot initialize terminal";
	error = NULL;
	row = 0;
	col = 0;
	if (raw() == ERR || noecho() == ERR || keypad(stdscr, TRUE) == ERR ||
	    erase() == ERR || refresh() == ERR) {
		error = "cannot configure terminal";
	} else {
		while ((ch = getch()) != KEY_CTRL('q')) {
			if (ch == ERR) {
				error = "cannot read input";
				break;
			}
			getmaxyx(stdscr, rows, cols);
			switch (ch) {
			case KEY_UP:
				if (row > 0)
					--row;
				break;
			case KEY_DOWN:
				if (row < rows - 1)
					++row;
				break;
			case KEY_LEFT:
				if (col > 0)
					--col;
				break;
			case KEY_RIGHT:
				if (col < cols - 1)
					++col;
				break;
			}
			if (row >= rows)
				row = rows - 1;
			if (col >= cols)
				col = cols - 1;
			if (move(row, col) == ERR || refresh() == ERR) {
				error = "cannot position cursor";
				break;
			}
		}
	}
	if (endwin() == ERR)
		error = "cannot restore terminal";
	delscreen(screen);
	return error;
}
