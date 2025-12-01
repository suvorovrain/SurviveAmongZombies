#include "units.h"

Projectile projectile_create(Player player, Vector movement) {
  Projectile result = {0};

  const int size = 8;

  result.type = UNIT_PROJECTILE;
  Vector player_center = unit_get_centre(&player);
  Vector proj_size = (Vector){.x = size * SCALE, .y = size * SCALE};
  result.position = vector_add(player_center, vector_multiply(proj_size, -0.5));
  result.stat_damage = player.stat_damage;
  result.stat_movespeed = 5.0;
  result.movement = movement;
  result.kills = 0;
  result.live_frames_last = 120;

  Sprite *frames = load_spritesheet_frames("assets/projectiles/blue.png", size,
                                           size, 1, SCALE * 0.5);
  result.spritesheet = (SpriteSheet){.frames = frames, .frames_count = 1};

  Sprite *explode_frames = load_spritesheet_frames(
      "assets/particles/homka_take.png", 32, 32, 12, SCALE * 0.5);
  result.explode_spritesheet =
      (SpriteSheet){.frames = explode_frames, .frames_count = 12};

  return result;
}

void projectile_free() { return; }