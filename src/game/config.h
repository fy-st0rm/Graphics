#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "math/vec.h"

#define MAX_ENTITY_CNT 100

// Window
const v2 WIN_SIZE  = { 800, 600 };
const v2 SURF_SIZE = { 266, 200 };

// Player
#define PLAYER_SPRITE "assets/samurai.png"
#define PLAYER_SPRITE_X_CNT 14
#define PLAYER_SPRITE_Y_CNT 8
const v2 PLAYER_SIZE = { 64, 64 };

#endif // __CONFIG_H__
