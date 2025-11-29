// #include "input.h"
#include "../units/units.h"
#include "state.h"
#include <math.h>
#include <stdlib.h>
#include <sys/random.h>

#include <stdbool.h>

#define MAX_ENEMIES 3000
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

  const int range = 6;

  for (float i = -range; i <= range; i++) {
    for (float j = -range; j <= range; j++) {
      if (i == j && i == 0) {
        continue;
      }

      result.enemies[result.enemies_count++] =
          enemy_create((Vector){90.0 * i, 90.0 * j});
    }
  }

  return result;
}

static void decrease_frame_counters(GlobalState *state) {
  uint64_t *invin = &state->player.invincibility_count;

  *invin = *invin == 0 ? 0 : *invin - 1;

  for (size_t i = 0; i < state->projectiles_count; i++) {
    state->projectiles[i].live_frames_last--;

    if (state->projectiles[i].live_frames_last == 0) {
      for (size_t j = i + 1; j < state->projectiles_count; j++) {
        state->projectiles[j - 1] = state->projectiles[j];
      }
      state->projectiles_count--;
      i--;
    }
  }
}

static void process_input(GlobalState *state, Input input) {
  Player *player = &state->player;
  Vector movement = {0.0, 0.0};

  if (input.w)
    movement.y = -1;
  if (input.s)
    movement.y = 1;
  if (input.d)
    movement.x = 1;
  if (input.a)
    movement.x = -1;

  if (movement.x == 0.0 && movement.y == 0.0) {
    return;
  }

  // normalize
  movement = vector_normalize(movement);
  movement = vector_multiply(movement, player->stat_movespeed);

  player->position = vector_add(player->position, movement);

  return;
}

// TODO: attack speed
// create projectile each 3 seconds
static void create_projectiles(GlobalState *state) {
  if (state->frame_counter % (int)(state->player.stat_attack_speed * 60) != 0) {
    return;
  }

  if (state->enemies_count == 0) {
    return;
  }

  // find closest
  float distance = 100000.0;
  Enemy *closest = NULL;
  for (size_t i = 0; i < state->enemies_count; i++) {
    Enemy *enemy = &state->enemies[i];

    float cur_distance = units_distance_between(enemy, &state->player);
    if (cur_distance < distance) {
      distance = cur_distance;
      closest = enemy;
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
    Projectile *proj = &state->projectiles[i];

    if (proj->state == PROJ_EXPLODE)
      continue;

    for (size_t j = 0; j < state->enemies_count; j++) {
      Enemy *enemy = &state->enemies[j];

      if (units_intersect(proj, enemy)) {
        enemy->stat_hp -= proj->stat_damage;
        enemy->state = ENEMY_HURTED;
        proj->kills += 1;

        if (proj->kills == (uint64_t)state->player.stat_piercing) {
          proj->state = PROJ_EXPLODE;
          proj->live_frames_last = 1;
        }
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

    for (size_t i = 0; i < state->enemies_count; i++) {
      for (size_t j = i + 1; j < state->enemies_count; j++) {

        Enemy *a = &state->enemies[i];
        Enemy *b = &state->enemies[j];

        Homka_Rect ra = unit_get_rect(a);
        Homka_Rect rb = unit_get_rect(b);

        if (ra.right <= rb.left || ra.left >= rb.right || ra.down <= rb.top ||
            ra.top >= rb.down) {
          continue;
        }

        float overlap_x = fminf(ra.right, rb.right) - fmaxf(ra.left, rb.left);
        float overlap_y = fminf(ra.down, rb.down) - fmaxf(ra.top, rb.top);

        if (overlap_x < overlap_y) {
          float half = overlap_x * 0.5f;
          a->position.x -= half;
          b->position.x += half;
        } else {
          float half = overlap_y * 0.5f;
          a->position.y -= half;
          b->position.y += half;
        }
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