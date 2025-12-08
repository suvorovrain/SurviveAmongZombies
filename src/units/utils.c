#include "units.h"

// get position from generic object
static Vector unit_get_position(void *unit) {
  UnitType type = ((Player *)unit)->type;

  switch (type) {
  case UNIT_PLAYER:
    return ((Player *)unit)->position;
    break;
  case UNIT_ENEMY:
    return ((Enemy *)unit)->position;
    break;
  case UNIT_PROJECTILE:
    return ((Projectile *)unit)->position;
    break;
  case UNIT_EXP_CRYSTAL:
    return ((Crystal *)unit)->position;
    break;
  default:
    fprintf(stderr, "Wrong unit type\n");
    exit(-1);
    break;
  }
}

// get position from generic object
static Sprite unit_get_sprite(void *unit) {
  UnitType type = ((Player *)unit)->type;

  switch (type) {
  case UNIT_PLAYER:
    return ((Player *)unit)->current_sprite;
    break;
  case UNIT_ENEMY:
    return ((Enemy *)unit)->current_sprite;
    break;
  case UNIT_PROJECTILE:
    return ((Projectile *)unit)->current_sprite;
    break;
  case UNIT_EXP_CRYSTAL:
    return ((Crystal *)unit)->current_sprite;
    break;
  default:
    fprintf(stderr, "Wrong unit type\n");
    exit(-1);
    break;
  }
}

Vector unit_get_size(void *unit) {
  Sprite sprite = unit_get_sprite(unit);
  Vector size = {sprite.width, sprite.height};

  return size;
}

Homka_Rect unit_get_rect(void *unit) {
  Vector pos = unit_get_position(unit);
  Vector size = unit_get_size(unit);

  return (Homka_Rect){.left = pos.x,
                      .right = pos.x + size.x,
                      .top = pos.y,
                      .down = pos.y + size.y};
}

Vector unit_get_centre(void *unit) {
  Vector pos = unit_get_position(unit);
  Vector size = unit_get_size(unit);

  return vector_add(pos, vector_multiply(size, 0.5));
}

Vector vector_multiply(Vector vector, float num) {
  return (Vector){vector.x * num, vector.y * num};
}

Vector vector_div(Vector vector, float num) {
  return (Vector){vector.x / num, vector.y / num};
}

Vector vector_add(Vector first, Vector second) {
  return (Vector){first.x + second.x, first.y + second.y};
}

Vector vector_sub(Vector first, Vector second) {
  return (Vector){first.x - second.x, first.y - second.y};
}

Vector vector_rotate(Vector vector, float angle) {
  float x = vector.x;
  float y = vector.y;

  return (Vector){x * cosf(angle) - y * sinf(angle),
                  x * sinf(angle) + y * cosf(angle)};
}

Vector vector_from_to(void *first, void *second) {
  return vector_sub(unit_get_position(second), unit_get_position(first));
}

float vector_length(Vector vector) {
  return sqrtl(vector.x * vector.x + vector.y * vector.y);
}

float units_distance_between(void *first, void *second) {
  return vector_length(vector_from_to(first, second));
}

Vector vector_normalize(Vector vector) {
  return vector_div(vector, vector_length(vector));
}

bool units_intersect(void *first, void *second, float padding) {

  Homka_Rect rect1 = unit_get_rect(first);
  rect1.left += padding * SCALE;
  rect1.right -= padding * SCALE;
  rect1.top += padding * SCALE;
  rect1.down -= padding * SCALE;

  Homka_Rect rect2 = unit_get_rect(second);
  rect2.left += padding * SCALE;
  rect2.right -= padding * SCALE;
  rect2.top += padding * SCALE;
  rect2.down -= padding * SCALE;

  return !(rect1.right <= rect2.left || rect1.left >= rect2.right ||
           rect1.top >= rect2.down || rect1.down <= rect2.top);
}