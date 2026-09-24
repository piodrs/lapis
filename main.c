#include <stdio.h>

#include "screen.h"

int main(void)
{
	const char *error;

	error = screen_run();
	if (error != NULL) {
		fprintf(stderr, "lapis: %s\n", error);
		return 1;
	}
	return 0;
}
