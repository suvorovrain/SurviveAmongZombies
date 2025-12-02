#include "game.h"
#include "state/state.h"
#include "static_objs.h"
#include "units/units.h"
#include <engine/coordinates.h>
#include <engine/engine.h>
#include <engine/input.h>
#include <engine/map.h>
#include <stb_ds.h>
#include <stdlib.h>

#define MAP_WIDTH 150
#define MAP_HEIGHT 150
#define STATIC_OBJ_COUNT 100

void game_free(Game *game);

static UIElement hp_bar(Game *game) {
  UIElement bar = {0};
  bar.mode = UI_POS_ATTACHED;
  bar.position.attached.object = game->player;
  bar.position.attached.offset =
      (Vector){-3 * SCALE, game->player->cur_sprite->height + 2.0 * SCALE};
  bar.z_index = 0;
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite = load_sprite("assets/hp_bar.png", 1.0f);
  bar.sprite = sprite;
  return bar;
}

static void update_hp_bar(UIElement *hp_bar, Game *game) {
  float percent = 100.0f;
  if (game->state.player.stat_hp < 0) {
    percent = 0.0f;
  } else {
    percent = game->state.player.stat_hp / game->state.player.stat_max_hp;
  }

  // printf("PERCENT %f\n", percent);

  float size = 62;
  int32_t width_pixels = (int32_t)(size * percent);

  for (uint32_t x = 1; x <= width_pixels; x++) {
    for (uint32_t y = 1; y <= 6; y++) {
      *(hp_bar->sprite->pixels + y * 64 + x) = 0xFF00FF00;
    }
  }

  for (uint32_t x = width_pixels; x <= 63; x++) {
    for (uint32_t y = 1; y <= 6; y++) {
      *(hp_bar->sprite->pixels + y * 64 + x) = 0xFFFF0000;
    }
  }
}

const float EXP_BAR_SCALE = 3.0f;

static UIElement exp_bar(Game *game) {
  UIElement bar = {0};
  bar.mode = UI_POS_SCREEN;
  bar.position.screen = (Vector){10.0f, 10.0f};
  bar.z_index = 0;
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite = load_sprite("assets/xp_bar.png", EXP_BAR_SCALE);
  bar.sprite = sprite;
  return bar;
}

static void update_exp_bar(UIElement *exp_bar, Game *game) {
  float percent = 100.0f;
  if (game->state.player.stat_experience == 0) {
    percent = 0.0f;
  } else {
    percent = game->state.player.stat_experience / 1000.0;
  }

  // printf("PERCENT EXP %f\n", percent);

  float size = 36 * EXP_BAR_SCALE;
  int32_t width_pixels = (int32_t)(size * percent);

  uint32_t x_left = (uint32_t)(29.0 * EXP_BAR_SCALE);
  uint32_t x_right = (uint32_t)(65 * EXP_BAR_SCALE - 1);
  uint32_t y_top = (uint32_t)EXP_BAR_SCALE;
  uint32_t y_down = (uint32_t)(9 * EXP_BAR_SCALE - 1);

  for (uint32_t x = x_left; x <= x_left + width_pixels; x++) {
    for (uint32_t y = y_top; y <= y_down; y++) {
      *(exp_bar->sprite->pixels + y * (66 * (uint32_t)EXP_BAR_SCALE) + x) =
          0xFF4D6DF3;
    }
  }

  for (uint32_t x = x_left + width_pixels; x <= x_right; x++) {
    for (uint32_t y = y_top; y <= y_down; y++) {
      *(exp_bar->sprite->pixels + y * (66 * (uint32_t)EXP_BAR_SCALE) + x) =
          0xFFFFFFFF;
    }
  }
}

static UIElement dead_background(Game *game) {
  UIElement bar = {0};
  bar.position.screen = (Vector){0.0, 0.0};
  bar.mode = UI_POS_SCREEN;
  bar.z_index = 0;
  Sprite *sprite = malloc(sizeof(Sprite));
  uint32_t *pixels = calloc((size_t)(800 * 600), sizeof(uint32_t));
  for (size_t i = 0; i < 800; i++) {
    for (size_t j = 0; j < 600; j++) {
      pixels[j * 800 + i] = 0xCC000000;
    }
  }
  sprite->pixels = pixels;
  sprite->width = 800;
  sprite->height = 600;

  bar.sprite = sprite;
  return bar;
}

