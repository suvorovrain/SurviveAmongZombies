// #include "dyn_objs.h"
// #include "state.h"
// #include <../vendor/GTA-VI/include/engine/coordinates.h>
// #include <../vendor/GTA-VI/include/engine/input.h>
// #include <../vendor/GTA-VI/include/engine/map.h>
// #include <../vendor/GTA-VI/include/engine/random.h>
// #include <../vendor/GTA-VI/include/engine/types.h>
// #include <math.h>
// #include <stb_ds.h>
// #include <stdlib.h>
// #include <string.h>

// #define ANIM_FRAME_TIME (1.0f / 8.0f) // in seconds
// #define MAN_COUNT 50
// #define SHEEPS_COUNT 30
// #define PLAYER_SPEED 5.5f

// typedef enum { ANIM_IDLE = 0, ANIM_WALK } AnimState;

// typedef enum { DIR_RIGHT = 0, DIR_LEFT, DIR_BACK, DIR_FORWARD } Direction;

// // Sprite collection for entity type
// typedef struct DynamicObjects {
//   GameObject *objects;
//   GameObject *player;
// } DynamicObjects;

// // Object animation data
// typedef struct {
//   UnitType type;
//   Sprite sprite;
// } EntityData;

// static GameObject *gen_dyn_objects(DynamicObjects *dyn_objs, Map *map) {
//   GameObject *objects = NULL;

//   if (!dyn_objs || !map || !dyn_objs->sprites[TYPE_MAN].all_frames ||
//       !dyn_objs->sprites[TYPE_SHEEP].all_frames)
//     return NULL;
//   VectorU32 map_size = map_get_size(map);

//   for (int i = 0; i < MAN_COUNT; i++) {
//     GameObject man = {0};

//     int x = map_size.x / 2 + i * 100;
//     int y = map_size.y / 2;
//     man.position = (Vector){(float)x, (float)y};

//     // Create entity data
//     EntityData *data = calloc(1, sizeof(EntityData));
//     data->type = TYPE_MAN;
//     data->sprites = &dyn_objs->sprites[TYPE_MAN];
//     data->state = ANIM_IDLE;
//     data->direction = DIR_BACK;
//     data->frame_in_anim = 0;
//     data->timer = 0.0f;

//     man.data = data;
//     man.velocity = (Vector){0, 0};

//     // Set initial sprite
//     man.cur_sprite =
//         &dyn_objs->sprites[TYPE_MAN].all_frames
//              [dyn_objs->sprites[TYPE_MAN].clips[ANIM_IDLE][DIR_BACK].start];

//     arrpush(objects, man);
//   }

//   // Create sheeps
//   for (int i = 0; i < SHEEPS_COUNT; i++) {
//     GameObject sheep = {0};

//     // Random positions around the map
//     VectorU32 pos = map_gen_random_position(map, 100, sheep.cur_sprite);
//     sheep.position = (Vector){(float)pos.x, (float)pos.y};

//     EntityData *data = calloc(1, sizeof(EntityData));
//     data->type = TYPE_SHEEP;
//     data->sprites = &dyn_objs->sprites[TYPE_SHEEP];
//     data->state = ANIM_IDLE;
//     data->direction = DIR_BACK;
//     data->frame_in_anim = 0;
//     data->timer = 0.0f;

//     sheep.data = data;
//     sheep.velocity = (Vector){0, 0};

//     // Set initial sprite
//     sheep.cur_sprite =
//         &dyn_objs->sprites[TYPE_SHEEP].all_frames
//              [dyn_objs->sprites[TYPE_SHEEP].clips[ANIM_IDLE][DIR_BACK].start];

//     arrpush(objects, sheep);
//   }

//   return objects;
// }

// DynamicObjects *create_dynamic_objects(Map *map) {
//   DynamicObjects *dyn_objs = calloc(1, sizeof(DynamicObjects));
//   if (!dyn_objs)
//     return NULL;

//   dyn_objs->sprites = calloc(TYPE_COUNT, sizeof(EntitySprites));
//   if (!dyn_objs->sprites) {
//     free(dyn_objs);
//     return NULL;
//   }

//   dyn_objs->sprites[TYPE_MAN] = create_man_sprites();
//   dyn_objs->sprites[TYPE_SHEEP] = create_sheep_sprites();

//   dyn_objs->objects = gen_dyn_objects(dyn_objs, map);
//   dyn_objs->player = &dyn_objs->objects[0]; // first man

//   return dyn_objs;
// }

// void free_dyn_objects(DynamicObjects *dyn_objs) {
//   if (!dyn_objs)
//     return;

//   for (int i = 0; i < TYPE_COUNT; i++) {
//     if (dyn_objs->sprites[i].all_frames) {
//       free_spritesheet_frames(dyn_objs->sprites[i].all_frames,
//                               dyn_objs->sprites[i].frame_count);
//     }
//   }

//   free(dyn_objs->sprites);
//   arrfree(dyn_objs->objects);
//   free(dyn_objs);
// }

