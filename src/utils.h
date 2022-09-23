#ifndef UTILS_H__HEADER_GUARD__
#define UTILS_H__HEADER_GUARD__

#include <stdlib.h>  /* size_t, malloc, realloc, free */
#include <string.h>  /* memcpy, strlen */
#include <assert.h>  /* asset */
#include <stdbool.h> /* bool, true, false */

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include <SDL2/SDL.h>

#define SIZE_OF(p_arr) (sizeof(p_arr) / sizeof(p_arr[0]))

#define SMEMALLOC(p_ptr, p_size)   (p_ptr = memalloc(sizeof(p_ptr[0]) * p_size))
#define SMEMREALLOC(p_ptr, p_size) (p_ptr = memrealloc(p_ptr, sizeof(p_ptr[0]) * p_size))
#define SMEMFREE(p_ptr)            (memfree((void**)&p_ptr))

#define CENTER(p_size, p_screen_size) (p_screen_size / 2 - p_size / 2)
#define ENDX(p_rect) (p_rect.x + p_rect.w)
#define ENDY(p_rect) (p_rect.y + p_rect.h)

void *memalloc(size_t p_size);
void *memrealloc(void *p_ptr, size_t p_size);
void  memfree(void **p_ptr);

void *memalloccopy(const void *p_ptr, size_t p_size);

char *copy_str(const char *p_str);

void flag_set(int *p_value, int p_flag, bool p_set);
bool flag_get(int p_value, int p_flag);

SDL_Color SDL_GetSurfacePixel(SDL_Surface *p_surface, int p_x, int p_y);

#endif
