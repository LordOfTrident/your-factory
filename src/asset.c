#include "asset.h"

asset_t asset_load(SDL_Renderer *p_renderer, const char *p_path) {
	asset_t asset = {0};

	SDL_Surface *surface = SDL_LoadBMP(p_path);
	if (surface == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	} else
		SDL_Log("Loaded asset '%s'", p_path);

	asset.rect.w = surface->w;
	asset.rect.h = surface->h;

	Uint32 color_key = SDL_MapRGB(surface->format, 255, 0, 255);
	SDL_SetColorKey(surface, true, color_key);

	asset.texture = SDL_CreateTextureFromSurface(p_renderer, surface);
	if (asset.texture == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	SDL_FreeSurface(surface);

	return asset;
}

void asset_free(asset_t *p_asset) {
	SDL_DestroyTexture(p_asset->texture);

	p_asset->texture = NULL;
}
