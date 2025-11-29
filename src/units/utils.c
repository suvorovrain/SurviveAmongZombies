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
  default:
    fprintf(stderr, "Wrong unit type\n");
    exit(-1);
    break;
  }
}

// get position from generic object
static SpriteSheet unit_get_spritesheet(void *unit) {
  UnitType type = ((Player *)unit)->type;

  switch (type) {
  case UNIT_PLAYER:
    return ((Player *)unit)->spritesheet;
    break;
  case UNIT_ENEMY:
    return ((Enemy *)unit)->spritesheet;
    break;
  case UNIT_PROJECTILE:
    return ((Projectile *)unit)->spritesheet;
    break;
  default:
    fprintf(stderr, "Wrong unit type\n");
    exit(-1);
    break;
  }
}

Vector unit_get_size(void *unit) {
  UnitType type = ((Player *)unit)->type;

  SpriteSheet spritesheet = unit_get_spritesheet(unit);
  Vector size = {spritesheet.frames->width, spritesheet.frames->width};

  return size;
}

Rect unit_get_rect(void *unit) {
  Vector pos = unit_get_position(unit);
  Vector size = unit_get_size(unit);

  return (Rect){.left = pos.x,
                .right = pos.x + size.x,
                .top = pos.y,
                .down = pos.y + size.y};
}

Vector unit_get_centre(void *unit) {
  Vector pos = unit_get_position(unit);
  Vector size = unit_get_size(unit);

  Vector diag = vector_add(pos, size);

  return vector_div(vector_sub(diag, pos), 0.5);
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

bool units_intersect(void *first, void *second) {

  Rect rect1 = unit_get_rect(first);
  Rect rect2 = unit_get_rect(second);

  return (rect1.left < rect2.right && rect1.right > rect2.left &&
          rect1.top > rect2.down && rect1.down < rect2.top);
}