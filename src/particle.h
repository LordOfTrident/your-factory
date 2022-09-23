#ifndef PARTICLE_H__HEADER_GUARD__
#define PARTICLE_H__HEADER_GUARD__

#include <stdlib.h> /* size_t */

#include "asset.h"

#define FRICTION 0.95

typedef struct {
	float x, y;
	float vel_x, vel_y;

	size_t lifetime, timer;

	asset_t asset;

	float gravity;
} particle_t;

void particle_emit(particle_t *p_particle, asset_t *p_asset, float p_vel_x, float p_vel_y,
                   int p_x, int p_y, size_t p_lifetime, float p_gravity);
void particle_update(particle_t *p_particle);

#endif
