// #include "input.h"
#include "../units/units.h"
#include "state.h"
#include <math.h>
#include <stdlib.h>

#include <stdbool.h>

#define MAX_ENEMIES 300
#define MAX_PROJECTILES 300

GlobalState init_global_state() {
  GlobalState result = {0};
  result.enemies = calloc(sizeof(Enemy), MAX_ENEMIES);
  result.enemies_count = 0;
  result.projectiles = calloc(sizeof(Projectile), MAX_PROJECTILES);
  result.projectiles_count = 0;
  result.status = GAME_ALIVE;
  result.kills = 0;
  result.frame_counter = 0;

  result.player = player_create();
}

static void decrease_frame_counters(GlobalState *state) {
  uint64_t *invin = &state->player.invincibility_count;

  *invin = *invin == 0 ? 0 : *invin - 1;
}

static void process_input(GlobalState *state, Input input) {
  Player player = state->player;
  float *x = &player.movement.x;
  float *y = &player.movement.y;

  if (input.w)
    *y = 1;
  if (input.s)
    *y = -1;
  if (input.d)
    *x = 1;
  if (input.a)
    *x = -1;

  // normalize
  double length = sqrtl((*x) * (*x) + (*y) * (*y));
  *y = *y / length;
  *x = *x / length;

  return;
}

// TODO: attack speed
// create projectile each 3 seconds
static void create_projectiles(GlobalState *state) {
  if (state->frame_counter % 180 != 0) {
    return;
  }

  if (state->enemies_count == 0) {
    return;
  }

  // find closest
  float distance = 100000.0;
  Enemy *closest = NULL;
  for (size_t i = 0; i < state->enemies_count; i++) {
    Enemy enemy = state->enemies[i];

    float cur_distance = units_distance_between(&enemy, &state->player);
    if (cur_distance < distance) {
      distance = cur_distance;
      closest = &enemy;
    }
  }
  Vector movement = vector_from_to(&state->player, closest);
  movement = vector_normalize(movement);

  Projectile projectile = projectile_create(state->player, movement);

  state->projectiles[state->projectiles_count] = projectile;
  state->projectiles_count++;

  return;
}

// Update position of projectiles (why i wrote this comment?)
static void update_projectile_positions(GlobalState *state) {
  for (size_t i = 0; i < state->projectiles_count; i++) {
    Projectile *proj = &state->projectiles[i];

    Vector movement = vector_multiply(proj->movement, proj->stat_movespeed);
    Vector result = vector_add(proj->position, movement);

    proj->position = result;
  }

  return;
}

// TODO: fix deleting from arrays this
static void damage_and_kill_enemies(GlobalState *state) {
  for (size_t i = 0; i < state->projectiles_count; i++) {
    for (size_t j = 0; j < state->enemies_count; j++) {
      Projectile *proj = &state->projectiles[i];
      Enemy *enemy = &state->enemies[j];

      if (units_intersect(&proj, &enemy)) {
        enemy->stat_hp -= proj->stat_damage;
        enemy->state = ENEMY_HURTED;
        proj->state = PROJ_EXPLODE;
      }
    }
  }

  for (size_t i = 0; i < state->enemies_count; i++) {
    Enemy *enemy = &state->enemies[i];

    if (enemy->stat_hp > 0) {
      continue;
    }

    // remove enemy from array.
    for (size_t j = i + 1; j < state->enemies_count; j++) {
      state->enemies[j - 1] = state->enemies[j];
    }

    state->enemies_count--;
    i--;
  }

  return;
}

static void update_enemies_positions(GlobalState *state) {
  // update movement vector to player
  for (size_t i = 0; i < state->enemies_count; i++) {
    state->enemies[i].movement =
        vector_normalize(vector_from_to(&state->enemies[i], &state->player));
  }

  // update enemy positions
  for (size_t i = 0; i < state->enemies_count; i++) {
    Enemy *enemy = &state->enemies[i];

    Vector movement = vector_multiply(enemy->movement, enemy->stat_movespeed);
    Vector result = vector_add(enemy->position, movement);

    enemy->position = result;
  }

  // iteration algorithm for resolve collisions
  for (size_t iteration = 0; iteration < 8; iteration++) {
    // TODO: sort by collision power
    for (size_t i = 0; i < state->enemies_count; i++) {
      for (size_t j = 0; j < state->enemies_count; j++) {
        if (i == j)
          continue;

        Enemy *first = &state->enemies[i];
        float first_len = vector_length(unit_get_size(first));
        Enemy *second = &state->enemies[j];
        float second_len = vector_length(unit_get_size(second));

        if (!units_intersect(first, second))
          continue;

        Vector first_center = unit_get_centre(first);
        Vector second_center = unit_get_centre(second);

        Vector delta = vector_sub(second_center, first_center);

        // TODO: fix collision movement
        Vector delta_norm = vector_normalize(delta);
        Vector first_movement = vector_multiply(delta_norm, -first_len / 2.0);
        Vector second_movement = vector_multiply(delta_norm, -second_len / 2.0);
        first->position = vector_add(first->position, first_movement);
        second->position = vector_add(second->position, second_movement);
      }
    }
  }

  return;
}

static void damage_player(GlobalState *state) {
  for (size_t i = 0; i < state->enemies_count; i++) {
    Enemy *enemy = &state->enemies[i];
    Player *player = &state->player;

    if (units_intersect(&state->player, &enemy)) {
      player->stat_hp -= enemy->stat_damage;
      player->state = PLAYER_HURTED;
      player->invincibility_count = 30; // half of second invincible
    }
  }
}

void make_step(GlobalState *state, Input input) {
  decrease_frame_counters(state);
  process_input(state, input);
  create_projectiles(state);
  update_projectile_positions(state);
  damage_and_kill_enemies(state);
  update_enemies_positions(state);
  damage_player(state);

  return;
}