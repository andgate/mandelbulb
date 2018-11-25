#ifndef CONSTANTS_H
#define CONSTANTS_H

const float CAM_UNIT_SPEED = 1.0f;
const float CAM_DEGREES_PER_PIXEL = 0.2f;

const float FOV = 67.0f;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const float EPSILON_FACTOR = 0.001f;
const float MAX_BAILOUT = 256.0f;  // probably can compute this
const int MAX_ITER = 4;
const int MAX_STEPS = 250;

#endif /* CONSTANTS_H */