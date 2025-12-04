#include "units.h"

Crystal crystal_create(Vector pos, CrystalType type_crystal) {
  Crystal result = {0};

  result.type = UNIT_EXP_CRYSTAL;
  result.type_crystal = type_crystal;
  result.position = pos;
  result.movement = (Vector){0.0f, 0.0f};
  result.spritesheet =
      sm_get_spritesheet(type_crystal == CRYSTAL_BLUE ? SPRITE_BLUE_CRYSTAL
                                                      : SPRITE_GREEN_CRYSTAL);
  result.current_sprite = result.spritesheet.frames[0];

  return result;
}

void exp_crystal_free() { return; }