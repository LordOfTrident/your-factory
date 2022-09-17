#include "game.h"

block_type_t blocks_shop[] = {
	BLOCK_CONVEYOR,
	BLOCK_FINISH,
	BLOCK_DROPPER
};

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
	                               SCREEN_W * SCALE, SCREEN_H * SCALE, SDL_WINDOW_RESIZABLE);
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

	assert(MAP_SIZE % 2 != 0);

	game.keyboard = SDL_GetKeyboardState(NULL);

	game.gold = DEFAULT_GOLD;

	game.cursor_block = block_new_with_top(BLOCK_GRASS, BLOCK_CONVEYOR);
	game.cursor.x = MAP_SIZE / 2;
	game.cursor.y = MAP_SIZE / 2;

	/* load assets */
	game.font = font_load("./res/font.bmp");
	game.text = text_renderer_new(game.renderer, &game.font);

	game_load_asset(&game, "./res/blocks.bmp",    ASSET_BLOCKS);
	game_load_asset(&game, "./res/icons.bmp",     ASSET_ICONS);
	game_load_asset(&game, "./res/particles.bmp", ASSET_PARTICLES);
	game_load_asset(&game, "./res/arrows.bmp",    ASSET_ARROWS);
	game_load_asset(&game, "./res/outline.bmp",   ASSET_OUTLINE);
	game_load_asset(&game, "./res/menu.bmp",      ASSET_MENU);

	for (int i = 0; i < BLOCKS_COUNT; ++ i) {
		game.block_type_sheet_pos_map[i].x = i % (game.assets[ASSET_BLOCKS].rect.w / BLOCK_SIZE) *
		                                     BLOCK_SIZE;
		game.block_type_sheet_pos_map[i].y = i / (game.assets[ASSET_BLOCKS].rect.w / BLOCK_SIZE) *
		                                     BLOCK_SIZE;
	}

	game.img.outline = image_new(&game.assets[ASSET_OUTLINE],
	                             CENTERX(game.assets[ASSET_OUTLINE].rect.w), 2);

	game.img.gold = image_new(&game.assets[ASSET_ICONS], 2, 2);

	int x = game.img.outline.dest.x - game.assets[ASSET_ARROWS].rect.w / 2 - 1;
	int y = game.img.outline.dest.y + CENTER(game.assets[ASSET_ARROWS].rect.h / 2,
	                                         game.img.outline.dest.h) - 1;

	game.button.left = button_new(&game.assets[ASSET_ARROWS], x, y);
	game.button.left.src.w  /= 2;
	game.button.left.dest.w /= 2;

	game.button.right = button_new(&game.assets[ASSET_ARROWS], ENDX(game.img.outline.dest) + 1, y);
	game.button.right.src.w  /= 2;
	game.button.right.dest.w /= 2;
	game.button.right.src.x   = game.button.right.src.w;

	game.button.menu = button_new(&game.assets[ASSET_MENU],
	                              SCREEN_W - game.assets[ASSET_MENU].rect.w - 2, 2);

	return game;
}

void game_quit(game_t *p_game) {
	font_free(&p_game->font);
	text_renderer_destroy(&p_game->text);

	game_free_assets(p_game);

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
		snprintf(title, sizeof(title), TITLE" | FPS: %zu", p_game->fps);

		SDL_SetWindowTitle(p_game->window, title);

		game_render(p_game);
		game_events(p_game);
		game_update(p_game);

		SDL_Delay(1000 / FPS_CAP);
	} while (!p_game->quit);
}

void game_load_asset(game_t *p_game, const char *p_path, int p_key) {
	p_game->assets[p_key] = asset_load(p_game->renderer, p_path);
}

void game_free_assets(game_t *p_game) {
	for (int i = 0; i < ASSETS_COUNT; ++ i)
		asset_free(&p_game->assets[i]);
}

void game_load_block(game_t *p_game, int p_x, int p_y) {
	block_set_timer(&p_game->map[p_y][p_x], BLOCK_ANIM_TIME);
	p_game->map[p_y][p_x].active = true;
}

