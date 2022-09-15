#ifndef GAME_H__HEADER_GUARD__
#define GAME_H__HEADER_GUARD__

#include <stdlib.h>  /* exit, EXIT_FAILURE, EXIT_SUCCESS, size_t */
#include <stdbool.h> /* bool, true, false */
#include <math.h>    /* sin, round */

#include <stdio.h>   /* snprintf, printf, puts */

#include <SDL2/SDL.h>

#include "text.h"
#include "block.h"
#include "particle.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 3
#define VERSION_PATCH 1

#define TITLE "Your Factory"

#define SCREEN_W 288
#define SCREEN_H 204

#define SCALE 4

#define FPS_CAP    60
#define BLINK_TIME 20

#define BLOCK_SIZE 24
#define MAP_SIZE   11

#define MAP_POS_X SCREEN_W / 2 - BLOCK_SIZE / 2
#define MAP_POS_Y 1.5 * BLOCK_SIZE

#define BLOCK_ANIM_TIME 15
#define BLOCK_ANIM_DELAY_BETWEEN_COLUMN 2

#define GRAVITY_ACCELERATION 0.3

#define SCREEN_SHAKE_TIME 5

#define REFUND_PENALTY 0.7

#define MOUSE_DRAG_FRICTION 3

typedef enum {
	MODE_VIEWING = 0,
	MODE_PLACING,
	MODE_DELETING
} interact_mode_t;

typedef struct {
	SDL_Window   *window;
	SDL_Renderer *renderer;
	SDL_Texture  *screen;
	SDL_Rect      screen_rect;

	SDL_Event    event;
	const Uint8 *keyboard;

	size_t fps, tick;

	text_renderer_t trend;
	texture_t       blocks, gold_icon, dust, arrows, block_outline;

	texture_t right_arrow, left_arrow;

	SDL_Point block_id_pos_map[BLOCKS_COUNT];

	block_t map[MAP_SIZE][MAP_SIZE];

	particle_t particles[512];

	SDL_Point       cursor, camera, mouse;
	block_t         cursor_block;
	interact_mode_t mode;

	size_t gold, shake_timer, map_anim_it;
	SDL_Point screen_shake_offset;

	SDL_Point mouse_drag_begin, prev_camera;
	bool      dragging;

	size_t shop_pos;

	bool quit, loaded;
} game_t;

extern block_type_t blocks_shop[];

void usage(void);
void version(void);

game_t game_new(void);
void   game_quit(game_t *p_game);
void   game_run(game_t *p_game);

void game_render_isometric_tile(game_t *p_game, int p_x, int p_y, int p_size,
                                SDL_Texture *p_texture, SDL_Point p_src, bool p_top_layer);
void game_render_block(game_t *p_game, int p_x, int p_y);
void game_render_cursor(game_t *p_game);
void game_render_map(game_t *p_game);
void game_render_ui(game_t *p_game);

void game_render_particles_at(game_t *p_game, int p_x, int p_y);

SDL_Point game_get_block_sheet_pos(game_t *p_game, block_type_t p_type);

block_t *game_cursor_block(game_t *p_game);
bool     game_cursor_has_block(game_t *p_game);

bool game_mouse_touches(game_t *p_game, SDL_Rect *p_rect);

const char *game_mode_name(game_t *p_game);

void game_emit_particles_at(game_t *p_game, int p_x, int p_y, size_t p_amount);
void game_shake_screen(game_t *p_game);
void game_animate_block(game_t *p_game, SDL_Point p_pos);

void game_place_cursor_block(game_t *p_game);
void game_refund_cursor_block(game_t *p_game);

void game_render(game_t *p_game);
void game_events(game_t *p_game);
void game_update(game_t *p_game);

#endif
