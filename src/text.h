#ifndef TEXT_H__HEADER_GUARD__
#define TEXT_H__HEADER_GUARD__

#include <stdlib.h>  /* size_t */
#include <string.h>  /* strcmp */
#include <stdbool.h> /* bool, true, false */

#include <SDL2/SDL.h>

#include "utils.h"

typedef struct {
	SDL_Surface *sheet;

	int w, h;
	int ch_w, ch_h;
} font_t;

font_t font_load(const char *p_path);
void   font_free(font_t *p_font);

SDL_Point font_get_ch_pos(font_t *p_font, char p_ch);

typedef struct {
	font_t        font;
	SDL_Renderer *renderer;

	struct {
		const char *key;
		texture_t   value;
	} cache[128];
	size_t cache_size;
} text_renderer_t;

text_renderer_t text_renderer_new(SDL_Renderer *p_renderer, font_t *p_font);
void            text_renderer_destroy(text_renderer_t *p_trend);

texture_t text_renderer_render(text_renderer_t *p_trend, const char *p_text);

#endif
