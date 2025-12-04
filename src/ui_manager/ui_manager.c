#include "ui_manager.h"
#include "../game.h"
#include "../state.h"
#include "../units/units.h"
#include "engine/types.h"
#include <stdbool.h>

UIElement uis[UI_COUNT];
UIElement *uis_pointers[UI_COUNT];
size_t uis_count = 0;
bool enable_flags[UI_COUNT];

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

  hp_bar->position.attached.object = game->batch.objs[0];
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
  } else if (game->state.player.stat_experience >=
             game->state.player.stat_experience_for_lvlup) {
    percent = 1.0f;
  } else {
    percent = game->state.player.stat_experience /
              game->state.player.stat_experience_for_lvlup;
  }

  // printf("PERCENT EXP %f\n", percent);

  float size = 36 * EXP_BAR_SCALE;
  int32_t width_pixels = (int32_t)(size * percent);

  uint32_t x_left = (uint32_t)(29.0 * EXP_BAR_SCALE);
  uint32_t x_right = (uint32_t)(65 * EXP_BAR_SCALE - 1);
  uint32_t y_top = (uint32_t)EXP_BAR_SCALE;
  uint32_t y_down = (uint32_t)(9 * EXP_BAR_SCALE - 1);

  for (uint32_t x = x_left; x <= x_left + width_pixels || x <= x_right; x++) {
    for (uint32_t y = y_top; y <= y_down; y++) {
      *(exp_bar->sprite->pixels + y * exp_bar->sprite->width + x) = 0xFF4D6DF3;
    }
  }

  for (uint32_t x = x_left + width_pixels; x <= x_right; x++) {
    for (uint32_t y = y_top; y <= y_down; y++) {
      *(exp_bar->sprite->pixels + y * exp_bar->sprite->width + x) = 0xFFFFFFFF;
    }
  }
}

static UIElement dead_background(Game *game) {
  UIElement bar = {0};
  bar.position.screen = (Vector){0.0, 0.0};
  bar.mode = UI_POS_SCREEN;
  bar.z_index = 2;
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
  bar.z_index = 3;
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
  *sprite =
      text_sprite("Level: not initialized", game->fonts[FONT_PIXELOID_SANS],
                  (SDL_Color){255, 255, 255, 255});
  ui.sprite = sprite;
  return ui;
}

static void update_level_ui(UIElement *ui, Game *game) {
  char text[100];
  snprintf(text, 100, "Level: %d", (int32_t)game->state.player.stat_level);
  free_sprite(ui->sprite);
  *ui->sprite = text_sprite(text, game->fonts[FONT_PIXELOID_SANS],
                            (SDL_Color){255, 255, 255, 255});
}

static UIElement kills_ui(Game *game) {
  UIElement ui = {0};
  ui.mode = UI_POS_SCREEN;
  ui.position.screen = (Vector){10.0f, 75.0f};
  ui.z_index = 0;
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite =
      text_sprite("Kills: not initialized", game->fonts[FONT_PIXELOID_SANS],
                  (SDL_Color){255, 255, 255, 255});
  ui.sprite = sprite;
  return ui;
}

static void update_kills_ui(UIElement *ui, Game *game) {
  char text[100];
  snprintf(text, 100, "Kills: %ld", (int64_t)game->state.kills);
  free_sprite(ui->sprite);
  *ui->sprite = text_sprite(text, game->fonts[FONT_PIXELOID_SANS],
                            (SDL_Color){255, 255, 255, 255});
}

