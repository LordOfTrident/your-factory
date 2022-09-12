#include "block.h"

block_t block_new_with_top(block_type_t p_floor, block_type_t p_top) {
	return (block_t){
		.floor   = p_floor,
		.top     = p_top,
		.has_top = true,
		.dir     = DIR_UP
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

	memset(&p_block->data, 0, sizeof(p_block->data));
}

void block_remove_top(block_t *p_block) {
	p_block->has_top = false;
}

void block_set_timer(block_t *p_block, size_t p_time) {
	p_block->anim_time  = p_time;
	p_block->anim_timer = p_time;
}
