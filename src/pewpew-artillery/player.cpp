#include "player.h"
#include <qstring.h>
#include <math.h>

#define MAX_HP 100
#define DEFAULT_ANGLE M_PI/6
#define MAX_POWER 1
#define MAX_POSITION 1            //0 == left end; 1 == right end

Player::Player(): hp(MAX_HP), position(MAX_POSITION), angle(DEFAULT_ANGLE), power(MAX_POWER){}
