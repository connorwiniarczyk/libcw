#ifndef CWGEOMETRY_H
#define CWGEOMETRY_H

#include <stdint.h>
#include <stdbool.h>

#ifndef RAYLIB_H
typedef struct Rectangle {
    float x;
    float y;
    float width;
    float height;
} Rectangle;

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

bool cwbox_is_inside(Rectangle self, Vector2 point);
Vector2 cwbox_get_point(Rectangle self, float x, float y);

Rectangle cwbox_pad(Rectangle self, uint16_t padding_x, uint16_t padding_y);
Rectangle cwbox_get_centered(Rectangle self, uint16_t width, uint16_t height);
Rectangle cwbox_split_top(Rectangle* self, uint16_t size);
Rectangle cwbox_split_bottom(Rectangle* self, uint16_t size);
Rectangle cwbox_split_left(Rectangle* self, uint16_t size);
Rectangle cwbox_split_right(Rectangle* self, uint16_t size);

#endif
