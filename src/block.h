#ifndef BLOCK_H__HEADER_GUARD__
#define BLOCK_H__HEADER_GUARD__

#include <stdbool.h> /* bool, true, false */
#include <assert.h>  /* assert */

#include <SDL2/SDL.h>

#define BLOCK_ANIM_TIME 15
#define BLOCK_ANIM_DELAY_BETWEEN_COLUMN 2

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

	dir_t dir;

	size_t anim_timer, anim_time;
	bool   active;
} block_t;

block_t block_new_with_top(block_type_t p_floor, block_type_t p_top);

block_type_t block_top_sprite_id(block_t *p_block);

void block_add_top(block_t *p_block, block_type_t p_top, dir_t p_dir);
void block_remove_top(block_t *p_block);

void block_set_timer(block_t *p_block, size_t p_time);

#endif
