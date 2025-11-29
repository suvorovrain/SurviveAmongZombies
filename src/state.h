#ifndef STATE_HEADER
#define STATE_HEADER

#include "../vendor/GTA-VI/include/engine/types.h"
#include "inttypes.h"

#define ENGINE_LOGIC_STEP (1.0f / 60.0f)

// state.h

// Sprite Sheet describe array of sprites for some state of
// player\enemy\projectile
typedef struct {
  uint64_t frames_count;
  Sprite *frames;
} SpriteSheet;

typedef enum { UNIT_PLAYER = 0, UNIT_ENEMY, UNIT_PROJECTILE } UnitType;

typedef enum { PLAYER_WALK = 0, PLAYER_HURTED, PLAYER_IDLE } PlayerState;

typedef struct {
  UnitType type; // must be UNIT_PLAYER
  Vector position;
  Vector movement;
  PlayerState state;
  // spritesheet for current state
  SpriteSheet spritesheet;
  double stat_movespeed;
  double stat_max_hp;
  double stat_hp;
  double stat_level;
  double stat_experience;
  double stat_damage;
  double stat_proj_count;
  // internal state (don't be used for render)
  uint64_t invincibility_count // after getting damage we has invincibility
} Player;

typedef enum { ENEMY_WALK = 0, ENEMY_IDLE, ENEMY_HURTED } EnemyState;

typedef struct {
  UnitType type; // must be UNIT_PLAYER
  Vector position;
  Vector movement;
  EnemyState state;
  // spritesheet for current state
  SpriteSheet spritesheet;
  double stat_movespeed;
  double stat_max_hp;
  double stat_hp;
  double stat_damage;
} Enemy;

typedef enum { PROJ_WALK = 0, PROJ_EXPLODE } ProjState;

typedef struct {
  UnitType type; // must be UNIT_PROJECTILE
  Vector position;
  Vector movement;
  ProjState state;
  // spritesheet for current state
  SpriteSheet spritesheet;
  double stat_movespeed;
  double stat_damage;
} Projectile;

// Is player alive (so game too) or player already dead and game at pause
typedef enum { GAME_ALIVE = 0, GAME_DEAD } GameStatus;

typedef struct {
  Player player;
  Enemy *enemies;
  uint64_t enemies_count;
  Projectile *projectiles;
  uint64_t projectiles_count;
  uint64_t kills;
  GameStatus status;
  // internal state (don't be used for render)
  uint64_t frame_counter; // count of frames lasted from begin of game
} GlobalState;

#endif