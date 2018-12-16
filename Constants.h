#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "Vector3f.h"

const bool IS_FULLSCREEN = false;
const int DEFAULT_FPS = 60;
const int INFO_FONT_SIZE_PX = 24;

const Vector3f CAM_INITIAL_POS = Vector3f(0.0f, 0.0f, -3.0f);
const float CAM_UNIT_SPEED = 1.0f;
const float CAM_DEGREES_PER_PIXEL = 0.2f;
const float CAM_SPEED_UP = 5.0f;

const float FOV = 67.0f;
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

const float EPSILON_FACTOR = 0.0001;
const float EPSILON_LIMIT = 4e-6f;
const float MAX_BAILOUT = 2.0f;  // Also called "DIVERGENCE"
const int MAX_ITER = 10;
const int MIN_ITER = 10;
const int MAX_STEPS = 20;
const float MAX_DIST = 350.0f;
const int POWER = 8;

const bool FOG_ENABLED = true;
const float FOG_MAX_DIST = 300.0f;

const float GLOW_DIST = 0.5f;
const bool GLOW_ENABLED = true;

const bool HEAT_ENABLED = false;

#endif /* CONSTANTS_H */