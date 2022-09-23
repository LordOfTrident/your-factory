#include "world.h"

void world_init(world_t *p_world, block_renderer_t *p_block_renderer, int p_cam_x, int p_cam_y) {
	memset(p_world, 0, sizeof(world_t));

	p_world->block_renderer = p_block_renderer;

	p_world->cursor.x = MAP_SIZE / 2;
	p_world->cursor.y = MAP_SIZE / 2;

	p_world->cam.x = p_cam_x;
	p_world->cam.y = p_cam_y;

	block_renderer_set_map_pos(p_block_renderer, p_world->cam.x, p_world->cam.y);

	for (int i = 0; i < MAP_SIZE; ++ i) {
		for (int j = 0; j < MAP_SIZE; ++ j)
			p_world->map[i][j] = tile_new(BLOCK_GRASS, false);
	}

	flag_set(&p_world->flags, FLAG_LOAD_MAP, true);
}

void world_load_tile(world_t *p_world, int p_x, int p_y) {
	tile_set_timer(&p_world->map[p_y][p_x], BLOCK_ANIM_TIME);
	tile_set_active(&p_world->map[p_y][p_x], true);
}

void world_load_next(world_t *p_world) {
	if (p_world->map_anim_it == 0)
		world_load_tile(p_world, p_world->cursor.x, p_world->cursor.y);
	else {
		SDL_Point left = p_world->cursor;
		left.x -= p_world->map_anim_it;

		SDL_Point top = p_world->cursor;
		top.y -= p_world->map_anim_it;

		SDL_Point right = p_world->cursor;
		right.x += p_world->map_anim_it;

		SDL_Point bottom = p_world->cursor;
		bottom.y += p_world->map_anim_it;

		for (SDL_Point pos = left; pos.x != top.x && pos.y != top.y; ++ pos.x, -- pos.y) {
			if (pos.x >= 0 && pos.y >= 0 && pos.x < MAP_SIZE && pos.y < MAP_SIZE)
				world_load_tile(p_world, pos.x, pos.y);
		}

		for (SDL_Point pos = top; pos.x != right.x && pos.y != right.y; ++ pos.x, ++ pos.y) {
			if (pos.x >= 0 && pos.y >= 0 && pos.x < MAP_SIZE && pos.y < MAP_SIZE)
				world_load_tile(p_world, pos.x, pos.y);
		}

		for (SDL_Point pos = right; pos.x != bottom.x && pos.y != bottom.y; -- pos.x, ++ pos.y) {
			if (pos.x >= 0 && pos.y >= 0 && pos.x < MAP_SIZE && pos.y < MAP_SIZE)
				world_load_tile(p_world, pos.x, pos.y);
		}

		for (SDL_Point pos = bottom; pos.x != left.x && pos.y != left.y; -- pos.x, -- pos.y) {
			if (pos.x >= 0 && pos.y >= 0 && pos.x < MAP_SIZE && pos.y < MAP_SIZE)
				world_load_tile(p_world, pos.x, pos.y);
		}
	}

	++ p_world->map_anim_it;
	if (p_world->map_anim_it >= MAP_SIZE) {
		flag_set(&p_world->flags, FLAG_MAP_LOADED, true);
		flag_set(&p_world->flags, FLAG_LOAD_MAP,   false);
	}
}

void world_shake(world_t *p_world) {
	p_world->shake_timer = SCREEN_SHAKE_TIME;
}

tile_t *world_cursor_tile(world_t *p_world) {
	return &p_world->map[p_world->cursor.y][p_world->cursor.x];
}

bool world_block_at_cursor(world_t *p_world) {
	return p_world->map[p_world->cursor.y][p_world->cursor.x].top.type != BLOCK_AIR;
}

void world_emit_particles_at(world_t *p_world, asset_t *p_asset,
                             int p_x, int p_y, size_t p_amount) {
	for (size_t i = 0; p_amount > 0; ++ i) {
		if (i > SIZE_OF(p_world->particles))
			assert(0 && "world_emit_particles_at(): no free particles in the particles pool");

		if (p_world->particles[i].timer != 0)
			continue;

		int x = BLOCK_SIZE / 2 - p_asset->rect.w / 2;
		int y = 10 - p_asset->rect.h / 2;

		x += 5 - rand() % 10;
		y -= rand() % 4;

		tile_emit_particle(&p_world->map[p_y][p_x], &p_world->particles[i], p_asset,
		                   (float)(4 - rand() % 8) / 10, -(float)(3 + rand() % 6) / 10,
		                   x, y, 35, 0);

		-- p_amount;
	}
}

void world_render_map(world_t *p_world) {
	for (int i = 0; i < MAP_SIZE; ++ i) {
		for (int j = 0; j < MAP_SIZE; ++ j)
			block_renderer_render_tile(p_world->block_renderer, j, i, &p_world->map[i][j]);
	}
}

void world_render_cursor_inactive(world_t *p_world) {
	block_renderer_set_alpha(p_world->block_renderer, 128);

	block_renderer_render_sprite(p_world->block_renderer, p_world->cursor.x, p_world->cursor.y,
	                             p_world->cursor_sprite, BLOCK_SIZE, LAYER_FLOOR);

	block_renderer_set_alpha(p_world->block_renderer, SDL_ALPHA_OPAQUE);
}