// GameObject *dyn_objs_get_player(DynamicObjects *dyn_objs) {
//   if (!dyn_objs)
//     return NULL;
//   return dyn_objs->player;
// }

// GameObject *dyn_objs_get_objects(DynamicObjects *dyn_objs) {
//   if (!dyn_objs)
//     return NULL;
//   return dyn_objs->objects;
// }

// static void player_update_velocity(Input *input, GameObject *player) {
//   float ax = 0.0f, ay = 0.0f;
//   if (input->a)
//     ax -= 1.0f;
//   if (input->d)
//     ax += 1.0f;
//   if (input->w)
//     ay -= 1.0f;
//   if (input->s)
//     ay += 1.0f;

//   // Normalize diagonal movement
//   float len = sqrtf(ax * ax + ay * ay);
//   if (len > 0.0f) {
//     ax /= len;
//     ay /= len;
//   }

//   // Update velocity
//   player->velocity.x = ax * PLAYER_SPEED;
//   player->velocity.y = ay * PLAYER_SPEED;
// }

// // Just some random movements for npc
// static void npc_update_velocity(GameObject *npc) {
//   Vector oldv = npc->velocity;
//   float spd = sqrtf(oldv.x * oldv.x + oldv.y * oldv.y);
//   float moving = spd >= 0.1f; // 1 if moving, 0 if standing

//   // Probability to keep current state 0.99 (walking/standing)
//   float keep = rand_big() % 100 < 99;

//   float angle = ((float)rand_big() / 100.0f) * 6.0f;
//   Vector rand_dir = {cosf(angle), sinf(angle)};

//   Vector newv = {keep * oldv.x + (1 - keep) * (moving * rand_dir.x * spd),
//                  keep * oldv.y + (1 - keep) * (moving * rand_dir.y * spd)};

//   newv.x += (1 - moving) * (1 - keep) * rand_dir.x;
//   newv.y += (1 - moving) * (1 - keep) * rand_dir.y;

//   npc->velocity = newv;
// }

// // Npc runs away from player when too close
// static void npc_run_away(GameObject *player, GameObject *npc) {
//   float dx = player->position.x - npc->position.x;
//   float dy = player->position.y - npc->position.y;
//   float dist = sqrtf(dx * dx + dy * dy);
//   npc->velocity.x = +(dx / dist) * 2.0f;
//   npc->velocity.y = +(dy / dist) * 2.0f;
// }

// // Update animation and set sprite
// static void update_entity_animation(GameObject *obj) {
//   if (!obj)
//     return;
//   EntityData *data = (EntityData *)obj->data;
//   if (!data || !data->sprites)
//     return;

//   int clip_start = data->sprites->clips[data->state][data->direction].start;
//   int clip_count = data->sprites->clips[data->state][data->direction].count;
//   if (clip_count == 0)
//     return;
//   if (data->timer >= ANIM_FRAME_TIME) {
//     data->timer = 0.0f;
//     data->frame_in_anim = (data->frame_in_anim + 1) % clip_count;
//   }

//   // Set current sprite
//   int sprite_index = clip_start + data->frame_in_anim;
//   obj->cur_sprite = &data->sprites->all_frames[sprite_index];
// }

// void dyn_objs_update(DynamicObjects *dyn_objs, Input *input, float
// delta_time) {
//   if (!dyn_objs || !input)
//     return;

//   for (int i = 0; i < arrlen(dyn_objs->objects); i++) {
//     GameObject *obj = &dyn_objs->objects[i];
//     if (!obj || !obj->data)
//       continue;
//     EntityData *data = (EntityData *)obj->data;
//     data->timer += delta_time;

//     if (obj == dyn_objs->player) {
//       player_update_velocity(input, obj);
//     } else if (data->type == TYPE_MAN) {
//       npc_run_away(dyn_objs->player, obj);
//     } else if (data->type == TYPE_SHEEP) {
//       npc_run_away(dyn_objs->player, obj);
//     }

//     float dx = obj->velocity.x;
//     float dy = obj->velocity.y;
//     bool moving = fabs(dx) + fabs(dy) > 0.1f;
//     AnimState n_st = moving ? ANIM_WALK : ANIM_IDLE;
//     Direction n_dir = data->direction;
//     if (!moving) {
//       n_dir = data->direction;
//     } else if (fabs(dy) > fabs(dx) && dy > 0.1f) {
//       n_dir = DIR_BACK;
//     } else if (fabs(dy) > fabs(dx) && dy < -0.1f) {
//       n_dir = DIR_FORWARD;
//     } else if (fabs(dy) < fabs(dx) && dx > 0.1f) {
//       n_dir = DIR_RIGHT;
//     } else if (fabs(dy) < fabs(dx) && dx < -0.1f) {
//       n_dir = DIR_LEFT;
//     }
//     if (n_st != data->state) {
//       data->frame_in_anim = 0;
//     }
//     if (n_dir != data->direction) {
//       data->frame_in_anim = 0;
//     }
//     data->state = n_st;
//     data->direction = n_dir;
//     obj->position.x += dx;
//     obj->position.y += dy;

//     update_entity_animation(obj);
//   }
// }
