#include "units.h"

Projectile projectile_create(Player player, Vector movement) {
  Projectile result = {0};

  result.type = UNIT_PROJECTILE;
  result.position = player.position;
  result.stat_damage = player.stat_damage;
  result.stat_movespeed = 5.0;
  result.movement = movement;

  return result;
}

void projectile_free() { return; }