#include "text.h"

font_t font_load(const char *p_path) {
	font_t font = {0};

	font.sheet = SDL_LoadBMP(p_path);
	if (font.sheet == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	} else
		SDL_Log("Loaded font '%s'", p_path);

	Uint32 color_key = SDL_MapRGB(font.sheet->format, 255, 0, 255);
	SDL_SetColorKey(font.sheet, true, color_key);

	SDL_Color ch_info = SDL_GetSurfacePixel(font.sheet, font.sheet->w - 1, font.sheet->h - 1);

	font.w = font.sheet->w - 1;
	font.h = font.sheet->h - 1;

	font.ch_w = ch_info.r;
	font.ch_h = ch_info.g;

	return font;
}

void font_free(font_t *p_font) {
	SDL_FreeSurface(p_font->sheet);
}

SDL_Point font_get_ch_pos(font_t *p_font, char p_ch) {
	int row_len = p_font->w / p_font->ch_w;

	SDL_Point pos = {
		.x = p_ch % row_len * p_font->ch_w,
		.y = p_ch / row_len * p_font->ch_h
	};

	return pos;
}

text_renderer_t text_renderer_new(SDL_Renderer *p_renderer, font_t *p_font) {
	text_renderer_t text_renderer = {
		.font     = *p_font,
		.renderer = p_renderer
	};

	return text_renderer;
}

void text_renderer_destroy(text_renderer_t *p_trend) {
	for (size_t i = 0; i < p_trend->cache_size; ++ i) {
		if (p_trend->cache[i].value.texture != NULL) {
			asset_free(&p_trend->cache[i].value);
			SMEMFREE(p_trend->cache[i].key);
		}
	}
}

asset_t text_renderer_render(text_renderer_t *p_trend, const char *p_text) {
	for (size_t i = 0; i < p_trend->cache_size; ++ i) {
		if (strcmp(p_trend->cache[i].key, p_text) == 0)
			return p_trend->cache[i].value;
	}

	if (p_trend->cache_size >= SIZE_OF(p_trend->cache)) {
		text_renderer_destroy(p_trend);

		p_trend->cache_size = 0;
	}

	size_t idx = p_trend->cache_size ++;

	size_t      text_len = strlen(p_text);
	const char *text     = copy_str(p_text);

	SDL_Surface *surface = SDL_CreateRGBSurface(0, text_len * p_trend->font.ch_w,
	                                            p_trend->font.ch_h, 32, 0, 0, 0, 0);
	if (surface == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	Uint32 color_key = SDL_MapRGB(surface->format, 0, 0, 0);
	SDL_SetColorKey(surface, true, color_key);

	SDL_Rect src = {
		.w = p_trend->font.ch_w,
		.h = p_trend->font.ch_h
	};

	SDL_Rect dest = src;

	for (size_t i = 0; i < text_len; ++ i) {
		dest.x = i * dest.w;

		SDL_Point pos = font_get_ch_pos(&p_trend->font, p_text[i]);
		src.x = pos.x;
		src.y = pos.y;

		SDL_BlitSurface(p_trend->font.sheet, &src, surface, &dest);
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(p_trend->renderer, surface);
	if (texture == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	SDL_FreeSurface(surface);

	asset_t text_texture = {
		.texture = texture,
		.rect    = {
			.w = src.w * text_len,
			.h = src.h
		}
	};

	p_trend->cache[idx].key   = text;
	p_trend->cache[idx].value = text_texture;

	return p_trend->cache[idx].value;
}
