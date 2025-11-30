#pragma once

#include "../state.h"
#include "engine/input.h"
#include "engine/map.h"
#include "engine/types.h"

GlobalState init_global_state(Map *map);
void make_step(GlobalState *state, Input input);
void player_level_up(Player *player, GlobalState *state);
