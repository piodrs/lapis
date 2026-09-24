#ifndef EDITOR_H
#define EDITOR_H

#include <stddef.h>

typedef struct {
	char *text;
	size_t len;
	size_t cap;
} Row;

typedef struct {
	Row *rows;
	size_t len;
	size_t cap;
	size_t row;
	size_t col;
} Editor;

int editor_init(Editor *ed);
void editor_free(Editor *ed);
int editor_insert(Editor *ed, int ch);
int editor_split(Editor *ed);
int editor_backspace(Editor *ed);
int editor_delete(Editor *ed);

#endif /* EDITOR_H */
