#include <allegro5/allegro_ttf.h>

#include <stdio.h>

#include "rendering.c"


void load_location(struct GameContext *game_context, enum Indices location_index);
void change_location(struct GameContext *game_context, uint_least8_t button_index);
extern void (*action_handlers[])(struct GameContext *);


#define lightweight_print(string) fputs(string, stdout)


const char images_directory_name[] = "data/images/";


ALLEGRO_BITMAP * load_image(const char *filename, size_t filename_length) {
	if (!*filename) {
		return NULL;
	}
	char *full_path = (char *) malloc(filename_length + ct_no_zero_len(images_directory_name));
	if (!full_path) {
		lightweight_print("Unable to allocate memory for a full image path!");
		exit(1);
	}
	memcpy(full_path, images_directory_name, ct_no_zero_len(images_directory_name));
	memcpy(full_path + ct_no_zero_len(images_directory_name), filename, filename_length);
	ALLEGRO_BITMAP *image = al_load_bitmap(full_path);
	free(full_path);
	if (!image) {
		lightweight_print("Unable to load an image for the current location! (Path: ");
		lightweight_print(full_path);
		lightweight_print(")");
		exit(1);
	}
	return image;
}


void load_menu_location(struct GameContext *game_context) {
	game_context->current_location = locations + MENU_LOCATION;
	game_context->current_image = load_image(
		locations[MENU_LOCATION].image_filename,
		locations[MENU_LOCATION].image_filename_length
	);
}


void load_location(struct GameContext *game_context, enum Indices location_index) {
	// Do al_init() and al_init_image_addon() before using this
	const char *old_image_filename = game_context->current_location->image_filename;
	const char *new_image_filename = locations[location_index].image_filename;
	game_context->current_location = locations + location_index;
	if (strcmp(old_image_filename, new_image_filename)) {
		// Strings are not equal
		al_destroy_bitmap(game_context->current_image);
		game_context->current_image = load_image(
			new_image_filename,
			locations[location_index].image_filename_length
		);
	}
}


void change_location(struct GameContext *game_context, uint_least8_t button_index) {
	// Do al_init() and al_init_image_addon() before using this
	enum Indices index = buttons[game_context->current_location->first_button_index + button_index].action_index;
	if (index < FIRST_ACTION) {  // This is a location
		load_location(game_context, index);
	} else {  // This is an action
		action_handlers[index - FIRST_ACTION](game_context);
	}
}


ALLEGRO_FONT * load_ttf_font(const char *font_path, unsigned int font_size) {
	// You need to call al_init_ttf_addon() before calling this
	ALLEGRO_FONT *font = al_load_ttf_font(
		font_path,
		font_size,
		/*flags: int=*/0
	);
	if (!font) {
		lightweight_print("Font ");
		lightweight_print(font_path);
		lightweight_print(" wasn't found!");
		exit(1);
	}
	return font;
}
