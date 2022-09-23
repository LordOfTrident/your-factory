#include "block_renderer.h"

void block_renderer_init(block_renderer_t *p_block_renderer,
                         SDL_Renderer *p_renderer, asset_t *p_blocks) {
	memset(p_block_renderer, 0, sizeof(block_renderer_t));

	p_block_renderer->renderer     = p_renderer;
	p_block_renderer->sheet.blocks = p_blocks;
}

void block_renderer_set_map_pos(block_renderer_t *p_block_renderer, int p_x, int p_y) {
	p_block_renderer->map_pos.x = p_x;
	p_block_renderer->map_pos.y = p_y;
}

void block_renderer_set_color(block_renderer_t *p_block_renderer, Uint8 p_r, Uint8 p_g, Uint8 p_b) {
	SDL_SetTextureColorMod(p_block_renderer->sheet.blocks->texture, p_r, p_g, p_b);
}

void block_renderer_set_alpha(block_renderer_t *p_block_renderer, Uint8 p_alpha) {
	SDL_SetTextureAlphaMod(p_block_renderer->sheet.blocks->texture, p_alpha);
}

void block_renderer_render_tile(block_renderer_t *p_block_renderer,
                                int p_x, int p_y, tile_t *p_tile) {
	block_renderer_render(p_block_renderer, p_x, p_y, &p_tile->floor, LAYER_FLOOR);
	block_renderer_render(p_block_renderer, p_x, p_y, &p_tile->top,   LAYER_TOP);

	if (p_tile->has_particles)
		block_renderer_render_particles(p_block_renderer, p_x, p_y,
		                                p_tile->particles, SIZE_OF(p_tile->particles));
}

void block_renderer_render(block_renderer_t *p_block_renderer, int p_x, int p_y,
                           block_t *p_block, layer_t p_layer) {
	if (p_block->type == BLOCK_AIR || !p_block->active)
		return;

	block_renderer_render_sprite(p_block_renderer, p_x, p_y, block_get_sprite(p_block),
	                             block_scale_from_timer(p_block), p_layer);
}

void block_renderer_render_sprite(block_renderer_t *p_block_renderer, int p_x, int p_y,
                                  block_sprite_t p_block_sprite, float p_scale, layer_t p_layer) {
	int x = p_x * (BLOCK_SIZE / 2) + p_y * -(BLOCK_SIZE / 2) + p_block_renderer->map_pos.x;
	int y = p_x * (BLOCK_SIZE / 4) + p_y *  (BLOCK_SIZE / 4) + p_block_renderer->map_pos.y;

	y -= (int)p_layer * (BLOCK_SIZE - 10);

	block_renderer_render_plain_sprite(p_block_renderer, x, y, p_block_sprite, p_scale);
}

void block_renderer_render_plain_sprite(block_renderer_t *p_block_renderer, int p_x, int p_y,
                                        block_sprite_t p_block_sprite, float p_scale) {
	if (p_scale == 0)
		return;

	SDL_Rect dest = {
		.x = p_x,
		.y = p_y,
		.w = round(p_scale),
		.h = round(p_scale)
	};

	if (p_scale != BLOCK_SIZE) {
		float calc = BLOCK_SIZE / 2 - p_scale / 2;

		dest.x += calc;
		dest.y += calc;
	}

	SDL_Rect src = {
		.x = p_block_sprite % (p_block_renderer->sheet.blocks->rect.w / BLOCK_SIZE) * BLOCK_SIZE,
		.y = p_block_sprite / (p_block_renderer->sheet.blocks->rect.w / BLOCK_SIZE) * BLOCK_SIZE,
		.w = BLOCK_SIZE,
		.h = BLOCK_SIZE
	};

	SDL_RenderCopy(p_block_renderer->renderer, p_block_renderer->sheet.blocks->texture,
	               &src, &dest);
}

void block_renderer_render_particles(block_renderer_t *p_block_renderer, int p_x, int p_y,
                                     particle_t **p_particles, size_t p_count) {
	int x = p_x * (BLOCK_SIZE / 2) + p_y * -(BLOCK_SIZE / 2) + p_block_renderer->map_pos.x;
	int y = p_x * (BLOCK_SIZE / 4) + p_y *  (BLOCK_SIZE / 4) + p_block_renderer->map_pos.y;

	for (size_t i = 0; i < p_count; ++ i) {
		if (p_particles[i] == NULL)
			continue;

		SDL_Rect dest = {
			.w = p_particles[i]->asset.rect.w,
			.h = p_particles[i]->asset.rect.h,
			.x = x + p_particles[i]->x,
			.y = y + p_particles[i]->y
		};

		float alpha = (float)p_particles[i]->timer / p_particles[i]->lifetime * 255;
		SDL_SetTextureAlphaMod(p_particles[i]->asset.texture, alpha);

		SDL_RenderCopy(p_block_renderer->renderer, p_particles[i]->asset.texture, NULL, &dest);
	}
}
