#ifndef WORLD_H__HEADER_GUARD__
#define WORLD_H__HEADER_GUARD__

#include <string.h> /* memset */

#include "block_renderer.h"

#define MAP_SIZE 11

#define BLOCK_ANIM_TIME 15
#define BLOCK_ANIM_DELAY_BETWEEN_COLUMN 2

#define SCREEN_SHAKE_TIME 5

#define CAM_SPEED 2

#define FLAG_UPDATE_CAM (1 << 0)
#define FLAG_LOAD_MAP   (1 << 1)
#define FLAG_MAP_LOADED (1 << 2)

typedef struct {
	block_renderer_t *block_renderer;

	tile_t     map[MAP_SIZE][MAP_SIZE];
	particle_t particles[512];

	SDL_Point      cursor, cam;
	block_sprite_t cursor_sprite;

	size_t    shake_timer, map_anim_it;
	SDL_Point screen_shake_offset;

	int flags;
} world_t;

void world_init(world_t *p_world, block_renderer_t *p_block_renderer, int p_cam_x, int p_cam_y);

void world_load_tile(world_t *p_world, int p_x, int p_y);
void world_load_next(world_t *p_world);

void world_shake(world_t *p_world);

tile_t *world_cursor_tile(world_t *p_world);
bool    world_block_at_cursor(world_t *p_world);

void world_emit_particles_at(world_t *p_world, asset_t *p_asset, int p_x, int p_y, size_t p_amount);

void world_render_map(world_t *p_world);

void world_render_cursor_inactive(world_t *p_world);
void world_render_cursor_select(world_t *p_world, Uint8 p_r, Uint8 p_g, Uint8 p_b, Uint8 p_a);
void world_render_cursor_active(world_t *p_world, block_sprite_t p_cursor_block,
                                Uint8 p_r, Uint8 p_g, Uint8 p_b, Uint8 p_a);

void world_events(world_t *p_world, SDL_Event *p_event);
void world_keyboard(world_t *p_world, const Uint8 *p_keyboard);

void world_update(world_t *p_world, size_t p_tick);

#endif
