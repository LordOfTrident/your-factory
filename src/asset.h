#ifndef ASSET_H__HEADER_GUARD__
#define ASSET_H__HEADER_GUARD__

#include <stdlib.h>  /* exit, EXIT_FAILURE */
#include <stdbool.h> /* bool, true, false */

#include <SDL2/SDL.h>

typedef struct {
	SDL_Texture *texture;
	SDL_Rect     rect;
} asset_t;

asset_t asset_load(SDL_Renderer *p_renderer, const char *p_path);
void    asset_free(asset_t *p_asset);

#endif
