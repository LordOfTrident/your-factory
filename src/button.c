#include "button.h"

button_t button_new(asset_t *p_asset, int p_x, int p_y) {
	return (button_t){
		.texture = p_asset->texture,

		.src = {
			.w = p_asset->rect.w,
			.h = p_asset->rect.h / 2
		},

		.dest = {
			.x = p_x,
			.y = p_y,
			.w = p_asset->rect.w,
			.h = p_asset->rect.h / 2
		}
	};
}

void button_render(button_t *p_button, SDL_Renderer *p_renderer) {
	SDL_RenderCopy(p_renderer, p_button->texture, &p_button->src, &p_button->dest);
}

void button_events(button_t *p_button, SDL_Event *p_event, SDL_Point *p_mouse) {
	if (p_button->state == BUTTON_STATE_UP)
		assert(0 && "Button was not reset");

	switch (p_event->type) {
	case SDL_MOUSEMOTION:
		if (SDL_PointInRect(p_mouse, &p_button->dest)) {
			p_button->hovered = true;
			p_button->src.y   = p_button->src.h;
		} else if (p_button->hovered) {
			p_button->hovered = false;
			p_button->src.y   = 0;
		}

		break;

	case SDL_MOUSEBUTTONDOWN:
		if (p_button->hovered)
			p_button->state = BUTTON_STATE_DOWN;

		break;

	case SDL_MOUSEBUTTONUP:
		if (p_button->hovered && p_button->state == BUTTON_STATE_DOWN)
			p_button->state = BUTTON_STATE_UP;

		break;
	}
}

bool button_pressed(button_t *p_button) {
	if (p_button->state == BUTTON_STATE_UP) {
		p_button->state = BUTTON_STATE_NONE;

		return true;
	} else
		return false;
}
