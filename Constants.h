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
const float EPSILON_LIMIT = 1e-6;
const float MAX_BAILOUT = 4.0f;  // probably can compute this
const int MAX_ITER = 11;
const int MAX_STEPS = 250;
const float MAX_DIST = 100.0f;
const float DIVERGENCE = 2.0f;
const int POWER = 8;

#endif /* CONSTANTS_H */