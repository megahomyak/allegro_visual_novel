#include "loaders.c"


void test_action_handler(struct GameContext *game_context) {
	static bool some_flag = 1;
	if (some_flag) {
		some_flag = 0;
		load_location(game_context, TEST_LOCATION_3);
	} else {
		load_location(game_context, TEST_LOCATION_1);
	}
	render_location(game_context);
}


void (*action_handlers[])(struct GameContext *) = {
	test_action_handler
};
