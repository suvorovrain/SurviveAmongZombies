#ifndef STATE_HEADER
#define STATE_HEADER

#include "engine/types.h"
#include "inttypes.h"

#define ENGINE_LOGIC_STEP (1.0f / 60.0f)

// state.h

// Sprite Sheet describe array of sprites for some state of
// player\enemy\projectile
typedef struct {
  uint64_t frames_count;
  Sprite *frames;
} SpriteSheet;

typedef enum {
  UNIT_PLAYER = 0,
  UNIT_ENEMY,
  UNIT_PROJECTILE,
  UNIT_EXP_CRYSTAL
} UnitType;
typedef enum { DIRECTION_UP = 0, DIRECTION_DOWN } UnitDirectionFace;
typedef enum { DIRECTION_LEFT = 0, DIRECTION_RIGHT } UnitDirectionSide;

typedef enum { PLAYER_WALK = 0, PLAYER_HURTED, PLAYER_IDLE } PlayerState;

typedef struct {
  UnitType type; // must be UNIT_PLAYER
  Vector position;
  Vector movement;
  PlayerState state;
  // spritesheet for current state
  Sprite current_sprite;
  double stat_movespeed;
  double stat_max_hp;
  double stat_hp;
  double stat_level;
  double stat_experience;
  double stat_experience_for_lvlup;
  double stat_attack_speed;
  double stat_damage;
  double stat_proj_count;
  double stat_piercing;
  // bosts
  double boost_attack_speed_percent;
  double boost_piercing_percent;
  double boost_movement_percent;
  double boost_experience_percent;
  double boost_damage_percent;
  // internal state (don't be used for render)
  uint64_t invincibility_count; // after getting damage we has invincibility
  UnitDirectionFace direction_face;
  UnitDirectionSide direction_side;
  SpriteSheet spritesheet_move;
} Player;

typedef enum { ENEMY_WALK = 0, ENEMY_IDLE, ENEMY_HURTED } EnemyState;
typedef enum { ENEMY_IMP = 0, ENEMY_GOBLIN, ENEMY_SLIME } EnemyType;

typedef struct {
  UnitType type; // must be UNIT_PLAYER
  EnemyType type_enemy;
  Vector position;
  Vector movement;
  EnemyState state;
  // spritesheet for current state
  Sprite current_sprite;
  double stat_movespeed;
  double stat_max_hp;
  double stat_hp;
  double stat_damage;
  // internal state (don't be user for render)
  SpriteSheet spritesheet_move;
} Enemy;

typedef enum { PROJ_WALK = 0, PROJ_EXPLODE } ProjState;

typedef struct {
  UnitType type; // must be UNIT_PROJECTILE
  Vector position;
  Vector movement;
  ProjState state;
  // spritesheet for current state
  Sprite current_sprite;
  double stat_movespeed;
  double stat_damage;
  // internal state
  uint64_t live_frames_last; // count of frames that projectile can be alive
  uint64_t kills;
  uint64_t explode_frame; // frame when projectile get explode state
  SpriteSheet spritesheet_move;
  SpriteSheet spritesheet_explode;
} Projectile;

typedef enum { CRYSTAL_BLUE = 0, CRYSTAL_GREEN } CrystalType;

typedef struct {
  UnitType type; // must be UNIT_EXP_CRYSTAL
  CrystalType type_crystal;
  Vector position;
  Vector movement;
  Sprite current_sprite;
  SpriteSheet spritesheet;
} Crystal;

// Is player alive (so game too) or player already dead and game at pause
typedef enum {
  GAME_ALIVE = 0,
  GAME_DEAD,
  GAME_PAUSE,
  GAME_LEVEL_UP
} GameStatus;

typedef struct {
  Player player;
  Enemy *enemies;
  uint64_t enemies_count;
  Projectile *projectiles;
  uint64_t projectiles_count;
  Crystal *exp_crystal;
  uint64_t exp_crystal_count;
  uint64_t kills;
  GameStatus status;
  // internal state (don't be used for render)
  uint64_t frame_counter; // count of frames lasted from begin of game
  float enemy_factor;
} GlobalState;

#endif