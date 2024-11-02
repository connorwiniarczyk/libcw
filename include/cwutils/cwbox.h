#ifndef CWBOX_H
#define CWBOX_H

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

#include <stdint.h>

Vector2 cwbox_get_point(Rectangle self, float x, float y);

Rectangle cwbox_pad(Rectangle self, uint16_t padding_x, uint16_t padding_y);
Rectangle cwbox_get_centered(Rectangle self, uint16_t width, uint16_t height);

Rectangle cwbox_split_top(Rectangle* self, uint16_t size);
Rectangle cwbox_split_bottom(Rectangle* self, uint16_t size);
Rectangle cwbox_split_left(Rectangle* self, uint16_t size);
Rectangle cwbox_split_right(Rectangle* self, uint16_t size);

#endif
