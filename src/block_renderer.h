#ifndef BLOCK_RENDERER_H__HEADER_GUARD__
#define BLOCK_RENDERER_H__HEADER_GUARD__

#include <string.h> /* memset */
#include <math.h>   /* round */

#include <SDL2/SDL.h>

#include "utils.h"
#include "block.h"
#include "particle.h"

typedef enum {
	LAYER_FLOOR = 0,
	LAYER_TOP
} layer_t;

typedef struct {
	SDL_Renderer *renderer;

	struct {
		asset_t *blocks;
	} sheet;

	SDL_Point map_pos;
} block_renderer_t;

void block_renderer_init(block_renderer_t *p_block_renderer,
                         SDL_Renderer *p_renderer, asset_t *p_blocks);

void block_renderer_set_map_pos(block_renderer_t *p_block_renderer, int p_x, int p_y);

void block_renderer_set_color(block_renderer_t *p_block_renderer, Uint8 p_r, Uint8 p_g, Uint8 p_b);
void block_renderer_set_alpha(block_renderer_t *p_block_renderer, Uint8 p_alpha);

void block_renderer_render_tile(block_renderer_t *p_block_renderer,
                                int p_x, int p_y, tile_t *p_tile);
void block_renderer_render(block_renderer_t *p_block_renderer,
                           int p_x, int p_y, block_t *p_block, layer_t p_layer);

void block_renderer_render_sprite(block_renderer_t *p_block_renderer, int p_x, int p_y,
                                  block_sprite_t p_block_sprite, float p_scale, layer_t p_layer);

void block_renderer_render_plain_sprite(block_renderer_t *p_block_renderer, int p_x, int p_y,
                                        block_sprite_t p_block_sprite, float p_scale);

void block_renderer_render_particles(block_renderer_t *p_block_renderer, int p_x, int p_y,
                                     particle_t **p_particles, size_t p_count);

#endif
