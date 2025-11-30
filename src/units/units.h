#pragma once

#include "../state.h"
#include "../vendor/GTA-VI/include/engine/types.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Rect {
  float left;
  float right;
  float top;
  float down;
} Homka_Rect;

#define SCALE (3.0f)

Projectile projectile_create();
void projectile_free();

Player player_create();
Enemy enemy_create();

// get unit hitbox size. Generic function
Vector unit_get_size(void *unit);
// get unit hitbox by coordinates. Generic function.
Homka_Rect unit_get_rect(void *unit);
// get centre of hitbox.
Vector unit_get_centre(void *unit);

// Get distance between units. Generic function
float units_distance_between(void *first, void *second);
// Get vector from first unit to second. Generic function;
Vector vector_from_to(void *first, void *second);
float vector_length(Vector vector);
// Normalize vector making their len by 1
Vector vector_normalize(Vector vector);
Vector vector_multiply(Vector vector, float value);
Vector vector_div(Vector vector, float num);
Vector vector_add(Vector first, Vector second);
Vector vector_sub(Vector first, Vector second);
bool units_intersect(void *first, void *second);