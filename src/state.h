#include "inttypes.h"

// include "types.h" from GTA-VI

typedef struct {
  float x, y;
} Vector;

typedef struct {
  uint32_t *pixels;
  int width;
  int height;
} Sprite;

// state.h

// Sprite Sheet describe array of sprites for some state of
// player\enemy\projectile
typedef struct {
  uint64_t frames_count;
  Sprite *frames;
} SpriteSheet;

typedef enum { PLAYER_WALK = 0, PLAYER_IDLE, PLAYER_HURTED } PlayerState;

typedef struct {
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
} Player;

typedef enum { ENEMY_WALK = 0, ENEMY_IDLE, ENEMY_HURTED } EnemyState;

typedef struct {
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
} GlobalState;