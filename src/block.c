#include "block.h"

size_t block_type_cost_map[BLOCKS_COUNT] = {
	[BLOCK_AIR] = 0,

	[BLOCK_GRASS] = 5,
	[BLOCK_STONE] = 10,
	[BLOCK_BRICK] = 15,

	[BLOCK_CONVEYOR] = 25,
	[BLOCK_FINISH]   = 50,
	[BLOCK_DROPPER]  = 75
};

size_t block_type_cost(block_type_t p_type) {
	return block_type_cost_map[p_type];
}

block_t block_new(block_type_t p_type, bool p_active) {
	block_t block = {
		.type   = p_type,
		.dir    = DIR_UP,
		.active = p_active,
		.cost   = block_type_cost(p_type)
	};

	return block;
}

void block_rotate_right(block_t *p_block) {
	if (p_block->dir == DIR_UP)
		p_block->dir = DIR_RIGHT;
	else
		-- p_block->dir;
}

void block_rotate_left(block_t *p_block) {
	if (p_block->dir == DIR_RIGHT)
		p_block->dir = DIR_UP;
	else
		++ p_block->dir;
}

void block_set_type(block_t *p_block, block_type_t p_type) {
	p_block->type = p_type;
	p_block->cost = block_type_cost(p_type);
}

void block_update(block_t *p_block) {
	if (p_block->timer > 0)
		-- p_block->timer;
	else if (p_block->time != 0)
		p_block->time = 0;
}

void block_set_timer(block_t *p_block, size_t p_time) {
	p_block->time  = p_time;
	p_block->timer = p_time;
}

float block_scale_from_timer(block_t *p_block) {
	return p_block->time == 0? BLOCK_SIZE :
	       (1.0 - (float)p_block->timer / (float)p_block->time) * BLOCK_SIZE;
}

block_sprite_t block_get_sprite(block_t *p_block) {
	switch (p_block->type) {
	case BLOCK_AIR: assert(0 && "block_get_sprite(): got block type BLOCK_AIR"); break;

	case BLOCK_GRASS: return BLOCK_SPRITE_GRASS;
	case BLOCK_STONE: return BLOCK_SPRITE_STONE;
	case BLOCK_BRICK: return BLOCK_SPRITE_BRICK;

	case BLOCK_CONVEYOR: return BLOCK_SPRITE_CONVEYOR + p_block->dir;
	case BLOCK_FINISH:   return BLOCK_SPRITE_FINISH;
	case BLOCK_DROPPER:  return BLOCK_SPRITE_DROPPER + p_block->dir;

	default: assert(0 && "block_get_sprite(): got incorrect block type");
	}
}

tile_t tile_new(block_type_t p_floor, bool p_active) {
	tile_t tile = {
		.floor = block_new(p_floor, p_active),
		.top   = block_new(BLOCK_AIR, false)
	};

	return tile;
}

void tile_add_top(tile_t *p_tile, block_type_t p_top, dir_t p_dir) {
	p_tile->top     = block_new(p_top, true);
	p_tile->top.dir = p_dir;
}

void tile_remove_top(tile_t *p_tile) {
	p_tile->top = block_new(BLOCK_AIR, false);
}

void tile_update(tile_t *p_tile) {
	block_update(&p_tile->floor);
	block_update(&p_tile->top);

	tile_update_particles(p_tile);
}

void tile_set_timer(tile_t *p_tile, size_t p_time) {
	block_set_timer(&p_tile->floor, p_time);
	block_set_timer(&p_tile->top,   p_time);
}

void tile_set_active(tile_t *p_tile, bool p_active) {
	p_tile->floor.active = p_active;
	p_tile->top.active   = p_active;
}

void tile_update_particles(tile_t *p_tile) {
	if (!p_tile->has_particles)
		return;

	bool no_active_particles = true;
	for (size_t i = 0; i < SIZE_OF(p_tile->particles); ++ i) {
		if (p_tile->particles[i] != NULL) {
			if (p_tile->particles[i]->timer == 0)
				p_tile->particles[i] = NULL;
			else {
				particle_update(p_tile->particles[i]);
				no_active_particles = false;
			}
		}
	}

	if (no_active_particles)
		p_tile->has_particles = false;
}

void tile_emit_particle(tile_t *p_tile, particle_t *p_particle, asset_t *p_asset,
                        float p_vel_x, float p_vel_y,
                        int p_x, int p_y, size_t p_lifetime, float p_gravity) {
	p_tile->has_particles = true;

	for (size_t i = 0; i < SIZE_OF(p_tile->particles); ++ i) {
		if (p_tile->particles[i] == NULL) {
			p_tile->particles[i] = p_particle;
			particle_emit(p_particle, p_asset, p_vel_x, p_vel_y, p_x, p_y, p_lifetime, p_gravity);

			return;
		}
	}

	assert(0 && "tile_emit_particle(): no more particle slots in tile");
}
