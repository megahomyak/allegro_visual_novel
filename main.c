#include <allegro5/allegro_image.h>

#include "action_handlers.c"


void check_init(bool init_is_successful, char *name) {
	if (!init_is_successful) {
		lightweight_print("Unable to initialize ");
		lightweight_print(name);
		lightweight_print("!");
		exit(1);
	}
}


ALLEGRO_DISPLAY * make_new_allegro_display(unsigned int width, unsigned int height, const char *window_title) {
	al_set_new_window_title(window_title);
	al_set_new_display_flags(ALLEGRO_RESIZABLE);
	return al_create_display(width, height);
}


void handle_button_click(struct GameContext *game_context, unsigned int x, unsigned int y) {
	unsigned int buttons_indent = game_context->indents.buttons_and_image;
	if (x > buttons_indent && x < game_context->display_info.width - buttons_indent) {
		unsigned int *current_button_height = game_context->button_heights_from_bottom;
		unsigned int lower_border = game_context->display_info.height;
		for (
			unsigned int *after_last_button = current_button_height + game_context->current_location->buttons_amount;
			current_button_height != after_last_button;
			++current_button_height
		) {
			lower_border -= buttons_indent;
			if (lower_border > y && lower_border - *current_button_height < y) {
				change_location(game_context, /*button_index: uint_least8_t=*/after_last_button - current_button_height - 1);
				render_location(game_context);
				return;
			}
			lower_border -= *current_button_height;
		}
	}
}


#define DEFAULT_DISPLAY_WIDTH 640
#define DEFAULT_DISPLAY_HEIGHT 480


#define DEFAULT_BUTTON_INDENT 20
#define DEFAULT_TEXT_INDENT 20
#define DEFAULT_FONT_SIZE 20

const char default_font_path[] = "data/fonts/jetbrains_mono.ttf";


int main(int argc, char **argv) {  // Not (void) because conflicting with allegro
    UNUSED(argc); UNUSED(argv);
	check_init(al_init(), "allegro");

	ALLEGRO_DISPLAY *display = make_new_allegro_display(DEFAULT_DISPLAY_WIDTH, DEFAULT_DISPLAY_HEIGHT, "Моя игра");
	check_init(display, "display");
	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	check_init(event_queue, "event queue");
	check_init(al_init_image_addon(), "image addon");
    check_init(al_init_primitives_addon(), "primitives addon");

    bool mouse_or_touch_input_was_installed_successfully = false;
    if (al_install_mouse()) {
        mouse_or_touch_input_was_installed_successfully = true;
        al_register_event_source(event_queue, al_get_mouse_event_source());
    }
    if (al_install_touch_input()) {
        mouse_or_touch_input_was_installed_successfully = true;
        al_register_event_source(event_queue, al_get_touch_input_event_source());
    }
    if (!mouse_or_touch_input_was_installed_successfully) {
        lightweight_print("Unable to initialize mouse or touch input!");
        exit(1);
    }

	al_register_event_source(event_queue, al_get_display_event_source(display));

	struct GameContext game_context;
	struct GameContext *game_context_ptr = &game_context;
	game_context.display_info.display = display;
	game_context.display_info.height = al_get_display_height(display);
	game_context.display_info.width = al_get_display_width(display);
	game_context.colors.background = al_map_rgb(0, 0, 0);  // Black
	game_context.colors.button = al_map_rgb(48, 48, 48);  // Dark gray
	game_context.colors.text = al_map_rgb(255, 255, 255);  // White
	game_context.indents.button_text = DEFAULT_TEXT_INDENT;
	game_context.indents.buttons_and_image = DEFAULT_BUTTON_INDENT;

    check_init(al_init_font_addon(), "font addon");
	check_init(al_init_ttf_addon(), "ttf addon");
	ALLEGRO_FONT *font = load_ttf_font(default_font_path, DEFAULT_FONT_SIZE);
	set_font(game_context_ptr, font);

	load_menu_location(game_context_ptr);
	render_location_without_free(game_context_ptr);
	ALLEGRO_EVENT event;

	for (;;) {
		al_wait_for_event(event_queue, &event);
		switch (event.type) {
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				goto event_loop_end;
			case ALLEGRO_EVENT_DISPLAY_RESIZE:
				game_context.display_info.height = al_get_display_height(display);
				game_context.display_info.width = al_get_display_width(display);
				render_location(game_context_ptr);
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				if (event.mouse.button & 1) {
					// Left mouse button is held
					handle_button_click(&game_context, event.mouse.x, event.mouse.y);
				}
				break;
			case ALLEGRO_EVENT_TOUCH_BEGIN:
				handle_button_click(&game_context, event.touch.x, event.touch.y);
				break;
		}
	}
	event_loop_end:
	al_destroy_display(display);
    al_shutdown_font_addon();
	al_shutdown_ttf_addon();
    al_shutdown_primitives_addon();
	al_shutdown_image_addon();
	al_destroy_font(font);
	al_destroy_event_queue(event_queue);
	return 0;
}
