#include <cwutils/cwbox.h>

Vector2 cwbox_get_point(Rectangle self, float x, float y) {
    return (Vector2) { self.x + self.width * x, self.y + self.height * y };
}

Rectangle cwbox_pad(Rectangle self, uint16_t padding_x, uint16_t padding_y) {
	return (Rectangle) {
		.x      = self.x + padding_x,
		.y      = self.y + padding_y,
		.width  = self.width - 2 * padding_x,
		.height = self.height - 2 * padding_y,
	};
}

Rectangle cwbox_split_top(Rectangle* self, uint16_t size) {
	Rectangle output = *self;
	output.height = size;

	self -> y += size;
	self -> height -= size;
	return output;
}

Rectangle cwbox_split_bottom(Rectangle* self, uint16_t size) {
	Rectangle output = *self;

	output.y = (self -> height - size);
	output.height = size;

	self -> height -= size;

	return output;
}

Rectangle cwbox_split_left(Rectangle* self, uint16_t size) {
	Rectangle output = *self;
	output.width = size;

	self -> x += size;
	self -> width -= size;

	return output;
}

Rectangle cwbox_split_right(Rectangle* self, uint16_t size) {
	Rectangle output = *self;
	output.x = self -> x + self -> width - size;
	output.width = size;

	self -> width -= size;
	return output;
}

Rectangle cwbox_get_centered(Rectangle self, uint16_t w, uint16_t h) {
	Rectangle output;

	uint16_t center_x = (self.x + self.width / 2);
	uint16_t center_y = (self.y + self.height / 2);

	output.x = center_x - (w / 2);
	output.y = center_y - (h / 2);
	output.width = w;
	output.height = h;

	return output;
}
