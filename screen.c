#include <stdio.h>
#include <curses.h>

#include "screen.h"

const char *screen_run(void)
{
	SCREEN *screen;
	const char *error;
	int ch;

	screen = newterm(NULL, stdout, stdin);
	if (screen == NULL)
		return "cannot initialize terminal";
	error = NULL;
	if (raw() == ERR || noecho() == ERR || erase() == ERR ||
	    refresh() == ERR) {
		error = "cannot configure terminal";
	} else {
		while ((ch = getch()) != 0x11) {
			if (ch == ERR) {
				error = "cannot read input";
				break;
			}
		}
	}
	if (endwin() == ERR)
		error = "cannot restore terminal";
	delscreen(screen);
	return error;
}
