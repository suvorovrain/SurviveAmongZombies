#include "sprite_manager.h"
#include "../state.h"
#include "../units/units.h"
#include <engine/types.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct SpriteDescription {
  char *path;
  int width;
  int height;
  int count;
  float scale;
} SpriteDescription;

SpriteSheet spritesheets[SPRITE_COUNT];
SpriteDescription sprite_descriptions[SPRITE_COUNT];

static void sm_load_descriptions(void) {
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
      (SpriteDescription){.path = "assets/units/imp.png",
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
                          .scale = SCALE * 0.5F};
  sprite_descriptions[SPRITE_PROJECTILE_EXPLODE] =
      (SpriteDescription){.path = "assets/particles/06.png",
                          .height = 32,
                          .width = 32,
                          .count = 12,
                          .scale = SCALE * 0.5F};
  sprite_descriptions[SPRITE_LEVEL_MENU] =
      (SpriteDescription){.path = "assets/level_menu.png",
                          .height = 90,
                          .width = 90,
                          .count = 1,
                          .scale = 5.5F};
  sprite_descriptions[SPRITE_BLUE_CRYSTAL] =
      (SpriteDescription){.path = "assets/crystals/blue.png",
                          .height = 64,
                          .width = 64,
                          .count = 4,
                          .scale = SCALE * 0.20F};
  sprite_descriptions[SPRITE_GREEN_CRYSTAL] =
      (SpriteDescription){.path = "assets/crystals/green.png",
                          .height = 64,
                          .width = 64,
                          .count = 4,
                          .scale = SCALE * 0.20F};
  sprite_descriptions[SPRITE_STATIC_BUSH] =
      (SpriteDescription){.path = "assets/static/bush_red.png",
                          .height = 10,
                          .width = 10,
                          .count = 1,
                          .scale = SCALE};
  sprite_descriptions[SPRITE_STATIC_CACTUS_LONG] =
      (SpriteDescription){.path = "assets/static/cactus_long.png",
                          .height = 15,
                          .width = 10,
                          .count = 1,
                          .scale = SCALE};
  sprite_descriptions[SPRITE_STATIC_CACTUS_SHORT] =
      (SpriteDescription){.path = "assets/static/cactus_short.png",
                          .height = 13,
                          .width = 12,
                          .count = 1,
                          .scale = SCALE};
  sprite_descriptions[SPRITE_STATIC_ROCK] =
      (SpriteDescription){.path = "assets/static/rock.png",
                          .height = 11,
                          .width = 10,
                          .count = 1,
                          .scale = SCALE};
}

static void sm_load_sprite(SpriteType type) {
  char *path = sprite_descriptions[type].path;
  int height = sprite_descriptions[type].height;
  int width = sprite_descriptions[type].width;
  int count = sprite_descriptions[type].count;
  float scale = sprite_descriptions[type].scale;
  Sprite *sprites = load_spritesheet_frames(path, width, height, count, scale);

  if (sprites == NULL || sprites->pixels == NULL) {
    fprintf(stderr, "Error when import %s\n", path);
    exit(-1);
  }

  spritesheets[type] = (SpriteSheet){.frames = sprites, .frames_count = count};
}

void sm_init(void) {
  sm_load_descriptions();

  for (size_t i = 0; i < SPRITE_COUNT; i++) {
    sm_load_sprite(i);
  }
}

Sprite sm_rotate_sprite(Sprite sprite, double angle) {
  if (sprite.pixels == NULL) {
    fprintf(stderr, "Pixels is NULL\n");
    exit(-1);
  }

  printf("angle %lf\n", angle);

  Sprite new_sprite;
  new_sprite.pixels =
      calloc((unsigned long)sprite.width * sprite.height, sizeof(uint32_t));
  new_sprite.width = sprite.width;
  new_sprite.height = sprite.height;

  Vector centre =
      (Vector){(float)sprite.width / 2.0F, (float)sprite.height / 2.0F};
  for (size_t x_input = 0; x_input < sprite.width; x_input++) {
    for (size_t y_input = 0; y_input < sprite.height; y_input++) {
      Vector vec = vector_sub((Vector){(float)x_input, (float)y_input}, centre);
      Vector vec_rotated = vector_rotate(vec, -(float)angle);
      Vector new_pos = vector_add(centre, vec_rotated);

      ssize_t x_out = (ssize_t)new_pos.x;
      x_out = x_out < 0 ? 0 : x_out;
      x_out = x_out >= sprite.width ? sprite.width - 1 : x_out;
      ssize_t y_out = (ssize_t)new_pos.y;
      y_out = y_out < 0 ? 0 : y_out;
      y_out = y_out >= sprite.height ? sprite.height - 1 : y_out;

      // printf("x_out: %ld y_out: %ld\n", x_out, y_out);

      new_sprite.pixels[y_input * sprite.width + x_input] =
          sprite.pixels[y_out * sprite.width + x_out];
    }
  }

  return new_sprite;
}

SpriteSheet sm_get_spritesheet(SpriteType type) { return spritesheets[type]; }
Sprite sm_get_sprite(SpriteType type) { return spritesheets[type].frames[0]; }
Sprite sm_get_rotated_sprite(SpriteType type, double angle) {
  return sm_rotate_sprite(spritesheets[type].frames[0], angle);
}
Sprite *sm_get_sprite_pointer(SpriteType type) {
  return &spritesheets[type].frames[0];
}

void sm_free(void) {
  for (size_t i = 0; i < SPRITE_COUNT; i++) {
    free_sprites(spritesheets[i].frames, spritesheets[i].frames_count);
  }
}
