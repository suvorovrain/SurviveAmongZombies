#pragma once

#include "../state.h"
#include "../vendor/GTA-VI/include/engine/input.h"
#include "../vendor/GTA-VI/include/engine/types.h"

GlobalState init_global_state(Map *map);
void make_step(GlobalState *state, Input input);