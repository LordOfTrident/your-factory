#include "particle.h"

void particle_emit(particle_t *p_particle, asset_t *p_asset, float p_vel_x, float p_vel_y,
                   int p_x, int p_y, size_t p_lifetime, float p_gravity) {
	p_particle->vel_x = p_vel_x;
	p_particle->vel_y = p_vel_y;
	p_particle->x     = p_x;
	p_particle->y     = p_y;

	p_particle->lifetime = p_lifetime;
	p_particle->timer    = p_lifetime;

	p_particle->asset = *p_asset;

	p_particle->gravity = p_gravity;
}

void particle_update(particle_t *p_particle) {
	if (p_particle->timer == 0)
		return;

	-- p_particle->timer;

	if (p_particle->gravity > 0)
		p_particle->vel_y += p_particle->gravity;

	p_particle->x += p_particle->vel_x;
	p_particle->y += p_particle->vel_y;

	p_particle->vel_x *= FRICTION;
	p_particle->vel_y *= FRICTION;
}
