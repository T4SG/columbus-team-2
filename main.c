/**
 * main.c - Sets up the main Window and an ActionMenuLayer to prompt a button
 * press. Once pressed, an ActionMenu is shown to allow choice of a
 * vibration pattern
 */

#include <pebble.h>

typedef enum {
    VibrationTypeShort,
    VibrationTypeLong,
    VibrationTypeDouble,
    VibrationTypeCustomShort,
    VibrationTypeCustomMedium,
    VibrationTypeCustomLong
} VibrationType;

typedef struct {
    VibrationType type;
} Context;

static Window *s_main_window;
static TextLayer *s_label_layer;
static ActionBarLayer *s_action_bar;

static GBitmap *s_ellipsis_bitmap, *s_check_bitmap;





static ActionMenu *s_action_menu;
static ActionMenuLevel *s_root_level, *s_custom_level, *s_second_level, *s_third_level, *s_fourth_level;

/********************************* ActionMenu *********************************/

static void action_performed_callback(ActionMenu *action_menu, const ActionMenuItem *action, void *context) {
    // An action was selected, determine which one
    VibrationType type = *(VibrationType*)action_menu_item_get_action_data(action);
    
    // Play this vibration
    static uint32_t segments[5];
    switch(type) {
        case VibrationTypeShort:  vibes_short_pulse();  break;
        case VibrationTypeLong:   vibes_long_pulse();   break;
        case VibrationTypeDouble: vibes_double_pulse(); break;
        default:
            // Create the patten
            for(int i = 0; i < 5; i++) {
                switch(type) {
                    case VibrationTypeCustomShort:  segments[i] = i * 100; break;
                    case VibrationTypeCustomMedium: segments[i] = i * 200; break;
                    case VibrationTypeCustomLong:   segments[i] = i * 300; break;
                    default: break;
                }
            }
            
            // Play the custom pattern
            VibePattern pat = {
                .durations = segments,
                .num_segments = ARRAY_LENGTH(segments),
            };
            vibes_enqueue_custom_pattern(pat);
            break;
    }
}

static void init_action_menu() {
    // Create the root level
    s_root_level = action_menu_level_create(4);
    
    // Create and set up the secondary level, adding it as a child to the root one
    s_custom_level = action_menu_level_create(1);
    action_menu_level_add_child(s_root_level, s_custom_level, "Attendance");
    
    // Set up the secondary actions for "Attendance"
    action_menu_level_add_action(s_custom_level, "Check In", action_performed_callback,
                                 &(Context){.type=VibrationTypeCustomShort});
    
    // Create and set up the secondary level, adding it as a child to the root one
    s_custom_level = action_menu_level_create(2);
    action_menu_level_add_child(s_root_level, s_custom_level, "Grades");
    
    // Set up the next levels for "Test & Homework"
    s_second_level = action_menu_level_create(5);
    action_menu_level_add_child(s_custom_level, s_second_level, "Test/Exam");
    action_menu_level_add_child(s_custom_level, s_second_level, "Homework");
    // Set up the next levels for letter grades
    s_third_level = action_menu_level_create(2);
    action_menu_level_add_child(s_second_level, s_third_level, "A");
    action_menu_level_add_child(s_second_level, s_third_level, "B");
    action_menu_level_add_child(s_second_level, s_third_level, "C");
    action_menu_level_add_child(s_second_level, s_third_level, "D");
    action_menu_level_add_child(s_second_level, s_third_level, "F");
    s_fourth_level = action_menu_level_create(1);
    action_menu_level_add_child(s_third_level, s_fourth_level, "CONFIRM");
    action_menu_level_add_child(s_third_level, s_fourth_level, "CANCEL");
    // Create and set up the secondary level, adding it as a child to the root one
    s_custom_level = action_menu_level_create(3);
    action_menu_level_add_child(s_root_level, s_custom_level, "Extra Curricular");
    
    // Set up the secondary actions for "Extra Curricular Activities"
    action_menu_level_add_action(s_custom_level, "Basketball", action_performed_callback,
                                 &(Context){.type=VibrationTypeCustomShort});
    action_menu_level_add_action(s_custom_level, "Band", action_performed_callback,
                                 &(Context){.type=VibrationTypeCustomMedium});
    action_menu_level_add_action(s_custom_level, "Community Service", action_performed_callback,
                                 &(Context){.type=VibrationTypeCustomLong});
    
    // Create and set up the secondary level, adding it as a child to the root one
    s_custom_level = action_menu_level_create(4);
    action_menu_level_add_child(s_root_level, s_custom_level, "Stressors");
    
    // Set up the secondary actions for "Stressors"
    action_menu_level_add_action(s_custom_level, "No Stressors", action_performed_callback,
                                 &(Context){.type=VibrationTypeCustomShort});
    action_menu_level_add_action(s_custom_level, "Home Life", action_performed_callback,
                                 &(Context){.type=VibrationTypeCustomMedium});
    action_menu_level_add_action(s_custom_level, "School", action_performed_callback,
                                 &(Context){.type=VibrationTypeCustomLong});
    action_menu_level_add_action(s_custom_level, "Personal Issues", action_performed_callback,
                                 &(Context){.type=VibrationTypeCustomLong});
}

/*********************************** Clicks ***********************************/

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    // Configure the ActionMenu Window about to be shown
    ActionMenuConfig config = (ActionMenuConfig) {
        .root_level = s_root_level,
        .colors = {
            .background = GColorChromeYellow,
            .foreground = GColorBlack,
        },
        .align = ActionMenuAlignCenter
    };
    
    // Show the ActionMenu
    s_action_menu = action_menu_open(&config);
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

/******************************** Main Window *********************************/

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
    
    s_ellipsis_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ELLIPSIS);
    s_check_bitmap = gbitmap_create_with_resource(RESOURCE_ID_check);
    
    
    
    static BitmapLayer *check_bitmap_layer;
    check_bitmap_layer = bitmap_layer_create(GRect(5, 5, 48, 48));
    bitmap_layer_set_bitmap(check_bitmap_layer, s_check_bitmap);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(check_bitmap_layer));
    gbitmap_destroy(s_check_bitmap);
    bitmap_layer_destroy(check_bitmap_layer);
    
    //above is the test for the check
    
    
    
    
    s_action_bar = action_bar_layer_create();
    action_bar_layer_set_click_config_provider(s_action_bar, click_config_provider);
    action_bar_layer_set_icon(s_action_bar, BUTTON_ID_SELECT, s_ellipsis_bitmap);
    action_bar_layer_add_to_window(s_action_bar, window);
    
    s_label_layer = text_layer_create(GRect(0, 0, bounds.size.w - ACTION_BAR_WIDTH, bounds.size.h));
    text_layer_set_text(s_label_layer, "Welcome, John Smith!");
    text_layer_set_font(s_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_color(s_label_layer, GColorBlack);
    text_layer_set_background_color(s_label_layer, GColorClear);
    text_layer_set_text_alignment(s_label_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_label_layer));
}

static void window_unload(Window *window) {
    text_layer_destroy(s_label_layer);
    action_bar_layer_destroy(s_action_bar);
    gbitmap_destroy(s_ellipsis_bitmap);
    
    action_menu_hierarchy_destroy(s_root_level, NULL, NULL);
}

/************************************ App *************************************/

static void init() {
    s_main_window = window_create();
    window_set_background_color(s_main_window, GColorChromeYellow);
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    window_stack_push(s_main_window, true);
    
    init_action_menu();
}

static void deinit() {
    window_destroy(s_main_window);
}

int main() {
    init();
    app_event_loop();
    deinit();
}