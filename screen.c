#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "screen.h"

#define KEY_CTRL(key) ((key)&0x1f)
#define KEY_DEL 0x7f

const char *screen_run(void)
{
	SCREEN *screen;
	const char *error;
	char *line;
	char *next;
	int ch;
	int col;
	int cols;
	int len;
	int cap;
	int size;

	screen = newterm(NULL, stdout, stdin);
	if (screen == NULL)
		return "cannot initialize terminal";
	error = NULL;
	line = NULL;
	len = 0;
	cap = 0;
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
			cols = getmaxx(stdscr);
			switch (ch) {
			case KEY_LEFT:
				if (col > 0)
					--col;
				break;
			case KEY_RIGHT:
				if (col < len && col < cols - 1)
					++col;
				break;
			case KEY_BACKSPACE:
			case KEY_DEL:
			case KEY_CTRL('h'):
				if (col > 0) {
					memmove(line + col - 1, line + col,
						len - col + 1);
					--len;
					--col;
				}
				break;
			case KEY_DC:
				if (col < len) {
					memmove(line + col, line + col + 1,
						len - col);
					--len;
				}
				break;
			default:
				if (ch < ' ' || ch > '~' || len >= cols - 1)
					break;
				if (len + 1 >= cap) {
					size = cap == 0 ? cols
							: (cap > cols / 2
								   ? cols
								   : cap * 2);
					next = realloc(line, size);
					if (next == NULL) {
						error = "cannot allocate line";
						break;
					}
					line = next;
					cap = size;
				}
				memmove(line + col + 1, line + col, len - col);
				line[col++] = ch;
				line[++len] = '\0';
				break;
			}
			if (error != NULL)
				break;
			if (col >= cols)
				col = cols - 1;
			if (erase() == ERR ||
			    (len > 0 && addnstr(line, cols - 1) == ERR) ||
			    move(0, col) == ERR || refresh() == ERR) {
				error = "cannot draw line";
				break;
			}
		}
	}
	if (endwin() == ERR)
		error = "cannot restore terminal";
	delscreen(screen);
	free(line);
	return error;
}
