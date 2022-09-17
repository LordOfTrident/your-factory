#ifndef BUTTON_H__HEADER_GUARD__
#define BUTTON_H__HEADER_GUARD__

#include <stdbool.h> /* bool, true, false */

#include "utils.h"

typedef enum {
	BUTTON_STATE_NONE = 0,
	BUTTON_STATE_DOWN,
	BUTTON_STATE_UP
} button_state_t;

typedef struct {
	SDL_Texture *texture;
	SDL_Rect src, dest;

	bool hovered;
	button_state_t state;
} button_t;

button_t button_new(asset_t *p_asset, int p_x, int p_y);

void button_render(button_t *p_button, SDL_Renderer *p_renderer);
void button_events(button_t *p_button, SDL_Event *p_event, SDL_Point *p_mouse);

bool button_pressed(button_t *p_button);

#endif
