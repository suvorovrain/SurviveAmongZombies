#include "../game.h"
#include "engine/types.h"

typedef enum {
  UI_HP_BAR = 0,
  UI_EXP_BAR,
  UI_DEAD_BACKGROUND,
  UI_DEAD_SCREEN,
  UI_LEVEL,
  UI_KILLS,
  UI_TIME,
  UI_COUNT
} UIType;

void um_ui_init(Game *game);
void um_ui_update(Game *game);
void um_ui_enable(UIType type);
void um_ui_disable(UIType type);
size_t um_ui_get_uis_count();
UIElement **um_ui_get_uis();