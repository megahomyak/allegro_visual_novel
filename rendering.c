#include <allegro5/allegro_primitives.h>

#include "game_context.h"


#define UNUSED(variable) (void)(variable)


bool text_placement_callback(int line_num, const ALLEGRO_USTR *line, void *lines_counter) {
	UNUSED(line_num); UNUSED(line);
	++(* (unsigned int *) lines_counter);
	return true;  // Continue calculating
}


#define sized_string_as_allegro_ustr(sized_string, allegro_ustr_name) ALLEGRO_USTR allegro_ustr_name; allegro_ustr_name.data = sized_string->data; allegro_ustr_name.slen = sized_string->length; allegro_ustr_name.mlen = sized_string->length


void draw_multiline_ustr(
	ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, ALLEGRO_COLOR color, unsigned int x, unsigned int y,
	unsigned int font_height, unsigned int width, ALLEGRO_USTR *string
) {
	al_set_target_backbuffer(display);
	al_draw_multiline_ustr(
		font, color, x, y, width, font_height, /*flags: int=*/0, string
	);
}


unsigned int get_multiline_ustr_height(
		ALLEGRO_FONT *font, unsigned int font_height,
		unsigned int width, ALLEGRO_USTR *string
) {
	unsigned int lines_counter = 0;
	al_do_multiline_ustr(
		font, width, string,
		&text_placement_callback, &lines_counter
	);
	return lines_counter * font_height;
}


unsigned int get_height_of_multiline_ustr_and_draw_it(
	ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, ALLEGRO_COLOR color, unsigned int x, unsigned int y,
	unsigned int font_height, unsigned int width, ALLEGRO_USTR *string
) {
	unsigned int text_height = get_multiline_ustr_height(font, font_height, width, string);
	draw_multiline_ustr(display, font, color, x, y - text_height, font_height, width, string);
	return text_height;
}


void render_location_without_free(struct GameContext *game_context) {
	al_clear_to_color(game_context->colors.background);
	ALLEGRO_DISPLAY *display = game_context->display_info.display;
	unsigned int lower_border = game_context->display_info.height;
	unsigned int display_width = game_context->display_info.width;
	game_context->display_info.width = display_width;
	game_context->display_info.height = lower_border;
	unsigned int buttons_and_image_indent = game_context->indents.buttons_and_image;
	unsigned int text_indent = game_context->indents.button_text;
	unsigned int full_text_indent = buttons_and_image_indent + text_indent;
	unsigned int character_name_indent = full_text_indent;
	const struct SizedString *character_name = &game_context->current_location->character_name;
	unsigned int location_text_indent = *character_name->data ? character_name_indent + text_indent : character_name_indent;
	unsigned int character_name_width = display_width - character_name_indent - text_indent;
	unsigned int location_text_width = display_width - location_text_indent - text_indent;
	unsigned int button_text_width = display_width - full_text_indent * 2;
	ALLEGRO_FONT *font = game_context->font_info.font;
	unsigned int font_height = game_context->font_info.font_height;
	ALLEGRO_COLOR button_color = game_context->colors.button;
	ALLEGRO_COLOR text_color = game_context->colors.text;
	uint_least8_t buttons_amount = game_context->current_location->buttons_amount;
	unsigned int *button_heights_from_bottom = (unsigned int *) malloc(sizeof (unsigned int) * buttons_amount);
	game_context->button_heights_from_bottom = button_heights_from_bottom;
	unsigned int last_button_index = game_context->current_location->first_button_index + buttons_amount - 1;
	for (
		unsigned int current_button_index_from_bottom = 0;
		current_button_index_from_bottom != buttons_amount;
		++current_button_index_from_bottom
	) {
		lower_border -= buttons_and_image_indent;
		unsigned int old_lower_border = lower_border;
		const struct SizedString *button_text = &buttons[last_button_index - current_button_index_from_bottom].text;
		sized_string_as_allegro_ustr(button_text, button_text_ustr);
		lower_border -= get_multiline_ustr_height(font, font_height, button_text_width, &button_text_ustr);
		lower_border -= text_indent * 2;
		al_draw_filled_rectangle(
			/*top left corner x: float=*/buttons_and_image_indent,
			/*top left corner y: float=*/lower_border,
			/*lower right corner x: float=*/display_width - buttons_and_image_indent,
			/*lower right corner y: float=*/old_lower_border,
			/*color: ALLEGRO_COLOR=*/button_color
		);
		button_heights_from_bottom[current_button_index_from_bottom] = old_lower_border - lower_border;
		draw_multiline_ustr(
			display, font, text_color,
			/*x: unsigned int=*/full_text_indent,
			/*y: unsigned int=*/lower_border + text_indent,
			font_height, button_text_width,
			&button_text_ustr
		);
	}
	const struct SizedString *location_text = &game_context->current_location->text;
	if (*(location_text->data)) {
		lower_border -= text_indent;
		sized_string_as_allegro_ustr(location_text, location_text_ustr);
		lower_border -= get_height_of_multiline_ustr_and_draw_it(
			display, font, text_color,
			/*x: float=*/location_text_indent,
			/*y: float=*/lower_border,
			font_height, location_text_width, &location_text_ustr
		);
	}
	if (*(character_name->data)) {
		sized_string_as_allegro_ustr(character_name, character_name_ustr);
		lower_border -= text_indent;
		lower_border -= get_height_of_multiline_ustr_and_draw_it(
			display, font, text_color,
			/*x: float=*/character_name_indent,
			/*y: float=*/lower_border,
			font_height, character_name_width, &character_name_ustr
		);
	}
	if (
		*(game_context->current_location->text.data)
		|| *(game_context->current_location->character_name.data)
	) {
		lower_border -= text_indent;
	} else if (buttons_amount != 0) {
		lower_border -= buttons_and_image_indent;
	}
	ALLEGRO_BITMAP *image = game_context->current_image;
	if (image) {
		unsigned int image_height = al_get_bitmap_height(image);
		unsigned int image_width = al_get_bitmap_width(image);
		if (image_width / image_height > display_width / lower_border) {
			unsigned int resized_image_height = (image_height * display_width) / image_width;
			al_draw_scaled_bitmap(
				/*bitmap: ALLEGRO_IMAGE *=*/image,
				/*source x: float=*/0,
				/*source y: float=*/0,
				/*source width: float=*/image_width,
				/*source height: float=*/image_height,
				/*destination x: float=*/0,
				/*destination y: float=*/lower_border / 2 - resized_image_height / 2,
				/*destination width: float=*/display_width,
				/*destination height: float=*/resized_image_height,
				/*flags: int=*/0  // No flags
			);
		} else {
			unsigned int resized_image_width = (image_width * lower_border) / image_height;
			al_draw_scaled_bitmap(
				/*bitmap: ALLEGRO_IMAGE *=*/image,
				/*source x: float=*/0,
				/*source y: float=*/0,
				/*source width: float=*/image_width,
				/*source height: float=*/image_height,
				/*destination x: float=*/display_width / 2 - resized_image_width / 2,
				/*destination y: float=*/0,
				/*destination width: float=*/resized_image_width,
				/*destination height: float=*/lower_border,
				/*flags: int=*/0  // No flags
			);
		}
	}
	al_flip_display();
}


void render_location(struct GameContext *game_context) {
	free(game_context->button_heights_from_bottom);
	render_location_without_free(game_context);
}
