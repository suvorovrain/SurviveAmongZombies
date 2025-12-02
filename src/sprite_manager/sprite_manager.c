#include "../state.h"
#include "../units/units.h"

typedef struct SpriteDescription {
  char *path;
  size_t width;
  size_t height;
  size_t count;
  float scale;
} SpriteDescription;

SpriteSheet spritesheets[SPRITE_COUNT];
SpriteDescription sprite_descriptions[SPRITE_COUNT];

static void sm_load_descriptions() {
  sprite_descriptions[SPRITE_GOBLIN] =
      (SpriteDescription){.path = "assets/units/goblin.png",
                          .height = 16,
                          .width = 16,
                          .count = 8,
                          .scale = SCALE};
  sprite_descriptions[SPRITE_SLIME] =
      (SpriteDescription){.path = "assets/units/slime.png",
                          .height = 13,
                          .width = 16,
                          .count = 8,
                          .scale = SCALE};
  sprite_descriptions[SPRITE_IMP] =
      (SpriteDescription){.path = "assets/units/goblin.png",
                          .height = 15,
                          .width = 16,
                          .count = 8,
                          .scale = SCALE};
  sprite_descriptions[SPRITE_PLAYER] =
      (SpriteDescription){.path = "assets/units/player.png",
                          .height = 15,
                          .width = 16,
                          .count = 16,
                          .scale = SCALE};
  sprite_descriptions[SPRITE_PROJECTILE] =
      (SpriteDescription){.path = "assets/projectiles/blue.png",
                          .height = 8,
                          .width = 8,
                          .count = 1,
                          .scale = SCALE * 0.5f};
  sprite_descriptions[SPRITE_PROJECTILE_EXPLODE] =
      (SpriteDescription){.path = "assets/particles/06.png",
                          .height = 32,
                          .width = 32,
                          .count = 12,
                          .scale = SCALE * 0.5f};
}

static void sm_load_sprite(SpriteType type) {
  char *path = sprite_descriptions[type].path;
  size_t height = sprite_descriptions[type].height;
  size_t width = sprite_descriptions[type].width;
  size_t count = sprite_descriptions[type].count;
  float scale = sprite_descriptions[type].scale;
  Sprite *sprites = load_spritesheet_frames(path, width, height, count, scale);

  if (sprites == NULL || sprites->pixels == NULL) {
    fprintf(stderr, "Error when import %s\n", path);
    exit(-1);
  }

  spritesheets[type] = (SpriteSheet){.frames = sprites, .frames_count = count};
}

void sm_init() {
  sm_load_descriptions();

  for (size_t i = 0; i < SPRITE_COUNT; i++) {
    sm_load_sprite(i);
  }

  return;
}

SpriteSheet sm_get_spritesheet(SpriteType type) { return spritesheets[type]; }

void sm_free() {
  for (size_t i = 0; i < SPRITE_COUNT; i++) {
    free_sprites(spritesheets[i].frames, spritesheets[i].frames_count);
  }
}