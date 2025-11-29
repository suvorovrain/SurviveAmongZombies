#include "core/types_priv.h"
#include "graphics/alpha_blend.h"
#include "graphics/camera.h"
#include "graphics/render.h"
#include "random/random_priv.h"
#include "world/map_priv.h"
#include <engine/coordinates.h>
#include <engine/random.h>
#include <engine/types.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static bool map_is_valid_position(const Map *map, int x, int y);
static TileType map_get_tile(const Map *map, int x, int y);
static void map_render_tiles(Map *map);
static void map_gen_tiles(Map *map);
static Sprite gen_tile(Color base, TileType type);

// Create map. Tiles are generated inside map_create
// Pre-render tiles into buffer

Map *map_create(int width, int height) {
  Map *map = (Map *)calloc(1, sizeof(Map));
  if (!map)
    return NULL;

  map->width = width;
  map->height = height;
  map->width_pix =
      (width + height) * (ISO_TILE_WIDTH / 2) + ISO_TILE_WIDTH + 20;
  map->height_pix =
      (width + height) * (ISO_TILE_HEIGHT / 2) + ISO_TILE_HEIGHT + 20;

  map->pixels =
      (uint32_t *)calloc(map->width_pix * map->height_pix, sizeof(uint32_t));
  if (!map->pixels) {
    free(map);
    return NULL;
  }

  map->tiles = (TileType *)calloc(width * height, sizeof(TileType));
  if (!map->tiles) {
    free(map->pixels);
    free(map);
    return NULL;
  }

  for (int i = 0; i < width * height; i++)
    map->tiles[i] = TILE_GRASS;

  map->tile_sprites[TILE_GRASS] =
      load_sprite("assets/static/grass.png", 4.0f);

  map_render_tiles(map);

  return map;
}

void map_free(Map *map) {
  if (!map)
    return;

  if (map->pixels) {
    free(map->pixels);
  }
  for (int i = 0; i < TILE_MAX; i++) {
    free_sprite(&map->tile_sprites[i]);
  }
  if (map->tiles) {
    free(map->tiles);
  }
  free(map);
}

// Returns map size in pixels
VectorU32 map_get_size(const Map *map) {
  VectorU32 size = {0, 0};
  if (!map)
    return size;

  size.x = (uint32_t)map->width_pix;
  size.y = (uint32_t)map->height_pix;
  return size;
}

// Returns pixel color at given map pixel coordinates
// If coordinates are out of bounds, returns 0
uint32_t map_get_pixel(const Map *map, int x, int y) {
  if (!map || x < 0 || y < 0 || x >= map->width_pix || y >= map->height_pix)
    return 0;

  return map->pixels[y * map->width_pix + x];
}

static bool map_is_valid_position(const Map *map, int x, int y) {
  if (!map)
    return false;
  return (x >= 0 && x < map->width && y >= 0 && y < map->height);
}

static TileType map_get_tile(const Map *map, int x, int y) {
  if (!map || !map->tiles)
    return TILE_EMPTY;

  if (map_is_valid_position(map, x, y)) {
    return map->tiles[y * map->width + x];
  }
  return TILE_EMPTY;
}

static void map_render_tiles(Map *map) {
  if (!map || !map->tiles)
    return;

  // Tile coordinates
  for (int yy = 0; yy < map->height + 20; yy++) {
    for (int xx = 0; xx < map->width + 20; xx++) {
      TileType tile = map_get_tile(map, xx, yy);
      if (tile == TILE_EMPTY)
        continue;

      Sprite *sprite = &map->tile_sprites[tile];
      if (!sprite->pixels)
        continue;

      Vector world_pos = iso_tile_to_world(xx, yy, map->height);

      int map_x = (int)(world_pos.x - (ISO_TILE_WIDTH / 2));
      int map_y = (int)world_pos.y;

      // Draw tiles
      for (int sy = 0; sy < sprite->height; sy++) {
        for (int sx = 0; sx < sprite->width; sx++) {
          int px = map_x + sx;
          int py = map_y + sy;

          // Bounds check
          if (px < 0 || px >= map->width_pix || py < 0 ||
              py >= map->height_pix) {
            continue;
          }

          uint32_t src = sprite->pixels[sy * sprite->width + sx];
          int idx = py * map->width_pix + px;
          uint32_t dst = map->pixels[idx];
          map->pixels[idx] = alpha_blend(src, dst);
        }
      }
    }
  }
}

// Generate a random position within the map boundaries, considering a margin
VectorU32 map_gen_random_position(const Map *map, uint32_t margin, const Sprite *sprite) {
    VectorU32 pos = {0,0};
    if (!map || !sprite) return pos;

    uint32_t max_x = map->width_pix - sprite->width - margin;
    uint32_t max_y = map->height_pix - sprite->height - margin;

    if (max_x <= margin) max_x = margin;
    if (max_y <= margin) max_y = margin;

    pos.x = margin + (rand_big() % (max_x - margin + 100));
    pos.y = margin + (rand_big() % (max_y - margin + 100));

    return pos;
}

