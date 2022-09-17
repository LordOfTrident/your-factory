#include "block.h"

size_t block_type_cost_map[BLOCKS_COUNT] = {
	[BLOCK_GRASS] = 5,
	[BLOCK_STONE] = 10,
	[BLOCK_BRICK] = 15,

	[BLOCK_CONVEYOR] = 25,
	[BLOCK_FINISH]   = 50,
	[BLOCK_DROPPER]  = 75
};

size_t block_type_cost(block_type_t p_type) {
	return block_type_cost_map[(int)p_type];
}

block_t block_new_with_top(block_type_t p_floor, block_type_t p_top) {
	return (block_t){
		.floor   = p_floor,
		.top     = p_top,
		.has_top = true,
		.dir     = DIR_UP,
		.cost    = block_type_cost(p_top)
	};
}

block_type_t block_top_sprite_id(block_t *p_block) {
	if (!p_block->has_top)
		assert(0 && "block_top_sprite_id() called, but block does not have a top");

	switch (p_block->top) {
	case BLOCK_DROPPER:
	case BLOCK_CONVEYOR: return (block_type_t)((int)p_block->top + (int)p_block->dir);

	default: return p_block->top;
	}
}

void block_add_top(block_t *p_block, block_type_t p_top, dir_t p_dir) {
	p_block->top     = p_top;
	p_block->has_top = true;
	p_block->dir     = p_dir;
	p_block->cost    = block_type_cost(p_top);

	memset(&p_block->data, 0, sizeof(p_block->data));
}

void block_remove_top(block_t *p_block) {
	p_block->has_top = false;
}

void block_update_particles(block_t *p_block) {
	if (!p_block->has_particles)
		return;

	bool no_active_particles = false;
	for (size_t i = 0; i < SIZE_OF(p_block->particles); ++ i) {
		if (p_block->particles[i] != NULL) {
			if (p_block->particles[i]->timer == 0)
				p_block->particles[i] = NULL;
			else {
				particle_update(p_block->particles[i]);
				no_active_particles = false;
			}
		}
	}

	if (no_active_particles)
		p_block->has_particles = false;
}

void block_emit_particle(block_t *p_block, particle_t *p_particle, asset_t *p_asset,
                         float p_vel_x, float p_vel_y,
                         int p_x, int p_y, size_t p_lifetime, float p_gravity) {
	p_block->has_particles = true;

	for (size_t i = 0; i < SIZE_OF(p_block->particles); ++ i) {
		if (p_block->particles[i] == NULL) {
			p_block->particles[i] = p_particle;
			particle_emit(p_particle, p_asset, p_vel_x, p_vel_y, p_x, p_y, p_lifetime, p_gravity);

			return;
		}
	}

	assert(0 && "No more block particles space");
}

void block_set_timer(block_t *p_block, size_t p_time) {
	block_set_floor_timer(p_block, p_time);
	block_set_top_timer(p_block, p_time);
}

void block_set_floor_timer(block_t *p_block, size_t p_time) {
	p_block->anim_time = p_time;
	p_block->timer     = p_time;
}

void block_set_top_timer(block_t *p_block, size_t p_time) {
	p_block->top_anim_time = p_time;
	p_block->top_timer     = p_time;
}