void world_render_cursor_select(world_t *p_world, Uint8 p_r, Uint8 p_g, Uint8 p_b, Uint8 p_a) {
	if (world_cursor_tile(p_world)->top.type != BLOCK_AIR) {
		block_renderer_set_color(p_world->block_renderer, p_r, p_g, p_b);
		block_renderer_set_alpha(p_world->block_renderer, p_a);

		block_renderer_render(p_world->block_renderer, p_world->cursor.x, p_world->cursor.y,
		                      &world_cursor_tile(p_world)->top, LAYER_TOP);

		block_renderer_set_color(p_world->block_renderer, 255, 255, 255);
		block_renderer_set_alpha(p_world->block_renderer, SDL_ALPHA_OPAQUE);
	}

	block_renderer_render_sprite(p_world->block_renderer, p_world->cursor.x, p_world->cursor.y,
	                             p_world->cursor_sprite, BLOCK_SIZE, LAYER_FLOOR);
}

void world_render_cursor_active(world_t *p_world, block_sprite_t p_cursor_block,
                                Uint8 p_r, Uint8 p_g, Uint8 p_b, Uint8 p_a) {
	block_renderer_set_color(p_world->block_renderer, p_r, p_g, p_b);
	block_renderer_set_alpha(p_world->block_renderer, p_a);

	block_renderer_render_sprite(p_world->block_renderer, p_world->cursor.x, p_world->cursor.y,
	                             p_cursor_block, BLOCK_SIZE, LAYER_TOP);

	block_renderer_set_color(p_world->block_renderer, 255, 255, 255);
	block_renderer_set_alpha(p_world->block_renderer, SDL_ALPHA_OPAQUE);

	block_renderer_render_sprite(p_world->block_renderer, p_world->cursor.x, p_world->cursor.y,
	                             p_world->cursor_sprite, BLOCK_SIZE, LAYER_FLOOR);
}

void world_events(world_t *p_world, SDL_Event *p_event) {
	switch (p_event->type) {
	case SDL_KEYDOWN:
		switch (p_event->key.keysym.sym) {
		case SDLK_w:
			if (flag_get(p_world->flags, FLAG_MAP_LOADED) && p_world->cursor.y > 0)
				-- p_world->cursor.y;

			break;

		case SDLK_a:
			if (flag_get(p_world->flags, FLAG_MAP_LOADED) && p_world->cursor.x > 0)
				-- p_world->cursor.x;

			break;

		case SDLK_s:
			if (flag_get(p_world->flags, FLAG_MAP_LOADED) && p_world->cursor.y < MAP_SIZE - 1)
				++ p_world->cursor.y;

			break;

		case SDLK_d:
			if (flag_get(p_world->flags, FLAG_MAP_LOADED) && p_world->cursor.x < MAP_SIZE - 1)
				++ p_world->cursor.x;

			break;

		default: break;
		}

		break;

	default: break;
	}
}

void world_keyboard(world_t *p_world, const Uint8 *p_keyboard) {
	if (p_keyboard[SDL_SCANCODE_RIGHT]) {
		p_world->cam.x -= CAM_SPEED * 2;
		p_world->cam.y -= CAM_SPEED;

		flag_set(&p_world->flags, FLAG_UPDATE_CAM, true);
	}

	if (p_keyboard[SDL_SCANCODE_LEFT]) {
		p_world->cam.x += CAM_SPEED * 2;
		p_world->cam.y += CAM_SPEED;

		flag_set(&p_world->flags, FLAG_UPDATE_CAM, true);
	}

	if (p_keyboard[SDL_SCANCODE_UP]) {
		p_world->cam.x -= CAM_SPEED * 2;
		p_world->cam.y += CAM_SPEED;

		flag_set(&p_world->flags, FLAG_UPDATE_CAM, true);
	}

	if (p_keyboard[SDL_SCANCODE_DOWN]) {
		p_world->cam.x += CAM_SPEED * 2;
		p_world->cam.y -= CAM_SPEED;

		flag_set(&p_world->flags, FLAG_UPDATE_CAM, true);
	}
}

void world_update(world_t *p_world, size_t p_tick) {
	p_world->cursor_sprite = BLOCK_SPRITE_CURSOR + round(sin(p_tick / 10)) + 1;

	for (size_t i = 0; i < MAP_SIZE; ++ i) {
		for (size_t j = 0; j < MAP_SIZE; ++ j)
			tile_update(&p_world->map[i][j]);
	}

	if (p_world->shake_timer > 0) {
		p_world->screen_shake_offset.x = p_world->shake_timer / 2 - rand() % p_world->shake_timer;
		p_world->screen_shake_offset.y = p_world->shake_timer / 2 - rand() % p_world->shake_timer;

		-- p_world->shake_timer;

		flag_set(&p_world->flags, FLAG_UPDATE_CAM, true);
	}

	if (flag_get(p_world->flags, FLAG_UPDATE_CAM)) {
		block_renderer_set_map_pos(p_world->block_renderer,
		                           p_world->cam.x + p_world->screen_shake_offset.x,
		                           p_world->cam.y + p_world->screen_shake_offset.y);

		flag_set(&p_world->flags, FLAG_UPDATE_CAM, false);
	}

	if (!flag_get(p_world->flags, FLAG_MAP_LOADED) && flag_get(p_world->flags, FLAG_LOAD_MAP) &&
	    p_tick % BLOCK_ANIM_DELAY_BETWEEN_COLUMN == 0)
		world_load_next(p_world);
}
