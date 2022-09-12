#include "game.h"

void usage(void) {
	puts("Usage: your-factory [OPTIONS]\n"
	     "https://github.com/LordOfTrident/your-factory\n"
	     "\n"
	     "Options:\n"
	     "  -h, --help     Show this message\n"
	     "  -v, --version  Print the game version");

	exit(EXIT_SUCCESS);
}

void version(void) {
	printf("your-factory %i.%i.%i\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

	exit(EXIT_SUCCESS);
}

game_t game_new(void) {
	game_t game = {0};

	for (int i = 0; i < MAP_SIZE; ++ i) {
		for (int j = 0; j < MAP_SIZE; ++ j)
			game.map[i][j].floor = BLOCK_GRASS;
	}

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	/* create window and screen */
	game.window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                               SCREEN_W * SCALE, SCREEN_H * SCALE, SDL_WINDOW_SHOWN);
	if (game.window == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED);
	if (game.renderer == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	game.screen = SDL_CreateTexture(game.renderer, SDL_PIXELFORMAT_RGBA8888,
	                                SDL_TEXTUREACCESS_TARGET, SCREEN_W, SCREEN_H);
	if (game.screen == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	game.screen_rect.w = SCREEN_W;
	game.screen_rect.h = SCREEN_H;

	/* set config */
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	if (SDL_RenderSetLogicalSize(game.renderer, SCREEN_W, SCREEN_H) != 0) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	if (SDL_SetRenderDrawBlendMode(game.renderer, SDL_BLENDMODE_BLEND) != 0) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	if (MAP_SIZE % 2 == 0)
		assert(0 && "Map center is not a single block");

	game.keyboard = SDL_GetKeyboardState(NULL);

	font_t font = font_load("./res/font.bmp");
	game.trend  = text_renderer_new(game.renderer, &font);

	game.blocks    = texture_load(game.renderer, "./res/blocks.bmp");
	game.gold_icon = texture_load(game.renderer, "./res/icons.bmp");

	for (int i = 0; i < BLOCKS_COUNT; ++ i) {
		game.block_id_pos_map[i].x = i % (game.blocks.rect.w / BLOCK_SIZE) * BLOCK_SIZE;
		game.block_id_pos_map[i].y = i / (game.blocks.rect.w / BLOCK_SIZE) * BLOCK_SIZE;
	}

	game.cursor_block = block_new_with_top(BLOCK_GRASS, BLOCK_CONVEYOR);
	game.cursor.x = MAP_SIZE / 2;
	game.cursor.y = MAP_SIZE / 2;

	return game;
}

void game_quit(game_t *p_game) {
	font_free(&p_game->trend.font);
	text_renderer_destroy(&p_game->trend);

	texture_free(&p_game->blocks);

	SDL_DestroyTexture(p_game->screen);
	SDL_DestroyRenderer(p_game->renderer);
	SDL_DestroyWindow(p_game->window);

	SDL_Quit();
}

void game_run(game_t *p_game) {
	size_t fps_timer = 0;

	do {
		size_t now   = SDL_GetTicks();
		size_t delta = now - fps_timer;

		p_game->fps = 1000 / delta;
		fps_timer   = now;

		char title[128] = {0};
		snprintf(title, sizeof(title), TITLE" FPS: %zu", p_game->fps);

		SDL_SetWindowTitle(p_game->window, title);

		game_render(p_game);
		game_events(p_game);
		game_update(p_game);

		SDL_Delay(1000 / FPS_CAP);
	} while (!p_game->quit);
}

void game_render_isometric_tile(game_t *p_game, SDL_Point p_pos, int p_size, SDL_Texture *p_texture,
                                SDL_Point p_src, bool p_top_layer) {
	SDL_Rect dest = {
		.x = p_pos.x * (BLOCK_SIZE / 2) + p_pos.y * -(BLOCK_SIZE / 2),
		.y = p_pos.x * (BLOCK_SIZE / 4) + p_pos.y *  (BLOCK_SIZE / 4),
		.w = p_size,
		.h = p_size
	};

	/* offset into the middle of the screen */
	dest.x += MAP_POS_X + p_game->screen_shake_offset.x;
	dest.y += MAP_POS_Y + p_game->screen_shake_offset.y;

	if (p_size != BLOCK_SIZE) {
		dest.x += BLOCK_SIZE / 2 - p_size / 2;
		dest.y += BLOCK_SIZE / 2 - p_size / 2;
	}

	if (p_top_layer)
		dest.y -= BLOCK_SIZE - 10;

	SDL_Rect src = {
		.x = p_src.x,
		.y = p_src.y,
		.w = BLOCK_SIZE,
		.h = BLOCK_SIZE
	};

	SDL_RenderCopy(p_game->renderer, p_texture, &src, &dest);
}

void game_render_block(game_t *p_game, block_t *p_block, int p_x, int p_y) {
	if (!p_block->active)
		return;

	SDL_Point sheet_pos = game_get_block_sheet_pos(p_game, p_block->floor);

	int size = BLOCK_SIZE;
	if (p_block->anim_timer > 0) {
		size = (1 - (float)p_block->anim_timer / (float)p_block->anim_time) * BLOCK_SIZE;

		-- p_block->anim_timer;
	}

	game_render_isometric_tile(p_game, (SDL_Point){.x = p_x, .y = p_y}, size,
	                           p_game->blocks.texture, sheet_pos, false);

	if (p_block->has_top) {
		sheet_pos = game_get_block_sheet_pos(p_game, block_top_sprite_id(p_block));

		game_render_isometric_tile(p_game, (SDL_Point){.x = p_x, .y = p_y}, size,
		                           p_game->blocks.texture, sheet_pos, true);
	}
}

void game_render_cursor(game_t *p_game) {
	if (p_game->mode == MODE_PLACING || p_game->mode == MODE_DELETING) {
		block_t *block = p_game->mode == MODE_DELETING?
		                 game_cursor_block(p_game) : &p_game->cursor_block;

		if (p_game->mode == MODE_PLACING || block->has_top) {
			SDL_Point sheet_pos = game_get_block_sheet_pos(p_game, block_top_sprite_id(block));

			SDL_SetTextureAlphaMod(p_game->blocks.texture,
			                       (sin((float)p_game->tick / 10) + 1) * 100);

			if (p_game->mode == MODE_DELETING || game_cursor_has_block(p_game))
				SDL_SetTextureColorMod(p_game->blocks.texture, 255, 20, 20);
			else
				SDL_SetTextureColorMod(p_game->blocks.texture, 0, 255, 0);

			game_render_isometric_tile(p_game, p_game->cursor, BLOCK_SIZE, p_game->blocks.texture,
			                           sheet_pos, true);

			SDL_SetTextureColorMod(p_game->blocks.texture, 255, 255, 255);
			SDL_SetTextureAlphaMod(p_game->blocks.texture, SDL_ALPHA_OPAQUE);
		}
	}

	if (p_game->mode == MODE_VIEWING)
		SDL_SetTextureAlphaMod(p_game->blocks.texture, 128);

	SDL_Point sheet_pos = game_get_block_sheet_pos(p_game,
	                                               BLOCK_SEL + round(sin(p_game->tick / 10)) + 1);

	game_render_isometric_tile(p_game, p_game->cursor, BLOCK_SIZE, p_game->blocks.texture,
	                           sheet_pos, false);

	if (p_game->mode == MODE_VIEWING)
		SDL_SetTextureAlphaMod(p_game->blocks.texture, SDL_ALPHA_OPAQUE);
}

void game_render_map(game_t *p_game) {
	for (int i = 0; i < MAP_SIZE; ++ i) {
		for (int j = 0; j < MAP_SIZE; ++ j)
			game_render_block(p_game, &p_game->map[i][j], j, i);
	}
}

void game_render_ui(game_t *p_game) {
	texture_t mode = text_renderer_render(&p_game->trend, game_mode_name(p_game));

	mode.rect.x = SCREEN_W / 2 - mode.rect.w / 2;
	mode.rect.y = SCREEN_H - mode.rect.h * 2;

	texture_render(&mode, p_game->renderer);

	char gold_str[128] = {0};
	snprintf(gold_str, sizeof(gold_str), "%zu", p_game->gold);

	texture_t gold = text_renderer_render(&p_game->trend, gold_str);

	gold.rect.x = gold.rect.w / 2 + p_game->gold_icon.rect.w + 2;
	gold.rect.y = gold.rect.h / 2;

	p_game->gold_icon.rect.x = gold.rect.w / 2;
	p_game->gold_icon.rect.y = gold.rect.h / 2 - round(p_game->gold_icon.rect.h - gold.rect.h) / 2;
	texture_render(&p_game->gold_icon, p_game->renderer);

	texture_render(&gold, p_game->renderer);
}

SDL_Point game_get_block_sheet_pos(game_t *p_game, block_type_t p_type) {
	return p_game->block_id_pos_map[(int)p_type];
}

block_t *game_cursor_block(game_t *p_game) {
	return &p_game->map[p_game->cursor.y][p_game->cursor.x];
}

bool game_cursor_has_block(game_t *p_game) {
	return p_game->map[p_game->cursor.y][p_game->cursor.x].has_top;
}

const char *game_mode_name(game_t *p_game) {
	switch (p_game->mode) {
	case MODE_VIEWING:  return "Viewing";
	case MODE_PLACING:  return "Placing";
	case MODE_DELETING: return "Deleting";

	default: assert(0 && "Non-existant game mode");
	}
}

void game_shake_screen(game_t *p_game) {
	p_game->shake_timer = SCREEN_SHAKE_TIME;
}

void game_animate_block(game_t *p_game, SDL_Point p_pos) {
	if (p_pos.x < 0 || p_pos.y < 0 || p_pos.x >= MAP_SIZE || p_pos.y >= MAP_SIZE)
		return;

	block_set_timer(&p_game->map[p_pos.y][p_pos.x], BLOCK_ANIM_TIME);
	p_game->map[p_pos.y][p_pos.x].active = true;
}

void game_render(game_t *p_game) {
	SDL_SetRenderDrawColor(p_game->renderer, 66, 30, 66, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(p_game->renderer);

	SDL_SetRenderTarget(p_game->renderer, p_game->screen);
	SDL_RenderClear(p_game->renderer);

	game_render_map(p_game);
	game_render_cursor(p_game);
	game_render_ui(p_game);

	SDL_SetRenderTarget(p_game->renderer, NULL);

	SDL_RenderCopy(p_game->renderer, p_game->screen, NULL, &p_game->screen_rect);
	SDL_RenderPresent(p_game->renderer);
}

void game_events(game_t *p_game) {
	while (SDL_PollEvent(&p_game->event)) {
		switch (p_game->event.type) {
		case SDL_QUIT: p_game->quit = true; break;

		case SDL_KEYDOWN:
			switch (p_game->event.key.keysym.sym) {
			case SDLK_q: p_game->quit = true; break;

			case SDLK_w:
				if (p_game->loaded && p_game->cursor.y > 0)
					-- p_game->cursor.y;

				break;

			case SDLK_a:
				if (p_game->loaded && p_game->cursor.x > 0)
					-- p_game->cursor.x;

				break;

			case SDLK_s:
				if (p_game->loaded && p_game->cursor.y < MAP_SIZE - 1)
					++ p_game->cursor.y;

				break;

			case SDLK_d:
				if (p_game->loaded && p_game->cursor.x < MAP_SIZE - 1)
					++ p_game->cursor.x;

				break;

			case SDLK_RETURN:
				if (p_game->mode == MODE_PLACING && !game_cursor_has_block(p_game)) {
					block_add_top(game_cursor_block(p_game),
					              p_game->cursor_block.top,
					              p_game->cursor_block.dir);

					game_shake_screen(p_game);
				} else if (p_game->mode == MODE_DELETING && game_cursor_has_block(p_game)) {
					block_remove_top(game_cursor_block(p_game));

					game_shake_screen(p_game);
				}

				break;

			case SDLK_ESCAPE:    p_game->mode = MODE_VIEWING;  break;
			case SDLK_e:         p_game->mode = MODE_PLACING;  break;
			case SDLK_BACKSPACE: p_game->mode = MODE_DELETING; break;

			case SDLK_1: p_game->cursor_block.top = BLOCK_CONVEYOR; break;
			case SDLK_2: p_game->cursor_block.top = BLOCK_FINISH;   break;

			case SDLK_r:
				if (p_game->mode == MODE_PLACING) {
					++ p_game->cursor_block.dir;
					if (p_game->cursor_block.dir > DIR_RIGHT)
						p_game->cursor_block.dir = DIR_UP;
				}

				break;

			default: break;
			}

			break;

		default: break;
		}
	}
}

void game_update(game_t *p_game) {
	++ p_game->tick;

	if (p_game->shake_timer > 0) {
		p_game->screen_shake_offset.x = p_game->shake_timer / 2 - rand() % p_game->shake_timer;
		p_game->screen_shake_offset.y = p_game->shake_timer / 2 - rand() % p_game->shake_timer;

		-- p_game->shake_timer;
	}

	if (p_game->loaded || p_game->tick % BLOCK_ANIM_DELAY_BETWEEN_COLUMN != 0)
		return;

	if (p_game->map_anim_it == 0)
		game_animate_block(p_game, p_game->cursor);
	else {
		SDL_Point left = p_game->cursor;
		left.x -= p_game->map_anim_it;

		SDL_Point top = p_game->cursor;
		top.y -= p_game->map_anim_it;

		SDL_Point right = p_game->cursor;
		right.x += p_game->map_anim_it;

		SDL_Point bottom = p_game->cursor;
		bottom.y += p_game->map_anim_it;

		for (SDL_Point pos = left; pos.x != top.x && pos.y != top.y; ++ pos.x, -- pos.y)
			game_animate_block(p_game, pos);

		for (SDL_Point pos = top; pos.x != right.x && pos.y != right.y; ++ pos.x, ++ pos.y)
			game_animate_block(p_game, pos);

		for (SDL_Point pos = right; pos.x != bottom.x && pos.y != bottom.y; -- pos.x, ++ pos.y)
			game_animate_block(p_game, pos);

		for (SDL_Point pos = bottom; pos.x != left.x && pos.y != left.y; -- pos.x, -- pos.y)
			game_animate_block(p_game, pos);
	}

	++ p_game->map_anim_it;
	if (p_game->map_anim_it >= MAP_SIZE)
		p_game->loaded = true;
}