static UIElement time_ui(Game *game) {
  UIElement ui = {0};
  ui.mode = UI_POS_SCREEN;
  ui.position.screen = (Vector){10.0f, 100.0f};
  ui.z_index = 0;
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite =
      text_sprite("Time: not initialized", game->fonts[FONT_PIXELOID_SANS],
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
  *ui->sprite = text_sprite(text, game->fonts[FONT_PIXELOID_SANS],
                            (SDL_Color){255, 255, 255, 255});
}

static UIElement pause_text_ui(Game *game) {
  UIElement ui = {0};
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite = text_sprite("PAUSE", game->fonts[FONT_PIXELOID_SANS_BOLD],
                        (SDL_Color){0x4D, 0x6D, 0xF3, 0xFF});
  ui.sprite = sprite;

  ui.mode = UI_POS_SCREEN;
  Vector pos = {800.0, 600.0};
  pos = vector_div(pos, 2.0);
  pos = vector_sub(
      pos, vector_div((Vector){ui.sprite->width, ui.sprite->height}, 2.0));
  ui.position.screen = pos;

  ui.z_index = 3;

  return ui;
}

static UIElement level_menu_ui(Game *game) {
  UIElement ui = {0};
  ui.sprite = sm_get_sprite_pointer(SPRITE_LEVEL_MENU);

  ui.mode = UI_POS_SCREEN;
  Vector pos = {800.0, 600.0};
  pos = vector_div(pos, 2.0);
  pos = vector_sub(
      pos, vector_div((Vector){ui.sprite->width, ui.sprite->height}, 2.0));
  ui.position.screen = pos;

  return ui;
}

static UIElement level_menu_first_ui(Game *game) {
  UIElement ui = {0};
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite =
      text_sprite("12345678901234567890", game->fonts[FONT_LEVEL_MENU_OPTION],
                  (SDL_Color){0x4D, 0x6D, 0xF3, 0xFF});
  ui.sprite = sprite;

  ui.mode = UI_POS_SCREEN;
  Vector pos = {280.0, 180.0};

  ui.position.screen = pos;

  ui.z_index = 3;

  return ui;
}

static void update_level_menu_option(UIElement *ui, Game *game,
                                     LevelUpStat stat) {
  char text[100];

  switch (stat) {
  case LVLUP_ATK_SPD:
    snprintf(text, 100, "+%.0f%% Increased ATK speed", lvlup_atk_speed_percent);
    break;
  case LVLUP_PROJ_COUNT:
    snprintf(text, 100, "+%.1f Projectile count", lvlup_proj_count);
    break;
  case LVLUP_PIERCE:
    snprintf(text, 100, "+%.1f Projectile pierce", lvlup_pierce_count);
    break;
  case LVLUP_MOVEMENT:
    snprintf(text, 100, "+%.0f%% Increased movement speed",
             lvlup_movement_percent);
    break;
  case LVLUP_EXP:
    snprintf(text, 100, "+%.0f%% Increased EXP", lvlup_exp_percent);
    break;
  case LVLUP_MAXHP:
    snprintf(text, 100, "+%.0f Max HP", lvlup_maxhp_count);
    break;
  case LVLUP_DMG:
    snprintf(text, 100, "+%.0f%% Increased DMG", lvlup_dmg_percent);
    break;
  default:
    break;
  }

  free_sprite(ui->sprite);
  *ui->sprite = text_sprite(text, game->fonts[FONT_LEVEL_MENU_OPTION],
                            (SDL_Color){0x4D, 0x6D, 0xF3, 0xFF});
}

static UIElement level_menu_second_ui(Game *game) {
  UIElement ui = {0};
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite = text_sprite("10% increased attack speed",
                        game->fonts[FONT_LEVEL_MENU_OPTION],
                        (SDL_Color){0x4D, 0x6D, 0xF3, 0xFF});
  ui.sprite = sprite;

  ui.mode = UI_POS_SCREEN;
  Vector pos = {280.0, 295.0};

  ui.position.screen = pos;

  ui.z_index = 3;

  return ui;
}

static UIElement level_menu_third_ui(Game *game) {
  UIElement ui = {0};
  Sprite *sprite = malloc(sizeof(Sprite));
  *sprite =
      text_sprite("+1 projectile count", game->fonts[FONT_LEVEL_MENU_OPTION],
                  (SDL_Color){0x4D, 0x6D, 0xF3, 0xFF});
  ui.sprite = sprite;

  ui.mode = UI_POS_SCREEN;
  Vector pos = {280.0, 410.0};

  ui.position.screen = pos;

  ui.z_index = 3;

  return ui;
}

void um_ui_init(Game *game) {
  uis[UI_HP_BAR] = hp_bar(game);
  uis[UI_EXP_BAR] = exp_bar(game);
  uis[UI_LEVEL] = level_ui(game);
  uis[UI_KILLS] = kills_ui(game);
  uis[UI_TIME] = time_ui(game);
  uis[UI_PAUSE] = pause_text_ui(game);
  uis[UI_DEAD_BACKGROUND] = dead_background(game);
  uis[UI_DEAD_SCREEN] = dead_screen(game);
  uis[UI_LEVEL_MENU_FIRST] = level_menu_first_ui(game);
  uis[UI_LEVEL_MENU_SECOND] = level_menu_second_ui(game);
  uis[UI_LEVEL_MENU_THIRD] = level_menu_third_ui(game);
  uis[UI_LEVEL_MENU] = level_menu_ui(game);

  for (size_t i = 0; i < UI_COUNT; i++) {
    enable_flags[i] = true;
    uis_pointers[i] = &uis[i];
  }

  return;
}

void um_ui_update(Game *game) {
  if (enable_flags[UI_HP_BAR])
    update_hp_bar(&uis[UI_HP_BAR], game);
  if (enable_flags[UI_EXP_BAR])
    update_exp_bar(&uis[UI_EXP_BAR], game);
  if (enable_flags[UI_LEVEL])
    update_level_ui(&uis[UI_LEVEL], game);
  if (enable_flags[UI_KILLS])
    update_kills_ui(&uis[UI_KILLS], game);
  if (enable_flags[UI_TIME])
    update_time_ui(&uis[UI_TIME], game);
  if (enable_flags[UI_LEVEL_MENU_FIRST])
    update_level_menu_option(&uis[UI_LEVEL_MENU_FIRST], game,
                             game->level_menu_first);
  if (enable_flags[UI_LEVEL_MENU_SECOND])
    update_level_menu_option(&uis[UI_LEVEL_MENU_SECOND], game,
                             game->level_menu_second);
  if (enable_flags[UI_LEVEL_MENU_THIRD])
    update_level_menu_option(&uis[UI_LEVEL_MENU_THIRD], game,
                             game->level_menu_third);

  return;
}

void um_ui_enable(UIType type) { enable_flags[type] = true; }
void um_ui_disable(UIType type) { enable_flags[type] = false; }

size_t um_ui_get_uis_count() {
  size_t count = 0;

  for (size_t i = 0; i < UI_COUNT; i++) {
    if (enable_flags[i])
      count++;
  }

  return count;
}

UIElement **um_ui_get_uis() {
  size_t index = 0;

  for (size_t i = 0; i < UI_COUNT; i++) {
    if (!enable_flags[i])
      continue;

    uis_pointers[index++] = &uis[i];
  }

  return uis_pointers;
}
