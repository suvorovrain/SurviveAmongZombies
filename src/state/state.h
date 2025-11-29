#pragma once

#include "../state.h"
#include "input.h"
#include "types.h"

GlobalState init_global_state();
void make_step(GlobalState *state, Input input);