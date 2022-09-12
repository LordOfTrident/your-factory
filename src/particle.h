#ifndef PARTICLE_H__HEADER_GUARD__
#define PARTICLE_H__HEADER_GUARD__

#include <stdlib.h> /* size_t */

typedef struct {
	float x, y;
	float vel_x, vel_y;

	size_t lifetime, timer;

	float gravity;
} particle_t;

void particle_emit(particle_t *p_particle, float p_vel_x, float p_vel_y,
                   int p_x, int p_y, size_t p_lifetime, float p_gravity);
void particle_update(particle_t *p_particle);

#endif
