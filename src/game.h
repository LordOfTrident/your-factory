#ifndef GAME_H__HEADER_GUARD__
#define GAME_H__HEADER_GUARD__

#include <stdlib.h>  /* exit, EXIT_FAILURE, EXIT_SUCCESS, size_t */
#include <stdbool.h> /* bool, true, false */
#include <string.h>  /* strcmp, memset */
#include <math.h>    /* sin, round */
#include <stdio.h>   /* snprintf, printf, puts */
#include <time.h>    /* time, time_t */

#include <SDL2/SDL.h>

#include "text.h"
#include "image.h"
#include "button.h"
#include "world.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 7
#define VERSION_PATCH 5

#define TITLE "Your Factory"

#define SCREEN_W 288
#define SCREEN_H 204

#define CENTERX(p_width)  CENTER(p_width,  SCREEN_W)
#define CENTERY(p_height) CENTER(p_height, SCREEN_H)

#define SCALE 4

#define CAM_POS_X SCREEN_W / 2 - BLOCK_SIZE / 2
#define CAM_POS_Y 1.5 * BLOCK_SIZE

#define FPS_CAP    60
#define BLINK_TIME 20

#define REFUND_PENALTY 0.7

#define DEFAULT_GOLD 175

#define SHOP_REPOS_TIME 10
#define PREV_NEXT_BLOCKS_OFFSET (BLOCK_SIZE + 14)

enum {
	ASSET_BLOCKS = 0,
	ASSET_GOLD,
	ASSET_PARTICLES,
	ASSET_ARROWS,
	ASSET_OUTLINE,
	ASSET_MENU,
	ASSET_ICONS,
	ASSETS_COUNT
};

typedef enum {
	MODE_VIEWING = 0,
	MODE_DELETING,
	MODE_PLACING,
	MODE_PATHING
} interact_mode_t;

typedef struct {
	SDL_Window   *window;
	SDL_Renderer *renderer;
	SDL_Texture  *screen;
	SDL_Rect      screen_rect;

	SDL_Event    event;
	const Uint8 *keyboard;

	size_t fps, tick, gold;

	block_renderer_t block_renderer;
	world_t world;

	block_t         cursor_block;
	interact_mode_t mode;

	text_renderer_t text_renderer;
	font_t font;

	asset_t assets[ASSETS_COUNT];

	struct {
		image_t outline, gold, icons[3];
	} img;

	struct {
		button_t left, right, menu;
	} button;

	SDL_Point mouse;

	size_t shop_pos, shop_repos_timer;
	dir_t  shop_repos_dir;

	block_type_t *shop;
	size_t        shop_size;

	bool quit, paused;
} game_t;

extern block_type_t blocks_shop[];
extern block_type_t ground_blocks_shop[];

void usage(void);
void version(void);

void game_init(game_t *p_game);
void game_finish(game_t *p_game);
void game_run(game_t *p_game);

void game_load_asset(game_t *p_game, const char *p_path, int p_key);
void game_free_assets(game_t *p_game);

void game_place_cursor_block(game_t *p_game);
void game_place_path_cursor_block(game_t *p_game);
void game_refund_cursor_block(game_t *p_game);

void         game_set_mode(game_t *p_game, interact_mode_t p_mode);
block_type_t game_shop_element(game_t *p_game);

void game_render(game_t *p_game);

void game_render_world(game_t *p_game);
void game_render_ui(game_t *p_game);
void game_render_shop_ui(game_t *p_game);
void game_render_gold_ui(game_t *p_game);
void game_render_mode_ui(game_t *p_game);
void game_render_paused_ui(game_t *p_game);

void game_render_text(game_t *p_game, int p_x, int p_y, const char *p_text);
void game_render_text_center(game_t *p_game, const char *p_text);
void game_render_text_center_x(game_t *p_game, int p_y, const char *p_text);

void game_input(game_t *p_game);

void game_events(game_t *p_game);
void game_keyboard(game_t *p_game);

void game_events_ingame(game_t *p_game);
void game_keyboard_ingame(game_t *p_game);
void game_events_paused(game_t *p_game);

void game_update(game_t *p_game);

#endif
