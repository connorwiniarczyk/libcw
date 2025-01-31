#ifndef CWVECTOR_H
#define CWVECTOR_H

#include <math.h>

#ifndef RAYLIB_H
typedef struct Vector2 {
    float x;
    float y;
} Vector2;
#endif

Vector2 cwvec2_add(Vector2 a, Vector2 b);
Vector2 cwvec2_sub(Vector2 a, Vector2 b);
float   cwvec2_dot(Vector2 a, Vector2 b);
Vector2 cwvec2_cross(Vector2 a, Vector2 b);
float   cwvec2_distance(Vector2 a, Vector2 b);

Vector2 cwvec2_normalize(Vector2 a);
float   cwvec2_magnitude(Vector2 a);

Vector2 cwvec2_rotate(Vector2 a, float angle);
Vector2 cwvec2_scale(Vector2 a, float scale);

#endif
