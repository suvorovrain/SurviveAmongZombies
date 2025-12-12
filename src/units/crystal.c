#include "../sprite_manager/sprite_manager.h"
#include "../state.h"
#include "engine/types.h"
#include "units.h"

Crystal crystal_create(Vector pos, CrystalType type_crystal) {
  Crystal result = {0};

  result.type = UNIT_EXP_CRYSTAL;
  result.type_crystal = type_crystal;
  result.position = pos;
  result.movement = (Vector){0.0F, 0.0F};
  result.spritesheet =
      sm_get_spritesheet(type_crystal == CRYSTAL_BLUE ? SPRITE_BLUE_CRYSTAL
                                                      : SPRITE_GREEN_CRYSTAL);
  result.current_sprite = result.spritesheet.frames[0];

  result.is_collectable = false;
  result.collected_frame = 0;
  result.collected_angle = (Vector){0.0F, 0.0F};
  result.collected_position = (Vector){0.0F, 0.0F};

  return result;
}

void exp_crystal_free(void) {}
