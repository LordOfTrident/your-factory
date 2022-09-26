#ifndef BLOCK_H__HEADER_GUARD__
#define BLOCK_H__HEADER_GUARD__

#include <stdbool.h> /* bool, true, false */
#include <assert.h>  /* assert */

#include <SDL2/SDL.h>

#include "utils.h"
#include "particle.h"
#include "asset.h"

#define BLOCK_SIZE 24

typedef enum {
	BLOCK_SPRITE_CURSOR = 0,
	BLOCK_SPRITE_CURSOR2,
	BLOCK_SPRITE_CURSOR3,

	BLOCK_SPRITE_GRASS,
	BLOCK_SPRITE_STONE,
	BLOCK_SPRITE_BRICK,

	BLOCK_SPRITE_CONVEYOR,
	BLOCK_SPRITE_CONVEYOR_LEFT,
	BLOCK_SPRITE_CONVEYOR_DOWN,
	BLOCK_SPRITE_CONVEYOR_RIGHT,

	BLOCK_SPRITE_FINISH,
	BLOCK_SPRITE_DROPPER,
	BLOCK_SPRITE_DROPPER_LEFT,
	BLOCK_SPRITE_DROPPER_DOWN,
	BLOCK_SPRITE_DROPPER_RIGHT,

	BLOCK_SPRITE_BOULDER,
	BLOCK_SPRITE_TREE,
	BLOCK_SPRITE_HIGH_GRASS,

	BLOCK_SPRITES_COUNT
} block_sprite_t;

typedef enum {
	BLOCK_AIR = 0,

	BLOCK_GRASS,
	BLOCK_STONE,
	BLOCK_BRICK,

	BLOCK_CONVEYOR,
	BLOCK_FINISH,
	BLOCK_DROPPER,

	BLOCK_BOULDER,
	BLOCK_TREE,
	BLOCK_HIGH_GRASS,

	BLOCKS_COUNT
} block_type_t;

typedef enum {
	DIR_UP = 0,
	DIR_LEFT,
	DIR_DOWN,
	DIR_RIGHT
} dir_t;

typedef struct {
	block_type_t type;

	dir_t  dir;
	size_t cost;

	size_t timer, time;
	bool   active;
} block_t;

extern size_t block_type_cost_map[BLOCKS_COUNT];

size_t block_type_cost(block_type_t p_type);

block_t block_new(block_type_t p_type, bool p_active);

void block_rotate_right(block_t *p_block);
void block_rotate_left(block_t *p_block);

void block_set_type(block_t *p_block, block_type_t p_type);

void block_update(block_t *p_block);

void  block_set_timer(block_t *p_block, size_t p_time);
float block_scale_from_timer(block_t *p_block);

block_sprite_t block_get_sprite(block_t *p_block);

typedef struct {
	block_t top, floor;

	union {
		struct {
			struct {
				int value, pos; /* pos = % of where the metal is located on the conveyor */
			} metal[256];
			size_t metal_count;
		} conveyor;

		struct {
			size_t delay_timer, delay_time;
		} dropper;
	} data;

	particle_t *particles[80];
	bool        has_particles;
} tile_t;

tile_t tile_new(block_type_t p_floor, bool p_active);

void tile_add_top(tile_t *p_tile, block_type_t p_top, dir_t p_dir);
void tile_set_floor(tile_t *p_tile, block_type_t p_floor);
void tile_remove_top(tile_t *p_tile);

void tile_update(tile_t *p_tile);

void tile_set_timer(tile_t *p_tile, size_t p_time);
void tile_set_active(tile_t *p_tile, bool p_active);

void tile_update_particles(tile_t *p_tile);
void tile_emit_particle(tile_t *p_tile, particle_t *p_particle, asset_t *p_asset,
                        float p_vel_x, float p_vel_y,
                        int p_x, int p_y, size_t p_lifetime, float p_gravity);

#endif
