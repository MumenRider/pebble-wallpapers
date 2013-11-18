#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "resource_ids.auto.h"

#define MY_UUID { 0x7B, 0x65, 0x21, 0x66, 0x88, 0xC6, 0x42, 0xE1, 0x8E, 0x5B, 0xD3, 0xFE, 0xD2, 0xB2, 0x24, 0xDB }
PBL_APP_INFO(MY_UUID, "wallpapers", "name", 1, 0 /* App version */, RESOURCE_ID_MENU_ICON, APP_INFO_WATCH_FACE);

#define WALLPAPER_COUNT 17
const int WALLPAPER_IDS[WALLPAPER_COUNT] = {
    RESOURCE_ID_WALL_0,RESOURCE_ID_WALL_1,RESOURCE_ID_WALL_2,
    RESOURCE_ID_WALL_3,RESOURCE_ID_WALL_4,RESOURCE_ID_WALL_5,
    RESOURCE_ID_WALL_6,RESOURCE_ID_WALL_7,RESOURCE_ID_WALL_8,
    RESOURCE_ID_WALL_9,RESOURCE_ID_WALL_10,RESOURCE_ID_WALL_11,
    RESOURCE_ID_WALL_12,RESOURCE_ID_WALL_13,RESOURCE_ID_WALL_14,
    RESOURCE_ID_WALL_15,RESOURCE_ID_WALL_16
};

#define UPDATE_INTERVAL_MINUTES 5
int _minutesSinceWallpaperChange = 0;

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168

#define WALLPAPER_HEIGHT 144
#define CLOCK_HEIGHT 24

Window _window;
BmpContainer _bmpContainer;
TextLayer _dateLayer;
TextLayer _timeLayer;
BitmapLayer _wallpaperLayer;

/**
 * Change the wallpaper layer. Borrowed from sdk sample watch face.
 * https://github.com/pebble/pebblekit/blob/master/Pebble/watches/ninety_one_dub/src/ninety_one_dub.c
 */
void change_wallpaper(BmpContainer *bmp_container, const int resource_id, GPoint origin) {
    layer_remove_from_parent(&bmp_container->layer.layer);
    bmp_deinit_container(bmp_container);

    bmp_init_container(resource_id, bmp_container);

    GRect frame = layer_get_frame(&bmp_container->layer.layer);
    frame.origin.x = origin.x;
    frame.origin.y = origin.y;
    layer_set_frame(&bmp_container->layer.layer, frame);

    layer_add_child(&_wallpaperLayer.layer, &bmp_container->layer.layer);
}

/**
 * Display a random wallpaper
 */
void change_wallpaper_random() {
    srand(time(NULL));
    int resource_index = rand() % WALLPAPER_COUNT;
    change_wallpaper(&_bmpContainer,WALLPAPER_IDS[resource_index],GPoint(0, 0));
}

/**
 * This function will be called at regular intervals as specified with the
 * tick_units field.
 * ----
 * Update displayed time and conditionally change the wallpaper.
 */
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
    // TODO: Is this required?
    (void)ctx;
    
    // Update the wallpaper every update interval
    _minutesSinceWallpaperChange++;
    if(_minutesSinceWallpaperChange >= UPDATE_INTERVAL_MINUTES) {
        _minutesSinceWallpaperChange = 0;
        change_wallpaper_random();
    }

    // set the date - only changing it when the day changes
    // format strings here: http://www.gnu.org/software/emacs/manual/html_node/elisp/Time-Parsing.html
    static char dateText[] = "Sun 01 September 00";
    static int lastShownDate = -1;
    PblTm *tickTime = t->tick_time;
    int theDay = tickTime->tm_yday;
    if (theDay != lastShownDate) {
        lastShownDate = theDay;
        string_format_time(dateText, sizeof(dateText), "%a %b %e", tickTime );
        text_layer_set_text(&_dateLayer, dateText);
    }

    // set the time based off the user's 12/24h clock style preference
    static char timeText[] = "00:00";
    const char *timeFormat = clock_is_24h_style() ? "%R" : "%l:%M";
    string_format_time(timeText, sizeof(timeText), timeFormat, tickTime);
    text_layer_set_text(&_timeLayer, timeText);
    
    // TODO: Use some kind of xor composition if text layers ever get this support. See GCompOp.
    // http://developer.getpebble.com/sdkref/group___graphics_types.html#ga0d021422bd90a2a49e6c8d848e6d556f
}

/**
 * Called by the system when the app gets killed. This is a
 * good moment to free things the app has allocated earlier.
 */
void handle_deinit(AppContextRef ctx) {
    (void)ctx;
    bmp_deinit_container(&_bmpContainer);
    // fonts_unload_custom_font(no custom fonts)
}

/**
 * Sets up one text layer.
 */
void setupTextLayer(TextLayer *layer, Window *parent, int x, int y, int width, int height, 
                    GFont font, GColor colorChoice, GColor bgChoice, GTextAlignment whatAlign) {
    text_layer_init(layer, _window.layer.frame);
    text_layer_set_text_color(layer, colorChoice);
    text_layer_set_background_color(layer, bgChoice);
    text_layer_set_text_alignment(layer, whatAlign);
    layer_set_frame(&layer->layer, GRect(x, y, width, height));
    text_layer_set_font(layer, font);
    layer_add_child(&parent->layer, &layer->layer);
}

/**
 * Called by the system when the app needs to initialize. This
 * is a good moment to allocate things the app needs to use.
 */
void handle_init(AppContextRef ctx) {
    (void)ctx;

    window_init(&_window, "Simplicity");
    window_stack_push(&_window, true /* Animated */);
    window_set_background_color(&_window, GColorBlack);
    window_set_fullscreen(&_window, true);
    resource_init_current_app(&APP_RESOURCES);

    // create the bitmap layer at the back
    bitmap_layer_init(&_wallpaperLayer, GRect(0,0, SCREEN_WIDTH, WALLPAPER_HEIGHT));
    layer_add_child(&_window.layer, &_wallpaperLayer.layer);
    
    // Create date layer
    setupTextLayer(&_dateLayer, &_window, 0, WALLPAPER_HEIGHT, SCREEN_WIDTH, CLOCK_HEIGHT, 
        fonts_get_system_font(FONT_KEY_GOTHIC_18), GColorWhite, GColorBlack, GTextAlignmentLeft);

    // Create time layer
    setupTextLayer(&_timeLayer, &_window, 0, WALLPAPER_HEIGHT, SCREEN_WIDTH, CLOCK_HEIGHT, 
        fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GColorWhite, GColorClear, GTextAlignmentRight);
    
    // Set the initial wallpaper
    change_wallpaper_random();
}

/**
 * main
 */
void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,
        .deinit_handler = &handle_deinit,
        //.input_handlers
        //.messaging_info
        //.render_handler
        .tick_info = {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        }
        //.timer_handler
    };
    app_event_loop(params, &handlers);
}
