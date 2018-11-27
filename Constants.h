#ifndef CONSTANTS_H
#define CONSTANTS_H

const bool IS_FULLSCREEN = false;
const int DEFAULT_FPS = 60;
const int INFO_FONT_SIZE_PX = 24;
const float CAM_UNIT_SPEED = 1.0f;
const float CAM_DEGREES_PER_PIXEL = 0.2f;
const float CAM_SPEED_UP = 5.0f;

const float FOV = 67.0f;
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

const float EPSILON_FACTOR = 0.001f;
const float EPSILON_LIMIT = 1e-6f;
const float MAX_BAILOUT = 2.0f;
const int MAX_ITER = 100;
const int MIN_ITER = 4;
const int MAX_STEPS = 100;
const float MAX_DIST = 400.0f;
const int POWER = 8;

const bool FOG_ENABLED = true;
const float FOG_MAX_DIST = 300.0f;

#endif /* CONSTANTS_H */