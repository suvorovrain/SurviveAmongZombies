#include "units.h"

Projectile projectile_create(Player player, Vector movement) {
  Projectile result = {0};

  result.type = UNIT_PROJECTILE;
  result.position = player.position;
  result.stat_damage = player.stat_damage;
  result.stat_movespeed = 5.0;
  result.movement = movement;

  Sprite *frames = load_spritesheet_frames(
      "../../assets/projectiles/PNGs/medium/blue/medium blue1.png", 16, 16, 1,
      1.0);
  result.spritesheet = (SpriteSheet){.frames = frames, .frames_count = 1};

  return result;
}

void projectile_free() { return; }