static UIElement dead_screen(Game *game) {
  UIElement bar = {0};
  bar.mode = UI_POS_SCREEN;
  bar.z_index = 0;
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite = load_sprite("assets/end_screen.png", 10.0f);
  bar.sprite = sprite;
  Vector pos = {800.0, 600.0};
  pos = vector_div(pos, 2.0);
  pos = vector_sub(
      pos, vector_div((Vector){bar.sprite->width, bar.sprite->height}, 2.0));
  bar.position.screen = pos;
  return bar;
}

static UIElement level_ui(Game *game) {
  UIElement ui = {0};
  ui.mode = UI_POS_SCREEN;
  ui.position.screen = (Vector){10.0f, 50.0f};
  ui.z_index = 0;
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite = text_sprite("Level: not initialized", game->fonts[0],
                        (SDL_Color){255, 255, 255, 255});
  ui.sprite = sprite;
  return ui;
}

static void update_level_ui(UIElement *ui, Game *game) {
  char text[100];
  snprintf(text, 100, "Level: %d", (int32_t)game->state.player.stat_level);
  free_sprite(ui->sprite);
  *ui->sprite =
      text_sprite(text, game->fonts[0], (SDL_Color){255, 255, 255, 255});
}

static UIElement kills_ui(Game *game) {
  UIElement ui = {0};
  ui.mode = UI_POS_SCREEN;
  ui.position.screen = (Vector){10.0f, 80.0f};
  ui.z_index = 0;
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite = text_sprite("Kills: not initialized", game->fonts[0],
                        (SDL_Color){255, 255, 255, 255});
  ui.sprite = sprite;
  return ui;
}

static void update_kills_ui(UIElement *ui, Game *game) {
  char text[100];
  snprintf(text, 100, "Kills: %ld", (int64_t)game->state.kills);
  free_sprite(ui->sprite);
  *ui->sprite =
      text_sprite(text, game->fonts[0], (SDL_Color){255, 255, 255, 255});
}

static UIElement time_ui(Game *game) {
  UIElement ui = {0};
  ui.mode = UI_POS_SCREEN;
  ui.position.screen = (Vector){10.0f, 110.0f};
  ui.z_index = 0;
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite = text_sprite("Time: not initialized", game->fonts[0],
                        (SDL_Color){255, 255, 255, 255});
  ui.sprite = sprite;
  return ui;
}

static void update_time_ui(UIElement *ui, Game *game) {
  char text[100];
  uint32_t seconds = game->state.frame_counter / 60 % 60;
  uint32_t mins = game->state.frame_counter / 60 / 60;

  snprintf(text, 100, "Time: %1d%1d:%1d%1d", mins / 10, mins % 10, seconds / 10,
           seconds % 10);
  free_sprite(ui->sprite);
  *ui->sprite =
      text_sprite(text, game->fonts[0], (SDL_Color){255, 255, 255, 255});
}

GameObject **get_game_objects_from_state(GlobalState *state) {
  GameObject **objects =
      calloc(sizeof(GameObject *),
             state->enemies_count + state->projectiles_count + 1);

  GameObject *player = calloc(sizeof(GameObject), 1);
  player->position = state->player.position;
  player->cur_sprite = &state->player.spritesheet.frames[0];

  objects[0] = player;

  for (size_t i = 0; i < state->enemies_count; i++) {
    GameObject *enemy = calloc(sizeof(GameObject), 1);
    enemy->position = state->enemies[i].position;
    enemy->cur_sprite = &state->enemies[i].spritesheet.frames[0];

    objects[i + 1] = enemy;
  }

  for (size_t i = 0; i < state->projectiles_count; i++) {
    GameObject *projectile = calloc(sizeof(GameObject), 1);
    projectile->position = state->projectiles[i].position;
    projectile->cur_sprite = &state->projectiles[i].spritesheet.frames[0];

    objects[i + state->enemies_count + 1] = projectile;
  }

  return objects;
}

