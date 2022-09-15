#ifndef BLOCK_H__HEADER_GUARD__
#define BLOCK_H__HEADER_GUARD__

#include <stdbool.h> /* bool, true, false */
#include <assert.h>  /* assert */

#include <SDL2/SDL.h>

#include "particle.h"
#include "utils.h"

typedef enum {
	BLOCK_SEL = 0,
	BLOCK_SEL2,
	BLOCK_SEL3,

	BLOCK_GRASS,
	BLOCK_STONE,
	BLOCK_BRICK,

	BLOCK_CONVEYOR,  /* up */
	BLOCK_CONVEYOR2, /* left */
	BLOCK_CONVEYOR3, /* down */
	BLOCK_CONVEYOR4, /* right */

	BLOCK_FINISH,
	BLOCK_DROPPER,  /* up */
	BLOCK_DROPPER2, /* left */
	BLOCK_DROPPER3, /* down */
	BLOCK_DROPPER4, /* right */

	BLOCKS_COUNT
} block_type_t;

typedef enum {
	DIR_UP = 0,
	DIR_LEFT,
	DIR_DOWN,
	DIR_RIGHT
} dir_t;

typedef struct {
	block_type_t floor, top;
	bool has_top;

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

	dir_t  dir;
	size_t cost;

	particle_t *particles[80];
	bool        has_particles;

	size_t timer, anim_time;
	size_t top_timer, top_anim_time;
	bool   active;
} block_t;

extern size_t block_type_cost_map[BLOCKS_COUNT];

size_t block_type_cost(block_type_t p_type);

block_t block_new_with_top(block_type_t p_floor, block_type_t p_top);

block_type_t block_top_sprite_id(block_t *p_block);

void block_add_top(block_t *p_block, block_type_t p_top, dir_t p_dir);
void block_remove_top(block_t *p_block);

void block_update_particles(block_t *p_block);
void block_emit_particle(block_t *p_block, particle_t *p_particle, texture_t *p_texture,
                         float p_vel_x, float p_vel_y,
                         int p_x, int p_y, size_t p_lifetime, float p_gravity);

void block_set_timer(block_t *p_block, size_t p_time);
void block_set_floor_timer(block_t *p_block, size_t p_time);
void block_set_top_timer(block_t *p_block, size_t p_time);

#endif
