#include "utils.h"

void *memalloc(size_t p_size) {
	void *ptr = malloc(p_size);
	if (ptr == NULL)
		assert(0 && "malloc fail");

	return ptr;
}

void *memrealloc(void *p_ptr, size_t p_size) {
	void *ptr = realloc(p_ptr, p_size);
	if (ptr == NULL) {
		free(p_ptr);

		assert(0 && "realloc fail");
	} else
		return ptr;
}

void memfree(void **p_ptr) {
	free(*p_ptr);
	*p_ptr = NULL;
}

void *memalloccopy(const void *p_ptr, size_t p_size) {
	void *copy = memalloc(p_size);
	memcpy(copy, p_ptr, p_size);

	return copy;
}

char *copy_str(const char *p_str) {
	return (char*)memalloccopy(p_str, strlen(p_str) + 1);
}

texture_t texture_load(SDL_Renderer *p_renderer, const char *p_path) {
	texture_t texture = {0};

	SDL_Surface *surface = SDL_LoadBMP(p_path);
	if (surface == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	} else
		SDL_Log("Loaded asset '%s'", p_path);

	texture.dest.w = surface->w;
	texture.dest.h = surface->h;

	texture.src = texture.dest;

	Uint32 color_key = SDL_MapRGB(surface->format, 255, 0, 255);
	SDL_SetColorKey(surface, true, color_key);

	texture.texture = SDL_CreateTextureFromSurface(p_renderer, surface);
	if (texture.texture == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	SDL_FreeSurface(surface);

	return texture;
}

void texture_free(texture_t *p_texture) {
	if (p_texture->texture != NULL)
		SDL_DestroyTexture(p_texture->texture);
}

void texture_render(texture_t *p_texture, SDL_Renderer *p_renderer) {
	SDL_RenderCopy(p_renderer, p_texture->texture, &p_texture->src, &p_texture->dest);
}

SDL_Color SDL_GetSurfacePixel(SDL_Surface *p_surface, int p_x, int p_y) {
	Uint32 *pixels = (Uint32*)p_surface->pixels;

	SDL_LockSurface(p_surface);

	SDL_Color color;

	SDL_GetRGBA(pixels[p_surface->w * p_y + p_x], p_surface->format,
	            &color.r, &color.g, &color.b, &color.a);

	SDL_UnlockSurface(p_surface);

	return color;
}
