#include "static_objs.h"
#include "engine/coordinates.h"
#include "engine/map.h"
#include "engine/random.h"
#include "engine/types.h"
#include "sprite_manager/sprite_manager.h"
#include "stb_ds.h"
#include <stdlib.h>
#include <time.h>

typedef enum {
  OBJ_BUSH = 0,
  OBJ_ROCK,
  OBJ_CACTUS_LONG,
  OBJ_CACTUS_SHORT,
  OBJ_COUNT
} ObjectType;

GameObject *gen_st_objs(Map *map, int count) {
  if (!map)
    return NULL;

  const int margin = 80;

  GameObject *objects = calloc(count, sizeof(GameObject));
  int cur_count = 0;
  while (cur_count < count) {
    SpriteType sprite_type = SPRITE_STATIC_BUSH;
    // Random object type (determine first to know sprite size)
    ObjectType type = rand_big() % OBJ_COUNT;
    switch (type) {
    case OBJ_BUSH:
      sprite_type = SPRITE_STATIC_BUSH;
      break;
    case OBJ_ROCK:
      sprite_type = SPRITE_STATIC_ROCK;
      break;
    case OBJ_CACTUS_LONG:
      sprite_type = SPRITE_STATIC_CACTUS_LONG;
      break;
    case OBJ_CACTUS_SHORT:
      sprite_type = SPRITE_STATIC_CACTUS_SHORT;
      break;
    default:
      exit(-1);
    }

    Sprite *sprite = sm_get_sprite_pointer(sprite_type);

    if (!sprite->pixels)
      continue;

    VectorU32 world = map_gen_random_position(map, margin);
    VectorU32 map_size = map_get_size(map);
    // Check if sprite corners are within map pixel bounds
    if (world.x + sprite->width >= map_size.x ||
        world.y + sprite->height >= map_size.y) {
      continue;
    }

    // Check that the pixel at this position is not transparent (inside diamond)
    // uint32_t center_x = (uint32_t)(world.x + sprite->width / 2);
    // uint32_t center_y = (uint32_t)(world.y + sprite->height);
    // if (center_x < map_size.x && center_y < map_size.y) {
    //   uint32_t pixel =
    //       is_point_within_map(map, (VectorU32){center_x, center_y}, margin);
    //   // Check if pixel is not background (has alpha > 0)
    //   if ((pixel >> 24) == 0) {
    //     continue; // Outside diamond shape
    //   }
    // }

    GameObject obj = {0};
    obj.position = (Vector){(float)world.x, (float)world.y};
    obj.cur_sprite = sprite;
    // obj.data = NULL;
    if (type == OBJ_ROCK || type == OBJ_CACTUS_SHORT ||
        type == OBJ_CACTUS_LONG) {
      obj.data = (void *)true;
    }

    objects[cur_count++] = obj;
  }

  return objects;
}

void free_static_objs(GameObject *st_objs) {
  if (!st_objs)
    return;

  free(st_objs);
}