Game *game_create() {
  Game *game = calloc(1, sizeof(Game));
  if (!game) {
    return NULL;
  }

  Engine *engine = engine_create(800, 600, "Survie Among Zombies");
  game->engine = engine;
  if (!engine) {
    game_free(game);
    return NULL;
  }

  TilesInfo ti = {0};
  ti.tile_sprites = calloc(1, sizeof(Sprite));
  ti.tile_sprites[0] = load_sprite("assets/static/grass.png", 4.0f);
  printf("%p\n", ti.tile_sprites[0]);
  ti.sprite_count = 1;
  ti.tiles = calloc(MAP_WIDTH * MAP_HEIGHT, sizeof(uint32_t));
  ti.sides_height = 32;

  Map *map = map_create(MAP_WIDTH, MAP_HEIGHT, ti);
  if (!map) {
    game_free(game);
    map_free(map);
    return NULL;
  };
  engine_set_map(engine, map);

  // Load fonts
  if (TTF_Init() != 0) {
    fprintf(stderr, "Failed to initialize TTF: %s\n", TTF_GetError());
    game_free(game);
    return NULL;
  }
  game->fonts = calloc(sizeof(TTF_Font *), FONT_COUNT);
  game->fonts[FONT_DEJAVU] = TTF_OpenFont("fonts/DejaVuSans.ttf", 20);
  game->fonts[FONT_PIXELOID_MONO] = TTF_OpenFont("fonts/PixeloidMono.ttf", 20);
  game->fonts[FONT_PIXELOID_SANS] = TTF_OpenFont("fonts/PixeloidSans.ttf", 20);
  game->fonts[FONT_PIXELOID_SANS_BOLD] =
      TTF_OpenFont("fonts/PixeloidSansBold.ttf", 20);

  if (!game->fonts[FONT_DEJAVU] || !game->fonts[FONT_PIXELOID_MONO] ||
      !game->fonts[FONT_PIXELOID_SANS] ||
      !game->fonts[FONT_PIXELOID_SANS_BOLD]) {
    game_free(game);
    return NULL;
  }

  GlobalState globalState = init_global_state(map);
  GameObject **objects = get_game_objects_from_state(&globalState);

  engine_set_player(engine, objects[0]);
  game->player = objects[0];

  game->ui_count = 7;
  game->uis = calloc(sizeof(UIElement), game->ui_count);
  game->uis[0] = hp_bar(game);
  game->uis[1] = exp_bar(game);
  game->uis[2] = level_ui(game);
  game->uis[3] = kills_ui(game);
  game->uis[4] = time_ui(game);
  game->uis[5] = dead_background(game);
  game->uis[6] = dead_screen(game);

  UIElement **batch_ui = calloc(sizeof(UIElement *), game->ui_count);
  batch_ui[0] = &game->uis[0];
  batch_ui[1] = &game->uis[1];
  batch_ui[2] = &game->uis[2];
  batch_ui[3] = &game->uis[3];
  batch_ui[4] = &game->uis[4];
  batch_ui[5] = &game->uis[5];
  batch_ui[6] = &game->uis[6];
  game->batch = (RenderBatch){.obj_count = 1 + game->state.enemies_count +
                                           game->state.enemies_count,
                              .objs = objects,
                              .ui_count = 5,
                              .uis = batch_ui};
  game->engine = engine;
  game->map = map;
  game->state = globalState;

  return game;
}

void game_free(Game *game) {
  if (!game)
    return;

  free(game);
}

void game_update(Game *game, Input *input) {
  if (!game || !input)
    return;

  if (game->state.status == GAME_DEAD) {
    game->batch.ui_count = 7;

    return;
  }

  make_step(&game->state, *input, game);
  GameObject **objects = get_game_objects_from_state(&(game->state));
  update_hp_bar(&(game->uis[0]), game);
  update_exp_bar(&(game->uis[1]), game);
  update_level_ui(&(game->uis[2]), game);
  update_kills_ui(&(game->uis[3]), game);
  update_time_ui(&(game->uis[4]), game);

  game->uis[0].position.attached.object = objects[0];
  game->batch.obj_count =
      1 + game->state.enemies_count + game->state.projectiles_count;
  game->batch.objs = objects;

  engine_set_player(game->engine, game->batch.objs[0]);
}
