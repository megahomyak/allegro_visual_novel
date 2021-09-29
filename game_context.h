#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "locations.h"


struct GameContext {
	struct Location *current_location;
	ALLEGRO_BITMAP *current_image;
	struct {
		ALLEGRO_DISPLAY *display;
		unsigned int width;
		unsigned int height;
	} display_info;
	struct {
		unsigned int buttons_and_image;
		unsigned int button_text;
	} indents;
	struct {
		ALLEGRO_COLOR background;
		ALLEGRO_COLOR button;
		ALLEGRO_COLOR text;
	} colors;
	struct {
		ALLEGRO_FONT *font;
		unsigned int font_height;
	} font_info;
	unsigned int *button_heights_from_bottom;
};


void set_font(struct GameContext *game_context, ALLEGRO_FONT *font) {
	game_context->font_info.font = font;
	game_context->font_info.font_height = al_get_font_line_height(font);
}
