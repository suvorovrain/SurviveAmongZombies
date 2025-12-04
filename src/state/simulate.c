// #include "input.h"
#include "../config.h"
#include "../game.h"
#include "../units/units.h"
#include "engine/coordinates.h"
#include "math.h"
#include "state.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <sys/time.h>

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

static inline long long ns() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)tv.tv_sec * 1000000000LL + (long long)tv.tv_usec * 1000LL;
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

GlobalState init_global_state(Map *map) {
  GlobalState result = {0};
  result.enemies = calloc(sizeof(Enemy), MAX_ENEMIES);
  result.enemies_count = 0;
  result.projectiles = calloc(sizeof(Projectile), MAX_PROJECTILES);
  result.projectiles_count = 0;
  result.exp_crystal = calloc(sizeof(ExpCrystal), MAX_EXP_CRYSTALS);
  result.exp_crystal_count = 0;
  result.status = GAME_ALIVE;
  result.kills = 0;
  result.enemy_factor = 3.0f;
  result.frame_counter = 0;

  VectorU32 map_size = map_get_size(map);

  result.player = player_create((Vector){map_size.x / 2, map_size.y / 2});

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
static void spawn_enemies(GlobalState *state, Game *game) {
  if (state->frame_counter % 60 != 0) {
    return;
  }

  printf("count: %ld, factor: %f\n", state->enemies_count, state->enemy_factor);

  if (state->enemies_count > 200) {
    return;
  }

  const float radius = 500.0;

  // spawn enemies at radius of circle
  for (size_t i = 0; i < (size_t)state->enemy_factor; i++) {
    Vector norm_vector = (Vector){1.0, 0.0};
    // [0; 2*PI)
    float random_angle = ((float)rand() / (float)RAND_MAX) * 3.14159 * 2;
    Vector vector_radius =
        vector_multiply(vector_rotate(norm_vector, random_angle), radius);

    Enemy new_enemy =
        enemy_create(vector_add(state->player.position, vector_radius));

    state->enemies[state->enemies_count++] = new_enemy;
  }
}

static void process_input(GlobalState *state, Input input) {
  Player *player = &state->player;
  Vector movement = {0.0, 0.0};

  if (input.w) {
    movement.y = -1;
    player->direction_face = DIRECTION_UP;
  }
  if (input.s) {
    movement.y = 1;
    player->direction_face = DIRECTION_DOWN;
  }
  if (input.d) {
    movement.x = 1;
    player->direction_side = DIRECTION_RIGHT;
  }
  if (input.a) {
    movement.x = -1;
    player->direction_side = DIRECTION_LEFT;
  }

  if (movement.x == 0.0 && movement.y == 0.0) {
    return;
  }

  // normalize
  movement = vector_normalize(movement);
  movement = vector_multiply(
      movement, (player->stat_movespeed * player->boost_movement_percent));

  player->position = vector_add(player->position, movement);

  return;
}

// TODO: attack speed
// create projectile each 3 seconds
static void create_projectiles(GlobalState *state, Game *game) {
  if (state->frame_counter %
          (int)((state->player.stat_attack_speed /
                 (state->player.boost_attack_speed_percent)) *
                60) !=
      0) {
    return;
  }

  if (state->enemies_count == 0) {
    return;
  }

  // find closests
  Enemy *closests =
      calloc((size_t)state->player.stat_proj_count, sizeof(Enemy));
  size_t *indicies =
      calloc((size_t)state->player.stat_proj_count, sizeof(size_t));
  float *distances =
      calloc((size_t)state->player.stat_proj_count, sizeof(float));
  for (size_t i = 0; i < (size_t)state->player.stat_proj_count; i++) {
    closests[i] = state->enemies[0];
    indicies[i] = 0;
    distances[i] = 100000.0;
  }

  size_t max_local = 0;
  for (size_t i = 0; i < state->enemies_count; i++) {
    Enemy *enemy = &state->enemies[i];

    float cur_distance = units_distance_between(enemy, &state->player);
    if (cur_distance < distances[max_local]) {
      distances[max_local] = cur_distance;
      indicies[max_local] = i;
      closests[max_local] = state->enemies[i];

      max_local = 0;
      for (size_t j = 0; j < (size_t)state->player.stat_proj_count; j++) {
        if (distances[j] > distances[max_local]) {
          max_local = j;
        }
      }
    }
  }

  for (size_t i = 0; i < (size_t)state->player.stat_proj_count; i++) {
    if (!is_visible(game->engine, closests[i].position)) {
      continue;
    }

    Vector movement = vector_from_to(&state->player, &closests[i]);
    movement = vector_normalize(movement);

    Projectile projectile = projectile_create(state->player, movement);

    state->projectiles[state->projectiles_count] = projectile;
    state->projectiles_count++;
  }

  return;
}

// Update position of projectiles (why i wrote this comment?)
static void update_projectile_positions(GlobalState *state) {
  for (size_t i = 0; i < state->projectiles_count; i++) {
    Projectile *proj = &state->projectiles[i];

    if (state->projectiles[i].state == PROJ_EXPLODE) {
      continue;
    }

    Vector movement = vector_multiply(proj->movement, proj->stat_movespeed);
    Vector result = vector_add(proj->position, movement);

    proj->position = result;
  }

  return;
}

// TODO: fix deleting from arrays this
static void damage_and_kill_enemies(GlobalState *state, Game *game) {
  for (size_t i = 0; i < state->projectiles_count; i++) {
    Projectile *proj = &state->projectiles[i];

    if (proj->state == PROJ_EXPLODE)
      continue;

    for (size_t j = 0; j < state->enemies_count; j++) {
      Enemy *enemy = &state->enemies[j];

      if (units_intersect(proj, enemy, 1.0f)) {
        enemy->stat_hp -=
            proj->stat_damage * state->player.boost_damage_percent;
        enemy->state = ENEMY_HURTED;
        proj->kills += 1;

        if (proj->kills == (uint64_t)(state->player.stat_piercing *
                                      state->player.boost_piercing_percent)) {
          proj->state = PROJ_EXPLODE;
          proj->explode_frame = state->frame_counter;
          proj->live_frames_last = 12 * PROJ_EXPLODE_FRAMES_COUNT;
          proj->position = vector_add(
              proj->position, (Vector){-8.0 * SCALE * 0.5, -8.0 * SCALE * 0.5});
        }
      }
    }
  }

  for (size_t i = 0; i < state->enemies_count; i++) {
    Enemy *enemy = &state->enemies[i];
    Vector enemy_pos = enemy->position;

    if (enemy->stat_hp > 0) {
      continue;
    }

    // remove enemy from array.
    for (size_t j = i + 1; j < state->enemies_count; j++) {
      state->enemies[j - 1] = state->enemies[j];
    }

    state->enemies_count--;
    i--;
    state->kills += 1;

    state->exp_crystal[state->exp_crystal_count++] =
        exp_crystal_create(enemy_pos);

    // state->player.stat_experience +=
    //     100.0 * state->player.boost_experience_percent;

    // if (state->player.stat_experience >=
    //     state->player.stat_experience_for_lvlup) {
    //   state->status = GAME_LEVEL_UP;

    //   game->level_menu_first = rand() % LVLUP_COUNT;

    //   do {
    //     game->level_menu_second = rand() % LVLUP_COUNT;
    //   } while (game->level_menu_second == game->level_menu_first);

    //   do {
    //     game->level_menu_third = rand() % LVLUP_COUNT;
    //   } while (game->level_menu_third == game->level_menu_first ||
    //            game->level_menu_third == game->level_menu_second);
    // }
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

  const int PADDING = 3.0;

  Homka_Rect rects[state->enemies_count];
  for (size_t i = 0; i < state->enemies_count; i++) {
    rects[i] = unit_get_rect(&state->enemies[i]);
    rects[i].left += PADDING * SCALE;
    rects[i].right -= PADDING * SCALE;
    rects[i].top += PADDING * SCALE;
    rects[i].down -= PADDING * SCALE;
  }

  const size_t enemy_n = state->enemies_count;
  // iteration algorithm for resolve collisions
  for (size_t iteration = 0; iteration < 8; iteration++) {
    for (size_t i = 0; i < enemy_n; i++) {
      for (size_t j = i + 1; j < enemy_n; j++) {
        Homka_Rect ra = rects[i];
        Homka_Rect rb = rects[j];

        if (ra.right <= rb.left || ra.left >= rb.right || ra.down <= rb.top ||
            ra.top >= rb.down) {
          continue;
        }

        float overlap_x = fminf(ra.right, rb.right) - fmaxf(ra.left, rb.left);
        float overlap_y = fminf(ra.down, rb.down) - fmaxf(ra.top, rb.top);

        Enemy *a = &state->enemies[i];
        Enemy *b = &state->enemies[j];

        if (overlap_x < overlap_y) {
          float half = overlap_x * 0.5f;
          a->position.x -= half;
          b->position.x += half;
          rects[i].left -= half;
          rects[i].right -= half;
          rects[j].left += half;
          rects[j].right += half;
        } else {
          float half = overlap_y * 0.5f;
          a->position.y -= half;
          b->position.y += half;
          rects[i].top -= half;
          rects[i].down -= half;
          rects[j].top += half;
          rects[j].down += half;
        }
      }
    }
  }

  return;
}

static void damage_player(GlobalState *state) {
  if (state->player.invincibility_count > 0) {
    return;
  }

  for (size_t i = 0; i < state->enemies_count; i++) {
    Enemy *enemy = &state->enemies[i];
    Player *player = &state->player;

    if (units_intersect(player, enemy, 5.0f)) {
      player->stat_hp -= enemy->stat_damage;
      player->state = PLAYER_HURTED;
      player->invincibility_count = 25; // time for invincible

      if (player->stat_hp < 0.0) {
        state->status = GAME_DEAD;
      }
    }
  }
}

static void update_animations(GlobalState *state) {
  // Projectiles
  for (size_t i = 0; i < state->projectiles_count; i++) {
    Projectile *proj = &state->projectiles[i];

    if (proj->state == PROJ_WALK)
      continue;

    uint64_t frames_diff = state->frame_counter - proj->explode_frame;
    if (frames_diff / 4 > proj->spritesheet_explode.frames_count) {
      fprintf(stderr, "Wrong explode animation calculation\n");
      exit(-1);
    }

    proj->current_sprite = proj->spritesheet_explode.frames[frames_diff / 4];
  }

  // Player
  Player *player = &state->player;
  size_t frame = 0;
  if (player->direction_face == DIRECTION_UP)
    frame += 8;
  if (player->direction_side == DIRECTION_LEFT)
    frame += 4;

  frame += (state->frame_counter % (PLAYER_WALK_FRAME_COUNT * 4)) /
           PLAYER_WALK_FRAME_COUNT;

  player->current_sprite = player->spritesheet_move.frames[frame];

  // Enemies
  for (size_t i = 0; i < state->enemies_count; i++) {
    Enemy *enemy = &state->enemies[i];

    Vector movement = vector_from_to(enemy, &state->player);
    size_t frame_enemy = 0;

    if (movement.y < 0) {
      frame_enemy += 4;
    }

    frame_enemy += (state->frame_counter % (ENEMY_WALK_FRAME_COUNT * 2)) /
                   ENEMY_WALK_FRAME_COUNT;
    enemy->current_sprite = enemy->spritesheet_move.frames[frame_enemy];
  }

  // Exp Crystal
  for (size_t i = 0; i < state->exp_crystal_count; i++) {
    ExpCrystal *crystal = &state->exp_crystal[i];

    size_t frame = (state->frame_counter % (EXP_CRYSTAL_FRAME_COUNT * 4)) /
                   EXP_CRYSTAL_FRAME_COUNT;

    if (state->frame_counter % 4 == 0) {
      if (state->frame_counter % 60 < 30) {
        crystal->position = vector_add(crystal->position, (Vector){0.0, 1.0f});
      } else {
        crystal->position = vector_add(crystal->position, (Vector){0.0, -1.0f});
      }
    }
  }
}

static void collect_crystals(GlobalState *state, Game *game) {
  for (size_t i = 0; i < state->exp_crystal_count; i++) {
    ExpCrystal *crystal = &state->exp_crystal[i];

    float distance = vector_length(vector_from_to(crystal, &state->player));

    if (distance < 40.0f) {
      for (size_t j = i + 1; j < state->exp_crystal_count; j++) {
        state->exp_crystal[j - 1] = state->exp_crystal[j];
      }
      state->exp_crystal_count--;

      state->player.stat_experience +=
          2.0 * state->player.boost_experience_percent;

      if (state->player.stat_experience >=
          state->player.stat_experience_for_lvlup) {
        state->status = GAME_LEVEL_UP;

        game->level_menu_first = rand() % LVLUP_COUNT;

        do {
          game->level_menu_second = rand() % LVLUP_COUNT;
        } while (game->level_menu_second == game->level_menu_first);

        do {
          game->level_menu_third = rand() % LVLUP_COUNT;
        } while (game->level_menu_third == game->level_menu_first ||
                 game->level_menu_third == game->level_menu_second);
      }
    }
  }
}

// if enemies far from player -- teleport them to random angle from player
static void teleport_enemies(GlobalState *state, Game *game) {
  const float radius = 500.0;

  for (size_t i = 0; i < state->enemies_count; i++) {
    Enemy *enemy = &state->enemies[i];

    float distance = vector_length(vector_from_to(enemy, &state->player));

    if (distance > 650.0f) {

      // spawn enemies at radius of circle
      Vector norm_vector = (Vector){1.0, 0.0};
      // [0; 2*PI)
      float random_angle = ((float)rand() / (float)RAND_MAX) * 3.14159 * 2;
      Vector vector_radius =
          vector_multiply(vector_rotate(norm_vector, random_angle), radius);

      enemy->position = vector_add(state->player.position, vector_radius);
    }
  }
}

void increase_dificilty(GlobalState *state) {
  // Each 7 second more by 5% to enemy spawn
  if (state->frame_counter % (60 * 7) == 0) {
    state->enemy_factor *= 1.05;
  }
}

void make_step(GlobalState *state, Input input, Game *game) {
  TIME(decrease_frame_counters(state), dec_frame);
  spawn_enemies(state, game);
  TIME(process_input(state, input), proc_input);
  TIME(create_projectiles(state, game), create_proj);
  TIME(update_projectile_positions(state), update_proj);
  TIME(damage_and_kill_enemies(state, game), dmg_kill);
  TIME(update_enemies_positions(state), upd_enemy);
  TIME(damage_player(state), dmg_player);
  update_animations(state);
  increase_dificilty(state);
  collect_crystals(state, game);
  teleport_enemies(state, game);

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

  return;
}