#include "units.h"

ExpCrystal exp_crystal_create(Vector pos) {
  ExpCrystal result = {0};

  result.type = UNIT_EXP_CRYSTAL;
  result.position = pos;
  result.movement = (Vector){0.0f, 0.0f};
  result.spritesheet = sm_get_spritesheet(SPRITE_EXP_CRYSTAL);
  result.current_sprite = result.spritesheet.frames[0];

  return result;
}

void exp_crystal_free() { return; }