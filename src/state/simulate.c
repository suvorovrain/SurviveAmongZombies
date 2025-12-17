#include "../config.h"
#include "../game.h"
#include "../state.h"
#include "../static_objs.h"
#include "../units/units.h"
#include "engine/coordinates.h"
#include "state.h"
#include <engine/input.h>
#include <engine/map.h>
#include <engine/types.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/time.h>
#endif

typedef struct {
  long long dec_frame;
  long long proc_input;
  long long create_proj;
  long long update_proj;
  long long dmg_kill;
  long long upd_enemy;
  long long dmg_player;
} Timings;

static Timings timings = {0};

static inline long long ns(void) {
#if defined(_WIN32) || defined(_WIN64)
  static LARGE_INTEGER frequency = {0};
  static BOOL frequency_queried = FALSE;
  LARGE_INTEGER counter;

  if (!frequency_queried) {
    QueryPerformanceFrequency(&frequency);
    frequency_queried = TRUE;
  }

  QueryPerformanceCounter(&counter);
  return (long long)((counter.QuadPart * 1000000000LL) / frequency.QuadPart);
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)tv.tv_sec * 1000000000LL + (long long)tv.tv_usec * 1000LL;
#endif
}

#define TIME(fn, field)                                                        \
  do {                                                                         \
    long long _t0 = ns();                                                      \
    fn;                                                                        \
    long long _t1 = ns();                                                      \
    timings.field += _t1 - _t0;                                                \
  } while (0)

#include <stdbool.h>

#define MAX_ENEMIES 3000
#define MAX_PROJECTILES 30000
#define MAX_EXP_CRYSTALS 3000
#define STATIC_COUNT 400

GlobalState init_global_state(Map *map) {
  GlobalState result = {0};
  result.enemies = calloc(MAX_ENEMIES, sizeof(Enemy));
  result.enemies_count = 0;
  result.projectiles = calloc(MAX_PROJECTILES, sizeof(Projectile));
  result.projectiles_count = 0;
  result.exp_crystal = calloc(MAX_EXP_CRYSTALS, sizeof(Crystal));
  result.exp_crystal_count = 0;
  result.status = GAME_ALIVE;
  result.kills = 0;
  result.enemy_factor = 3.0F;
  result.frame_counter = 0;
  result.static_objects = gen_st_objs(map, STATIC_COUNT);
  result.static_objects_count = STATIC_COUNT;

  VectorU32 map_size = map_get_size(map);

  result.player =
      player_create((Vector){(float)map_size.x / 2, (float)map_size.y / 2});

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

// generate enemies every second
static void spawn_enemies(GlobalState *state) {
  if (state->frame_counter % 60 != 0) {
    return;
  }

  // ИСПРАВЛЕННЫЙ PRINTF - ИСПОЛЬЗУЕМ PRIu64 ДЛЯ uint64_t
  printf("count: %" PRIu64 ", factor: %f\n", state->enemies_count,
         state->enemy_factor);

  size_t max_enemies = 0;
  EnemyType type_enemy = ENEMY_SLIME;

  if (state->frame_counter < ((unsigned long)60 * 30 * 1)) {
    type_enemy = ENEMY_SLIME;
    max_enemies = 15;
  } else if (state->frame_counter < ((unsigned long)60 * 30 * 2)) {
    type_enemy = rand() % 2 == 0 ? ENEMY_SLIME : ENEMY_IMP;
    max_enemies = 30;
  } else if (state->frame_counter < ((unsigned long)60 * 30 * 3)) {
    type_enemy = ENEMY_SLIME;
    max_enemies = 50;
  } else if (state->frame_counter < ((unsigned long)60 * 30 * 4)) {
    type_enemy = ENEMY_GOBLIN;
    max_enemies = 40;
  } else {
    type_enemy = rand() % 3;
    max_enemies = 150;
  }

  if (state->enemies_count > max_enemies) {
    return;
  }

  const float radius = (float)500.0;

  // spawn enemies at radius of circle
  for (size_t i = 0; i < (size_t)state->enemy_factor; i++) {
    Vector norm_vector = (Vector){(float)1.0, (float)0.0};
    // [0; 2*PI)
    float random_angle = ((float)rand() / (float)RAND_MAX) * (float)3.14159 * 2;
    Vector vector_radius =
        vector_multiply(vector_rotate(norm_vector, random_angle), radius);

    Enemy new_enemy = enemy_create(
        vector_add(state->player.position, vector_radius), type_enemy);

    state->enemies[state->enemies_count++] = new_enemy;
  }
}

// ... остальной код без изменений ...

void increase_dificilty(GlobalState *state) {
  // Each 7 second more by 5% to enemy spawn
  if (state->frame_counter % (unsigned long)(60 * 7) == 0) {
    state->enemy_factor *= (float)1.05;
  }
}

void make_step(GlobalState *state, Input input, Game *game) {
  TIME(decrease_frame_counters(state), dec_frame);
  spawn_enemies(state);
  TIME(process_input(state, input), proc_input);
  TIME(create_projectiles(state, game), create_proj);
  TIME(update_projectile_positions(state), update_proj);
  TIME(damage_and_kill_enemies(state), dmg_kill);
  TIME(update_enemies_positions(state), upd_enemy);
  TIME(damage_player(state), dmg_player);
  update_animations(state);
  increase_dificilty(state);
  collect_crystals(state);
  teleport_enemies(state);
  move_crystals(state, game);

  state->frame_counter++;

#if false
  if (state->frame_counter % 30 == 0) {
    printf("avg ns per frame:\n");
    printf("  dec_frame:   %lld\n", timings.dec_frame / 30);
    printf("  proc_input:  %lld\n", timings.proc_input / 30);
    printf("  create_proj: %lld\n", timings.create_proj / 30);
    printf("  upd_proj:    %lld\n", timings.update_proj / 30);
    printf("  dmg_kill:    %lld\n", timings.dmg_kill / 30);
    printf("  upd_enemy:   %lld\n", timings.upd_enemy / 30);
    printf("  dmg_player:  %lld\n", timings.dmg_player / 30);

    timings = (Timings){0};
  }
#endif
}