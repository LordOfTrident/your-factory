#include "image.h"

image_t image_new(asset_t *p_asset, int p_x, int p_y) {
	return (image_t){
		.texture = p_asset->texture,

		.src = {
			.x = p_asset->rect.x,
			.y = p_asset->rect.y,
			.w = p_asset->rect.w,
			.h = p_asset->rect.h
		},

		.dest = {
			.x = p_x,
			.y = p_y,
			.w = p_asset->rect.w,
			.h = p_asset->rect.h
		}
	};
}

void image_set_alpha(image_t *p_image, Uint8 p_a) {
	SDL_SetTextureAlphaMod(p_image->texture, p_a);
}

void image_render(image_t *p_image, SDL_Renderer *p_renderer) {
	SDL_RenderCopy(p_renderer, p_image->texture, &p_image->src, &p_image->dest);
}