SDL_Point game_get_block_sheet_pos(game_t *p_game, block_type_t p_type) {
	return p_game->block_type_sheet_pos_map[(int)p_type];
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

void game_emit_particles_at(game_t *p_game, int p_x, int p_y, size_t p_amount) {
	for (size_t i = 0; p_amount > 0; ++ i) {
		if (i > SIZE_OF(p_game->particles))
			assert(0 && "No free particles in the particles pool");

		if (p_game->particles[i].timer != 0)
			continue;

		int x = BLOCK_SIZE / 2 - p_game->assets[ASSET_PARTICLES].rect.w / 2;
		int y = 10 - p_game->assets[ASSET_PARTICLES].rect.h / 2;

		x += 5 - rand() % 10;
		y -= rand() % 4;

		block_emit_particle(&p_game->map[p_y][p_x], &p_game->particles[i],
		                    &p_game->assets[ASSET_PARTICLES],
		                    (float)(4 - rand() % 8) / 10, -(float)(3 + rand() % 6) / 10,
		                    x, y, 35, 0);

		-- p_amount;
	}
}

void game_shake_screen(game_t *p_game) {
	p_game->shake_timer = SCREEN_SHAKE_TIME;
}

void game_place_cursor_block(game_t *p_game) {
	if (block_type_cost(p_game->cursor_block.top) > p_game->gold)
		return;

	p_game->gold -= block_type_cost(p_game->cursor_block.top);
	block_add_top(game_cursor_block(p_game),
	              p_game->cursor_block.top,
	              p_game->cursor_block.dir);

	block_set_top_timer(game_cursor_block(p_game), BLOCK_ANIM_TIME);
}

void game_refund_cursor_block(game_t *p_game) {
	p_game->gold += game_cursor_block(p_game)->cost * REFUND_PENALTY;
	block_remove_top(game_cursor_block(p_game));

	game_shake_screen(p_game);
	game_emit_particles_at(p_game, p_game->cursor.x, p_game->cursor.y, 12);
}

void game_render(game_t *p_game) {
	SDL_SetRenderDrawColor(p_game->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(p_game->renderer);

	SDL_SetRenderDrawColor(p_game->renderer, 66, 30, 66, SDL_ALPHA_OPAQUE);
	SDL_SetRenderTarget(p_game->renderer, p_game->screen);
	SDL_RenderClear(p_game->renderer);

	game_render_map(p_game);
	game_render_ui(p_game);

	SDL_SetRenderTarget(p_game->renderer, NULL);

	SDL_RenderCopy(p_game->renderer, p_game->screen, NULL, &p_game->screen_rect);
	SDL_RenderPresent(p_game->renderer);
}

void game_render_map(game_t *p_game) {
	for (int i = 0; i < MAP_SIZE; ++ i) {
		for (int j = 0; j < MAP_SIZE; ++ j) {
			game_render_block(p_game, j, i);
			game_render_particles_at(p_game, j, i);
		}
	}

	game_render_cursor(p_game);
}

void game_render_cursor(game_t *p_game) {
	if (p_game->mode == MODE_PLACING || p_game->mode == MODE_DELETING) {
		block_t *block = p_game->mode == MODE_DELETING?
		                 game_cursor_block(p_game) : &p_game->cursor_block;

		if (p_game->mode == MODE_PLACING || block->has_top) {
			SDL_Point sheet_pos = game_get_block_sheet_pos(p_game, block_top_sprite_id(block));

			SDL_SetTextureAlphaMod(p_game->assets[ASSET_BLOCKS].texture,
			                       (sin((float)p_game->tick / 10) + 1) * 100);

			if (p_game->mode == MODE_DELETING || game_cursor_has_block(p_game) ||
			    block_type_cost(p_game->cursor_block.top) > p_game->gold)
				SDL_SetTextureColorMod(p_game->assets[ASSET_BLOCKS].texture, 255, 20, 20);
			else
				SDL_SetTextureColorMod(p_game->assets[ASSET_BLOCKS].texture, 0, 255, 0);

			game_render_isometric_tile(p_game, p_game->cursor.x, p_game->cursor.y, BLOCK_SIZE,
			                           p_game->assets[ASSET_BLOCKS].texture, sheet_pos, true);

			SDL_SetTextureColorMod(p_game->assets[ASSET_BLOCKS].texture, 255, 255, 255);
			SDL_SetTextureAlphaMod(p_game->assets[ASSET_BLOCKS].texture, SDL_ALPHA_OPAQUE);
		}
	}

	if (p_game->mode == MODE_VIEWING)
		SDL_SetTextureAlphaMod(p_game->assets[ASSET_BLOCKS].texture, 128);

	SDL_Point sheet_pos = game_get_block_sheet_pos(p_game,
	                                               BLOCK_SEL + round(sin(p_game->tick / 10)) + 1);

	game_render_isometric_tile(p_game, p_game->cursor.x, p_game->cursor.y, BLOCK_SIZE,
	                           p_game->assets[ASSET_BLOCKS].texture, sheet_pos, false);

	if (p_game->mode == MODE_VIEWING)
		SDL_SetTextureAlphaMod(p_game->assets[ASSET_BLOCKS].texture, SDL_ALPHA_OPAQUE);
}

void game_render_isometric_tile(game_t *p_game, int p_x, int p_y, int p_size,
                                SDL_Texture *p_texture, SDL_Point p_src, bool p_top_layer) {
	SDL_Rect dest = {
		.x = p_x * (BLOCK_SIZE / 2) + p_y * -(BLOCK_SIZE / 2),
		.y = p_x * (BLOCK_SIZE / 4) + p_y *  (BLOCK_SIZE / 4),
		.w = p_size,
		.h = p_size
	};

	/* offset into the middle of the screen */
	dest.x += MAP_POS_X + p_game->screen_shake_offset.x - p_game->cam.x;
	dest.y += MAP_POS_Y + p_game->screen_shake_offset.y - p_game->cam.y;

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

void game_render_block(game_t *p_game, int p_x, int p_y) {
	block_t *block = &p_game->map[p_y][p_x];

	if (!block->active)
		return;

	SDL_Point sheet_pos = game_get_block_sheet_pos(p_game, block->floor);

	int size = BLOCK_SIZE;
	if (block->timer > 0)
		size = (1 - (float)block->timer / (float)block->anim_time) * BLOCK_SIZE;

	game_render_isometric_tile(p_game, p_x, p_y, size,
	                           p_game->assets[ASSET_BLOCKS].texture, sheet_pos, false);

	if (block->has_top) {
		sheet_pos = game_get_block_sheet_pos(p_game, block_top_sprite_id(block));

		size = BLOCK_SIZE;
		if (block->top_timer > 0)
			size = (1 - (float)block->top_timer / (float)block->top_anim_time) * BLOCK_SIZE;

		game_render_isometric_tile(p_game, p_x, p_y, size,
		                           p_game->assets[ASSET_BLOCKS].texture, sheet_pos, true);
	}
}

void game_render_particles_at(game_t *p_game, int p_x, int p_y) {
	int x = p_x * (BLOCK_SIZE / 2) + p_y * -(BLOCK_SIZE / 2);
	int y = p_x * (BLOCK_SIZE / 4) + p_y *  (BLOCK_SIZE / 4);

	/* offset into the middle of the screen */
	x += MAP_POS_X + p_game->screen_shake_offset.x - p_game->cam.x;
	y += MAP_POS_Y + p_game->screen_shake_offset.y - p_game->cam.y;

	block_t *block = &p_game->map[p_y][p_x];

	for (size_t i = 0; i < SIZE_OF(block->particles); ++ i) {
		if (block->particles[i] == NULL)
			continue;

		SDL_Rect dest = {
			.w = p_game->assets[ASSET_PARTICLES].rect.w,
			.h = p_game->assets[ASSET_PARTICLES].rect.h,
			.x = x + block->particles[i]->x,
			.y = y + block->particles[i]->y
		};

		float alpha = (float)block->particles[i]->timer / block->particles[i]->lifetime * 255;
		SDL_SetTextureAlphaMod(p_game->assets[ASSET_PARTICLES].texture, alpha);

		SDL_RenderCopy(p_game->renderer, p_game->assets[ASSET_PARTICLES].texture, NULL, &dest);
	}
}

void game_render_ui(game_t *p_game) {
	game_render_gold_ui(p_game);
	game_render_mode_ui(p_game);

	button_render(&p_game->button.menu, p_game->renderer);

	if (p_game->mode == MODE_PLACING)
		game_render_placing_ui(p_game);

	if (p_game->paused)
		game_render_paused_ui(p_game);
}

void game_render_placing_ui(game_t *p_game) {
	image_render(&p_game->img.outline, p_game->renderer);

	button_render(&p_game->button.left, p_game->renderer);
	button_render(&p_game->button.right, p_game->renderer);

	SDL_Point sheet_pos = game_get_block_sheet_pos(p_game, p_game->cursor_block.top);

	SDL_Rect dest = {
		.x = p_game->img.outline.dest.x + 2,
		.y = p_game->img.outline.dest.y + 2,
		.w = BLOCK_SIZE,
		.h = BLOCK_SIZE
	};

	SDL_Rect src = {
		.x = sheet_pos.x,
		.y = sheet_pos.y,
		.w = BLOCK_SIZE,
		.h = BLOCK_SIZE
	};

	if (block_type_cost(p_game->cursor_block.top) > p_game->gold)
		SDL_SetTextureAlphaMod(p_game->assets[ASSET_BLOCKS].texture, 128);

	SDL_RenderCopy(p_game->renderer, p_game->assets[ASSET_BLOCKS].texture, &src, &dest);

	if (block_type_cost(p_game->cursor_block.top) > p_game->gold)
		SDL_SetTextureAlphaMod(p_game->assets[ASSET_BLOCKS].texture, SDL_ALPHA_OPAQUE);

	char gold_str[128] = {0};
	snprintf(gold_str, sizeof(gold_str), "%zu", block_type_cost(blocks_shop[p_game->shop_pos]));

	asset_t gold_asset = text_renderer_render(&p_game->text, gold_str);
	image_t gold = image_new(&gold_asset, CENTERX(gold_asset.rect.w),
	                         ENDY(p_game->img.outline.dest) + 1);

	image_render(&gold, p_game->renderer);
}

void game_render_gold_ui(game_t *p_game) {
	char gold_str[128] = {0};
	snprintf(gold_str, sizeof(gold_str), "%zu", p_game->gold);

	asset_t gold_asset = text_renderer_render(&p_game->text, gold_str);
	image_t gold = image_new(&gold_asset, ENDX(p_game->img.gold.dest) + 2,
	                         p_game->img.gold.dest.y + CENTER(p_game->font.ch_h,
	                                                          p_game->img.gold.dest.h));

	image_render(&p_game->img.gold, p_game->renderer);
	image_render(&gold, p_game->renderer);
}

void game_render_mode_ui(game_t *p_game) {
	asset_t mode_asset = text_renderer_render(&p_game->text, game_mode_name(p_game));
	image_t mode = image_new(&mode_asset, CENTERX(mode_asset.rect.w),
	                         SCREEN_H - mode_asset.rect.h * 2);

	image_render(&mode, p_game->renderer);
}

void game_render_paused_ui(game_t *p_game) {
	asset_t paused_asset = text_renderer_render(&p_game->text, "Paused");
	image_t paused = image_new(&paused_asset, CENTERX(paused_asset.rect.w),
	                           CENTERY(paused_asset.rect.h));

	SDL_Rect rect = {
		.w = SCREEN_W,
		.h = SCREEN_H
	};

	SDL_SetRenderDrawColor(p_game->renderer, 0, 0, 0, 180); /* TEMPORARY */
	SDL_RenderFillRect(p_game->renderer, &rect);

	image_render(&paused, p_game->renderer);
}

void game_events(game_t *p_game) {
	while (SDL_PollEvent(&p_game->event)) {
		switch (p_game->event.type) {
		case SDL_QUIT: p_game->quit = true; break;

		case SDL_MOUSEMOTION:
			p_game->mouse.x = p_game->event.motion.x;
			p_game->mouse.y = p_game->event.motion.y;

			break;

		default: break;
		}

		if (p_game->paused)
			game_events_paused(p_game);
		else
			game_events_ingame(p_game);
	}
}

void game_events_ingame(game_t *p_game) {
	button_events(&p_game->button.menu, &p_game->event, &p_game->mouse);

	if (p_game->mode == MODE_PLACING) {
		button_events(&p_game->button.left,  &p_game->event, &p_game->mouse);
		button_events(&p_game->button.right, &p_game->event, &p_game->mouse);

		if (button_pressed(&p_game->button.left)) {
			if (p_game->shop_pos == 0)
				p_game->shop_pos = SIZE_OF(blocks_shop) - 1;
			else
				-- p_game->shop_pos;

			p_game->cursor_block.top = blocks_shop[p_game->shop_pos];
		} else if (button_pressed(&p_game->button.right)) {
			++ p_game->shop_pos;
			if (p_game->shop_pos >= SIZE_OF(blocks_shop))
				p_game->shop_pos = 0;

			p_game->cursor_block.top = blocks_shop[p_game->shop_pos];
		}
	}

	if (button_pressed(&p_game->button.menu))
		p_game->paused = true;

	bool drag_screen = !p_game->button.right.hovered && !p_game->button.left.hovered &&
	                   !p_game->button.menu.hovered;

	switch (p_game->event.type) {
	case SDL_MOUSEBUTTONUP: p_game->dragging = false; break;
	case SDL_MOUSEBUTTONDOWN:
		if (drag_screen) {
			p_game->dragging         = true;
			p_game->mouse_drag_begin = p_game->mouse;
			p_game->prev_cam         = p_game->cam;
		}

		break;

	case SDL_MOUSEMOTION:
		if (p_game->dragging) {
			p_game->cam.x = (p_game->mouse_drag_begin.x - p_game->mouse.x) /
			                MOUSE_DRAG_FRICTION + p_game->prev_cam.x;

			p_game->cam.y = (p_game->mouse_drag_begin.y - p_game->mouse.y) /
			                MOUSE_DRAG_FRICTION + p_game->prev_cam.y;
		}

		break;

	case SDL_KEYDOWN:
		switch (p_game->event.key.keysym.sym) {
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
			if (p_game->mode == MODE_PLACING && !game_cursor_has_block(p_game))
				game_place_cursor_block(p_game);
			else if (p_game->mode == MODE_DELETING && game_cursor_has_block(p_game))
				game_refund_cursor_block(p_game);

			break;

		case SDLK_ESCAPE:    p_game->mode = MODE_VIEWING;  break;
		case SDLK_e:         p_game->mode = MODE_PLACING;  break;
		case SDLK_BACKSPACE: p_game->mode = MODE_DELETING; break;

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

void game_events_paused(game_t *p_game) {
	button_events(&p_game->button.menu, &p_game->event, &p_game->mouse);

	if (button_pressed(&p_game->button.menu))
		p_game->paused = false;

	switch (p_game->event.type) {
	default: break;
	}
}

void game_update(game_t *p_game) {
	++ p_game->tick;

	for (size_t i = 0; i < MAP_SIZE; ++ i) {
		for (size_t j = 0; j < MAP_SIZE; ++ j) {
			if (p_game->map[i][j].timer > 0)
				-- p_game->map[i][j].timer;

			if (p_game->map[i][j].top_timer > 0)
				-- p_game->map[i][j].top_timer;

			block_update_particles(&p_game->map[i][j]);
		}
	}

	if (p_game->shake_timer > 0) {
		p_game->screen_shake_offset.x = p_game->shake_timer / 2 - rand() % p_game->shake_timer;
		p_game->screen_shake_offset.y = p_game->shake_timer / 2 - rand() % p_game->shake_timer;

		-- p_game->shake_timer;
	}

	if (p_game->loaded || p_game->tick % BLOCK_ANIM_DELAY_BETWEEN_COLUMN != 0)
		return;

	if (p_game->map_anim_it == 0)
		game_load_block(p_game, p_game->cursor.x, p_game->cursor.y);
	else {
		SDL_Point left = p_game->cursor;
		left.x -= p_game->map_anim_it;

		SDL_Point top = p_game->cursor;
		top.y -= p_game->map_anim_it;

		SDL_Point right = p_game->cursor;
		right.x += p_game->map_anim_it;

		SDL_Point bottom = p_game->cursor;
		bottom.y += p_game->map_anim_it;

		for (SDL_Point pos = left; pos.x != top.x && pos.y != top.y; ++ pos.x, -- pos.y) {
			if (pos.x >= 0 && pos.y >= 0 && pos.x < MAP_SIZE && pos.y < MAP_SIZE)
				game_load_block(p_game, pos.x, pos.y);
		}

		for (SDL_Point pos = top; pos.x != right.x && pos.y != right.y; ++ pos.x, ++ pos.y) {
			if (pos.x >= 0 && pos.y >= 0 && pos.x < MAP_SIZE && pos.y < MAP_SIZE)
				game_load_block(p_game, pos.x, pos.y);
		}

		for (SDL_Point pos = right; pos.x != bottom.x && pos.y != bottom.y; -- pos.x, ++ pos.y) {
			if (pos.x >= 0 && pos.y >= 0 && pos.x < MAP_SIZE && pos.y < MAP_SIZE)
				game_load_block(p_game, pos.x, pos.y);
		}

		for (SDL_Point pos = bottom; pos.x != left.x && pos.y != left.y; -- pos.x, -- pos.y) {
			if (pos.x >= 0 && pos.y >= 0 && pos.x < MAP_SIZE && pos.y < MAP_SIZE)
				game_load_block(p_game, pos.x, pos.y);
		}
	}

	++ p_game->map_anim_it;
	if (p_game->map_anim_it >= MAP_SIZE)
		p_game->loaded = true;
}
