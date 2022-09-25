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

void game_init(game_t *p_game) {
	memset(p_game, 0, sizeof(game_t));

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	/* create window and screen */
	p_game->window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                                  SCREEN_W * SCALE, SCREEN_H * SCALE, SDL_WINDOW_RESIZABLE);
	if (p_game->window == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	p_game->renderer = SDL_CreateRenderer(p_game->window, -1, SDL_RENDERER_ACCELERATED);
	if (p_game->renderer == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	p_game->screen = SDL_CreateTexture(p_game->renderer, SDL_PIXELFORMAT_RGBA8888,
	                                   SDL_TEXTUREACCESS_TARGET, SCREEN_W, SCREEN_H);
	if (p_game->screen == NULL) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	p_game->screen_rect.w = SCREEN_W;
	p_game->screen_rect.h = SCREEN_H;

	/* set config */
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	if (SDL_RenderSetLogicalSize(p_game->renderer, SCREEN_W, SCREEN_H) != 0) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	if (SDL_SetRenderDrawBlendMode(p_game->renderer, SDL_BLENDMODE_BLEND) != 0) {
		SDL_Log("%s", SDL_GetError());

		exit(EXIT_FAILURE);
	}

	assert(MAP_SIZE % 2 != 0);

	p_game->keyboard = SDL_GetKeyboardState(NULL);

	p_game->gold = DEFAULT_GOLD;

	p_game->cursor_block = block_new(BLOCK_CONVEYOR, true);

	/* load assets */
	p_game->font = font_load("./res/font.bmp");
	text_renderer_init(&p_game->text_renderer, p_game->renderer, &p_game->font);

	game_load_asset(p_game, "./res/blocks.bmp",    ASSET_BLOCKS);
	game_load_asset(p_game, "./res/gold.bmp",      ASSET_GOLD);
	game_load_asset(p_game, "./res/particles.bmp", ASSET_PARTICLES);
	game_load_asset(p_game, "./res/arrows.bmp",    ASSET_ARROWS);
	game_load_asset(p_game, "./res/outline.bmp",   ASSET_OUTLINE);
	game_load_asset(p_game, "./res/menu.bmp",      ASSET_MENU);
	game_load_asset(p_game, "./res/icons.bmp",     ASSET_ICONS);

	block_renderer_init(&p_game->block_renderer, p_game->renderer, &p_game->assets[ASSET_BLOCKS]);
	world_init(&p_game->world, &p_game->block_renderer, CAM_POS_X, CAM_POS_Y);

	p_game->img.outline = image_new(&p_game->assets[ASSET_OUTLINE],
	                             CENTERX(p_game->assets[ASSET_OUTLINE].rect.w), 2);

	p_game->img.gold = image_new(&p_game->assets[ASSET_GOLD], 2, 2);

	p_game->img.icons[1] = image_new(&p_game->assets[ASSET_ICONS],
	                                 CENTERX(p_game->assets[ASSET_ICONS].rect.w / 3),
	                                 SCREEN_H - p_game->assets[ASSET_ICONS].rect.h / 2 - 2);
	p_game->img.icons[1].dest.w /= 3;
	p_game->img.icons[1].dest.h /= 2;
	p_game->img.icons[1].src.w  /= 3;
	p_game->img.icons[1].src.h  /= 2;
	p_game->img.icons[1].src.x   = p_game->assets[ASSET_ICONS].rect.w / 3;

	p_game->img.icons[0] = image_new(&p_game->assets[ASSET_ICONS],
	                                 p_game->img.icons[1].dest.x - 24,
	                                 p_game->img.icons[1].dest.y);
	p_game->img.icons[0].dest.w = p_game->img.icons[1].dest.w;
	p_game->img.icons[0].dest.h = p_game->img.icons[1].dest.h;
	p_game->img.icons[0].src    = p_game->img.icons[1].src;
	p_game->img.icons[0].src.x  = 0;

	p_game->img.icons[2] = image_new(&p_game->assets[ASSET_ICONS],
	                                 p_game->img.icons[1].dest.x + 24,
	                                 p_game->img.icons[1].dest.y);
	p_game->img.icons[2].dest.w = p_game->img.icons[1].dest.w;
	p_game->img.icons[2].dest.h = p_game->img.icons[1].dest.h;
	p_game->img.icons[2].src    = p_game->img.icons[1].src;
	p_game->img.icons[2].src.x  = p_game->assets[ASSET_ICONS].rect.w / 3 * 2;

	int x = p_game->img.outline.dest.x - p_game->assets[ASSET_ARROWS].rect.w / 2 - 1;
	int y = p_game->img.outline.dest.y + CENTER(p_game->assets[ASSET_ARROWS].rect.h / 2,
	                                            p_game->img.outline.dest.h) - 1;

	p_game->button.left = button_new(&p_game->assets[ASSET_ARROWS], x, y);
	p_game->button.left.src.w  /= 2;
	p_game->button.left.dest.w /= 2;

	p_game->button.right = button_new(&p_game->assets[ASSET_ARROWS],
	                                  ENDX(p_game->img.outline.dest) + 1, y);
	p_game->button.right.src.w  /= 2;
	p_game->button.right.dest.w /= 2;
	p_game->button.right.src.x   = p_game->button.right.src.w;

	p_game->button.menu = button_new(&p_game->assets[ASSET_MENU],
	                                 SCREEN_W - p_game->assets[ASSET_MENU].rect.w - 2, 2);
}

void game_finish(game_t *p_game) {
	font_free(&p_game->font);
	text_renderer_destroy(&p_game->text_renderer);

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
		game_input(p_game);
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

const char *game_mode_name(game_t *p_game) {
	switch (p_game->mode) {
	case MODE_VIEWING:  return "Viewing";
	case MODE_DELETING: return "Deleting";
	case MODE_PLACING:  return "Placing";
	case MODE_CHANGING: return "Changing";

	default: assert(0 && "game_mode_name(): non-existant game mode");
	}
}

void game_place_cursor_block(game_t *p_game) {
	if (p_game->cursor_block.cost > p_game->gold)
		return;

	p_game->gold -= p_game->cursor_block.cost;
	tile_add_top(world_cursor_tile(&p_game->world), p_game->cursor_block.type,
	             p_game->cursor_block.dir);

	block_set_timer(&world_cursor_tile(&p_game->world)->top, BLOCK_ANIM_TIME);
}

void game_refund_cursor_block(game_t *p_game) {
	p_game->gold += world_cursor_tile(&p_game->world)->top.cost * REFUND_PENALTY;
	tile_remove_top(world_cursor_tile(&p_game->world));

	world_shake(&p_game->world);
	world_emit_particles_at(&p_game->world, &p_game->assets[ASSET_PARTICLES],
	                        p_game->world.cursor.x, p_game->world.cursor.y, 12);
}

void game_render(game_t *p_game) {
	SDL_SetRenderDrawColor(p_game->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(p_game->renderer);

	SDL_SetRenderDrawColor(p_game->renderer, 66, 30, 66, SDL_ALPHA_OPAQUE);
	SDL_SetRenderTarget(p_game->renderer, p_game->screen);
	SDL_RenderClear(p_game->renderer);

	game_render_world(p_game);
	game_render_ui(p_game);

	SDL_SetRenderTarget(p_game->renderer, NULL);

	SDL_RenderCopy(p_game->renderer, p_game->screen, NULL, &p_game->screen_rect);
	SDL_RenderPresent(p_game->renderer);
}

void game_render_world(game_t *p_game) {
	world_render_map(&p_game->world);

	Uint8 a = p_game->paused? 128 : (sin((float)p_game->tick / 10) + 1) * 100;

	switch (p_game->mode) {
	case MODE_VIEWING:  world_render_cursor_inactive(&p_game->world);               break;
	case MODE_DELETING: world_render_cursor_select(&p_game->world, 255, 20, 20, a); break;
	case MODE_PLACING:
		{
			Uint8 r, g, b;
			if (p_game->can_place) {
				r = 0;
				g = 255;
				b = 0;
			} else {
				r = 255;
				g = 20;
				b = 20;
			}

			world_render_cursor_active(&p_game->world, block_get_sprite(&p_game->cursor_block),
			                           r, g, b, a);
		}

		break;

	default: break;
	}
}

void game_render_ui(game_t *p_game) {
	game_render_gold_ui(p_game);
	game_render_mode_ui(p_game);

	button_render(&p_game->button.menu, p_game->renderer);

	for (size_t i = 0; i < SIZE_OF(p_game->img.icons); ++ i) {
		bool highlight_this = p_game->mode == i + 1;
		if (highlight_this)
			p_game->img.icons[i].src.y = p_game->assets[ASSET_ICONS].rect.h / 2;
		else
			image_set_alpha(&p_game->img.icons[i], 128);

		image_render(&p_game->img.icons[i], p_game->renderer);

		if (highlight_this)
			p_game->img.icons[i].src.y = 0;
		else
			image_set_alpha(&p_game->img.icons[i], SDL_ALPHA_OPAQUE);
	}

	if (p_game->mode == MODE_PLACING)
		game_render_placing_ui(p_game);

	if (p_game->paused)
		game_render_paused_ui(p_game);
}

void game_render_placing_ui(game_t *p_game) {
	image_render(&p_game->img.outline, p_game->renderer);

	button_render(&p_game->button.left, p_game->renderer);
	button_render(&p_game->button.right, p_game->renderer);

	if (p_game->cursor_block.cost > p_game->gold)
		block_renderer_set_alpha(&p_game->block_renderer, 128);

	int sel_x = CENTERX(BLOCK_SIZE);
	int sel_y = p_game->img.outline.dest.y + 2;

	int offset = PREV_NEXT_BLOCKS_OFFSET / SHOP_REPOS_TIME * p_game->shop_repos_timer;

	if (p_game->shop_repos_dir == DIR_LEFT)
		sel_x -= offset;
	else
		sel_x += offset;

	block_renderer_render_plain_sprite(&p_game->block_renderer, sel_x, sel_y,
	                                   block_get_sprite(&p_game->cursor_block), BLOCK_SIZE);

	if (p_game->cursor_block.cost > p_game->gold)
		block_renderer_set_alpha(&p_game->block_renderer, SDL_ALPHA_OPAQUE);

	char gold_str[128] = {0};
	snprintf(gold_str, sizeof(gold_str), "%zu", block_type_cost(blocks_shop[p_game->shop_pos]));

	game_render_text_center_x(p_game, ENDY(p_game->img.outline.dest) + 1, gold_str);

	block_t prev = p_game->cursor_block, next = p_game->cursor_block;

	if (p_game->shop_pos + 1 < SIZE_OF(blocks_shop)) {
		block_set_type(&next, blocks_shop[p_game->shop_pos + 1]);

		int x = CENTERX(BLOCK_SIZE) + PREV_NEXT_BLOCKS_OFFSET;
		int y = p_game->img.outline.dest.y + 2;

		if (p_game->shop_repos_dir == DIR_LEFT)
			x -= offset;
		else
			x += offset;

		block_renderer_set_alpha(&p_game->block_renderer, 128);

		block_renderer_render_plain_sprite(&p_game->block_renderer, x, y,
		                                   block_get_sprite(&next), BLOCK_SIZE);

		block_renderer_set_alpha(&p_game->block_renderer, SDL_ALPHA_OPAQUE);
	}

	if (p_game->shop_pos > 0) {
		block_set_type(&prev, blocks_shop[p_game->shop_pos - 1]);

		int x = CENTERX(BLOCK_SIZE) - PREV_NEXT_BLOCKS_OFFSET;
		int y = p_game->img.outline.dest.y + 2;

		if (p_game->shop_repos_dir == DIR_LEFT)
			x -= offset;
		else
			x += offset;

		block_renderer_set_alpha(&p_game->block_renderer, 128);

		block_renderer_render_plain_sprite(&p_game->block_renderer, x, y,
		                                   block_get_sprite(&prev), BLOCK_SIZE);

		block_renderer_set_alpha(&p_game->block_renderer, SDL_ALPHA_OPAQUE);
	}

}

void game_render_gold_ui(game_t *p_game) {
	char gold_str[128] = {0};
	snprintf(gold_str, sizeof(gold_str), "%zu", p_game->gold);

	game_render_text(p_game, ENDX(p_game->img.gold.dest) + 2,
	                 p_game->img.gold.dest.y + CENTER(p_game->font.ch_h, p_game->img.gold.dest.h),
	                 gold_str);

	image_render(&p_game->img.gold, p_game->renderer);
}

void game_render_mode_ui(game_t *p_game) {
	game_render_text_center_x(p_game, SCREEN_H - p_game->font.ch_h * 2 -
	                          p_game->assets[ASSET_ICONS].rect.h / 2, game_mode_name(p_game));
}

void game_render_paused_ui(game_t *p_game) {
	SDL_Rect rect = {
		.w = SCREEN_W,
		.h = SCREEN_H
	};

	SDL_SetRenderDrawColor(p_game->renderer, 0, 0, 0, 180); /* TEMPORARY */
	SDL_RenderFillRect(p_game->renderer, &rect);

	game_render_text_center(p_game, "Paused");
}

void game_render_text(game_t *p_game, int p_x, int p_y, const char *p_text) {
	asset_t asset = text_renderer_render(&p_game->text_renderer, p_text);
	image_t text  = image_new(&asset, p_x, p_y);

	image_render(&text, p_game->renderer);

}

void game_render_text_center(game_t *p_game, const char *p_text) {
	asset_t asset = text_renderer_render(&p_game->text_renderer, p_text);
	image_t text  = image_new(&asset, CENTERX(asset.rect.w), CENTERY(asset.rect.h));

	image_render(&text, p_game->renderer);
}

void game_render_text_center_x(game_t *p_game, int p_y, const char *p_text) {
	asset_t asset = text_renderer_render(&p_game->text_renderer, p_text);
	image_t text  = image_new(&asset, CENTERX(asset.rect.w), p_y);

	image_render(&text, p_game->renderer);
}

void game_input(game_t *p_game) {
	game_events(p_game);
	game_keyboard(p_game);
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

void game_keyboard(game_t *p_game) {
	if (!p_game->paused)
		game_keyboard_ingame(p_game);
}

void game_events_ingame(game_t *p_game) {
	button_events(&p_game->button.menu, &p_game->event, &p_game->mouse);

	if (p_game->mode == MODE_PLACING) {
		button_events(&p_game->button.left,  &p_game->event, &p_game->mouse);
		button_events(&p_game->button.right, &p_game->event, &p_game->mouse);

		if (button_pressed(&p_game->button.left)) {
			if (p_game->shop_repos_timer > 0)
				return;

			if (p_game->shop_pos == 0)
				p_game->shop_pos = SIZE_OF(blocks_shop) - 1;
			else
				-- p_game->shop_pos;

			block_set_type(&p_game->cursor_block, blocks_shop[p_game->shop_pos]);

			p_game->shop_repos_timer = SHOP_REPOS_TIME;
			p_game->shop_repos_dir   = DIR_LEFT;

			return;
		} else if (button_pressed(&p_game->button.right)) {
			if (p_game->shop_repos_timer > 0)
				return;

			++ p_game->shop_pos;
			if (p_game->shop_pos >= SIZE_OF(blocks_shop))
				p_game->shop_pos = 0;

			block_set_type(&p_game->cursor_block, blocks_shop[p_game->shop_pos]);

			p_game->shop_repos_timer = SHOP_REPOS_TIME;
			p_game->shop_repos_dir   = DIR_RIGHT;

			return;
		}
	}

	if (button_pressed(&p_game->button.menu)) {
		p_game->paused = true;

		return;
	}

	switch (p_game->event.type) {
	case SDL_MOUSEWHEEL:
		if (p_game->event.wheel.y > 0) {
			switch (p_game->mode) {
			case MODE_VIEWING:  p_game->mode = MODE_CHANGING; break;
			case MODE_CHANGING: p_game->mode = MODE_DELETING; break;

			default: ++ p_game->mode;
			}
		} else if (p_game->event.wheel.y < 0) {
			switch (p_game->mode) {
			case MODE_VIEWING:  p_game->mode = MODE_DELETING; break;
			case MODE_DELETING: p_game->mode = MODE_CHANGING; break;

			default: -- p_game->mode;
			}
		}

		break;

	case SDL_KEYDOWN:
		switch (p_game->event.key.keysym.sym) {
		case SDLK_RETURN:
			if (p_game->mode == MODE_PLACING && !world_block_at_cursor(&p_game->world))
				game_place_cursor_block(p_game);
			else if (p_game->mode == MODE_DELETING && world_block_at_cursor(&p_game->world))
				game_refund_cursor_block(p_game);

			break;

		case SDLK_ESCAPE: p_game->mode = MODE_VIEWING;  break;
		case SDLK_1:      p_game->mode = MODE_DELETING; break;
		case SDLK_2:      p_game->mode = MODE_PLACING;  break;
		case SDLK_3:      p_game->mode = MODE_CHANGING; break;

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

	world_events(&p_game->world, &p_game->event);
}

void game_keyboard_ingame(game_t *p_game) {
	world_keyboard(&p_game->world, p_game->keyboard);
}

void game_events_paused(game_t *p_game) {
	button_events(&p_game->button.menu, &p_game->event, &p_game->mouse);

	if (button_pressed(&p_game->button.menu)) {
		p_game->paused = false;

		return;
	}

	switch (p_game->event.type) {
	default: break;
	}
}

void game_update(game_t *p_game) {
	++ p_game->tick;

	if (p_game->shop_repos_timer > 0)
		-- p_game->shop_repos_timer;

	if (p_game->mode == MODE_PLACING)
		p_game->can_place = world_cursor_tile(&p_game->world)->top.type == BLOCK_AIR;

	if (!p_game->paused)
		world_update(&p_game->world, p_game->tick);
}
