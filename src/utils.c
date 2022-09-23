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

void flag_set(int *p_value, int p_flag, bool p_set) {
	if (p_set)
		*p_value |= p_flag;
	else
		*p_value &= ~p_flag;
}

bool flag_get(int p_value, int p_flag) {
	return p_value & p_flag;
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
