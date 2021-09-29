#include <stdint.h>

#include <allegro5/allegro.h>

#include "indices.h"


#define ct_len(string) (sizeof string / sizeof string[0])
#define ct_no_zero_len(string) (ct_len(string) - 1)
#define as_sized_string(string) {.data=(unsigned char *) string, .length=ct_no_zero_len(string)}


struct SizedString {
	unsigned char *data;
	unsigned int length;
};


struct Button {
	const struct SizedString text;
	enum Indices action_index;
};


enum ButtonIndices {
	TEST_BUTTON_1,
	TEST_BUTTON_2,
	BACK_TO_MENU_BUTTON
};


#define button(text, action_index) {as_sized_string(text), action_index}

struct Button buttons[] = {
	button("1", TEST_LOCATION_1),
	button("2", TEST_LOCATION_2),
	button("Go back", MENU_LOCATION),
	button("Custom action test", TEST_ACTION)
};

#undef button


struct Location {
	const char *image_filename;
	const unsigned int image_filename_length;
	const struct SizedString character_name;
	const struct SizedString text;
	uint_least8_t buttons_amount;
	enum ButtonIndices first_button_index;
};


#define location(image_filename, character_name, text, buttons_amount, first_button_index) {image_filename, ct_len(image_filename), as_sized_string(character_name), as_sized_string(text), buttons_amount, first_button_index}

struct Location locations[] = {
	// image_filename, character_name, text, buttons_amount, first_button_index
	location("menu.jpg", "Character's name", "Location text", 2, TEST_BUTTON_1),
	location("1.png", "", "1", 1, BACK_TO_MENU_BUTTON),
	location("2.jpg", "", "2", 2, BACK_TO_MENU_BUTTON),
	location("", "", "This location change was made using a custom handler! The next time this button will work differently", 1, BACK_TO_MENU_BUTTON)
};

#undef location
