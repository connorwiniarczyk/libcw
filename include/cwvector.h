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


#ifdef CWVECTOR_IMPLEMENTATION

Vector2 cwvec2_add(Vector2 a, Vector2 b) { return (Vector2){ a.x + b.x, a.y + b.y }; }
Vector2 cwvec2_sub(Vector2 a, Vector2 b) { return (Vector2){ a.x - b.x, a.y - b.y }; }
float   cwvec2_dot(Vector2 a, Vector2 b) { return  a.x * b.x + a.y * b.y; }

float cwvec2_distance(Vector2 a, Vector2 b) { return cwvec2_magnitude(cwvec2_sub(a, b)); }

// TODO
// Vector2 cwvec2_cross(Vector2 a, Vector2 b);

float   cwvec2_magnitude(Vector2 a) { return sqrt(a.x * a.x + a.y * a.y); }
Vector2 cwvec2_normalize(Vector2 a) {
    float mag = cwvec2_magnitude(a);
    return (Vector2){a.x / mag, a.y / mag};
}

Vector2 cwvec2_scale(Vector2 a, float scale) {  return (Vector2){ a.x * scale, a.y * scale }; }
Vector2 cwvec2_rotate(Vector2 a, float angle) {
    return (Vector2) {
        cos(angle) * a.x - sin(angle) * a.y,
        sin(angle) * a.x + cos(angle) * a.y,
    };
}

#endif
