#ifndef IMAGE_H__HEADER_GUARD__
#define IMAGE_H__HEADER_GUARD__

#include "utils.h"
#include "asset.h"

typedef struct {
	SDL_Texture *texture;
	SDL_Rect src, dest;
} image_t;

image_t image_new(asset_t *p_asset, int p_x, int p_y);

void image_render(image_t *p_image, SDL_Renderer *p_renderer);

#endif
