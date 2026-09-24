#include <stdlib.h>
#include <string.h>

#include "editor.h"

int editor_row_reserve(Row *row, size_t size)
{
	char *text;
	size_t cap;

	if (size <= row->cap)
		return 0;
	cap = row->cap ? row->cap : 1;
	while (cap < size) {
		if (cap > (size_t)-1 / 2) {
			cap = size;
			break;
		}
		cap *= 2;
	}
	text = realloc(row->text, cap);
	if (text == NULL)
		return 1;
	row->text = text;
	row->cap = cap;
	row->text[row->len] = '\0';
	return 0;
}

void editor_free(Editor *ed)
{
	size_t i;

	for (i = 0; i < ed->len; ++i)
		free(ed->rows[i].text);
	free(ed->rows);
}

int editor_init(Editor *ed)
{
	Row row = {NULL, 0, 0};

	ed->row = 0;
	ed->col = 0;
	ed->rows = malloc(sizeof *ed->rows);
	if (ed->rows == NULL)
		return 1;
	if (editor_row_reserve(&row, 1)) {
		free(ed->rows);
		return 1;
	}
	ed->rows[0] = row;
	ed->len = 1;
	ed->cap = 1;
	return 0;
}

int editor_insert(Editor *ed, int ch)
{
	Row *row;

	row = &ed->rows[ed->row];
	if (row->len > (size_t)-1 - 2 || editor_row_reserve(row, row->len + 2))
		return 1;
	memmove(row->text + ed->col + 1, row->text + ed->col,
		row->len - ed->col + 1);
	row->text[ed->col++] = ch;
	++row->len;
	return 0;
}

int editor_split(Editor *ed)
{
	Row row = {NULL, 0, 0};
	Row *rows;
	size_t cap;
	size_t len;

	if (ed->len == ed->cap) {
		if (ed->cap >= (size_t)-1 / sizeof *rows)
			return 1;
		cap = ed->cap > (size_t)-1 / sizeof *rows / 2
			      ? (size_t)-1 / sizeof *rows
			      : ed->cap * 2;
		rows = realloc(ed->rows, cap * sizeof *rows);
		if (rows == NULL)
			return 1;
		ed->rows = rows;
		ed->cap = cap;
	}
	len = ed->rows[ed->row].len - ed->col;
	if (editor_row_reserve(&row, len + 1))
		return 1;
	memcpy(row.text, ed->rows[ed->row].text + ed->col, len + 1);
	row.len = len;
	memmove(ed->rows + ed->row + 2, ed->rows + ed->row + 1,
		(ed->len - ed->row - 1) * sizeof *ed->rows);
	ed->rows[ed->row].len = ed->col;
	ed->rows[ed->row].text[ed->col] = '\0';
	ed->rows[++ed->row] = row;
	++ed->len;
	ed->col = 0;
	return 0;
}

int editor_join(Editor *ed, size_t pos)
{
	Row *row;
	Row *next;

	row = &ed->rows[pos];
	next = row + 1;
	if (next->len > (size_t)-1 - row->len - 1 ||
	    editor_row_reserve(row, row->len + next->len + 1))
		return 1;
	memcpy(row->text + row->len, next->text, next->len + 1);
	row->len += next->len;
	free(next->text);
	memmove(next, next + 1, (ed->len - pos - 2) * sizeof *next);
	--ed->len;
	return 0;
}

int editor_backspace(Editor *ed)
{
	Row *row;
	size_t col;

	row = &ed->rows[ed->row];
	if (ed->col > 0) {
		memmove(row->text + ed->col - 1, row->text + ed->col,
			row->len - ed->col + 1);
		--row->len;
		--ed->col;
	} else if (ed->row > 0) {
		col = ed->rows[ed->row - 1].len;
		if (editor_join(ed, ed->row - 1))
			return 1;
		--ed->row;
		ed->col = col;
	}
	return 0;
}

int editor_delete(Editor *ed)
{
	Row *row;

	row = &ed->rows[ed->row];
	if (ed->col < row->len) {
		memmove(row->text + ed->col, row->text + ed->col + 1,
			row->len - ed->col);
		--row->len;
	} else if (ed->row + 1 < ed->len) {
		return editor_join(ed, ed->row);
	}
	return 0;
}
