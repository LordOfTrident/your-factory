#include <string.h> /* strcmp */
#include <stdio.h>  /* fprintf */

#include "game.h"

int main(int p_argc, char **p_argv) {
	for (int i = 1; i < p_argc; ++ i) {
		if (strcmp(p_argv[i], "-v") == 0 || strcmp(p_argv[i], "--version") == 0)
			version();
		else if (strcmp(p_argv[i], "-h") == 0 || strcmp(p_argv[i], "--help") == 0)
			usage();
		else {
			fprintf(stderr, "Error: Unknown option '%s'\nTry 'your-factory -h'\n", p_argv[i]);

			exit(EXIT_FAILURE);
		}
	}

	game_t game = game_new();
	game_run(&game);

	game_quit(&game);

	return EXIT_SUCCESS;
}
