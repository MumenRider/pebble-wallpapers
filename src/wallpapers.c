#include <pebble.h>
#include <time.h>
#include "gbitmap_color_palette_manipulator.h"

#define WALLPAPER_COUNT 17
const int WALLPAPER_IDS[WALLPAPER_COUNT] = {
    RESOURCE_ID_WALL_0,RESOURCE_ID_WALL_1,RESOURCE_ID_WALL_2,
    RESOURCE_ID_WALL_3,RESOURCE_ID_WALL_4,RESOURCE_ID_WALL_5,
    RESOURCE_ID_WALL_6,RESOURCE_ID_WALL_7,RESOURCE_ID_WALL_8,
    RESOURCE_ID_WALL_9,RESOURCE_ID_WALL_10,RESOURCE_ID_WALL_11,
    RESOURCE_ID_WALL_12,RESOURCE_ID_WALL_13,RESOURCE_ID_WALL_14,
    RESOURCE_ID_WALL_15,RESOURCE_ID_WALL_16
};

#define UPDATE_INTERVAL_MINUTES 3
int _minutesSinceWallpaperChange = UPDATE_INTERVAL_MINUTES;

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168

#define WALLPAPER_HEIGHT 144
#define CLOCK_HEIGHT 24

Window* _window;
GBitmap* _bmp;
bool _bmpCreated = false;
TextLayer* _dateLayer;
TextLayer* _timeLayer;
BitmapLayer* _wallpaperLayer;


void change_wallpaper(const int resource_id, GColor whiteSwapColor, GColor blackSwapColor) {
    if(_bmpCreated) {
      gbitmap_destroy(_bmp);
      _bmpCreated = false;
    }

    _bmp = gbitmap_create_with_resource(resource_id);
    _bmpCreated = true;

    #ifdef PBL_COLOR
      // Compositing mode could be used to change either black or white to another color.
      //bitmap_layer_set_background_color(_wallpaperLayer, GColorRed);
      //bitmap_layer_set_compositing_mode(_wallpaperLayer, GCompOpSet);

      // Perform palette-based color replacement on the indexed black/white image
      GBitmap* bmp = gbitmap_create_palettized_from_1bit(_bmp);
      gbitmap_destroy(_bmp);
      _bmp = bmp;
      if (gcolor_equal(GColorWhite, blackSwapColor) && gcolor_equal(GColorBlack, whiteSwapColor)) {
        // If we're swapping white for black and black for white, then we need to change one of the
        // colors to a different color or the entire bitmap will become one color.
        replace_gbitmap_color(GColorWhite, GColorRed, _bmp, _wallpaperLayer);
        replace_gbitmap_color(GColorBlack, GColorWhite, _bmp, _wallpaperLayer);
        replace_gbitmap_color(GColorRed, GColorBlack, _bmp, _wallpaperLayer);
      } else {
        if (!gcolor_equal(GColorWhite, whiteSwapColor)) {
          replace_gbitmap_color(GColorWhite, whiteSwapColor, _bmp, _wallpaperLayer);
        }
        if (!gcolor_equal(GColorBlack, blackSwapColor)) {
          replace_gbitmap_color(GColorBlack, blackSwapColor, _bmp, _wallpaperLayer);
        }
      }
    #endif

    bitmap_layer_set_bitmap(_wallpaperLayer, _bmp);
    layer_add_child(window_get_root_layer(_window), bitmap_layer_get_layer(_wallpaperLayer));
}

/**
 * Display a random wallpaper
 */
void change_wallpaper_random() {
    srand(time(NULL));
    int resource_index = rand() % WALLPAPER_COUNT;
    GColor whiteSwapColor = GColorWhite;
    GColor blackSwapColor = GColorBlack;
    switch (resource_index) {
      case 0: // android, green A4CA39
        whiteSwapColor = GColorGreen;
        blackSwapColor = GColorWhite;
        break;
      case 1: // berserk
        whiteSwapColor = GColorRed;
        break;
      case 2: // black_knights
        // NO-OP
        break;
      case 3: // clare
        whiteSwapColor = GColorBlack;
        blackSwapColor = GColorWhite;
        break;
      case 4: // dragon1
        whiteSwapColor = GColorDarkCandyAppleRed;
        blackSwapColor = GColorWhite;
        break;
      case 5: // dragon3
        // NO-OP
        break;
      case 6: // drill
        // NO-OP
        break;
      case 7: // fairy_tale
        whiteSwapColor = GColorRed;
        break;
      case 8: // frozen_flame
        whiteSwapColor = GColorOrange;
        break;
      case 9: // lxx
        whiteSwapColor = GColorLightGray;
        blackSwapColor = GColorImperialPurple;
        break;
      case 10: // neverwinter
        whiteSwapColor = GColorBabyBlueEyes;
        break;
      case 11: // octocat
        // NO-OP
        break;
      case 12: // pentagon_skull
        whiteSwapColor = GColorBlack;
        blackSwapColor = GColorWhite;
        break;
      case 13: // pipboy
        whiteSwapColor = GColorGreen;
        break;
      case 14: // sac
        // NO-OP
        break;
      case 15: // vault
        whiteSwapColor = GColorDarkCandyAppleRed;
        blackSwapColor = GColorIcterine;
        break;
      case 16: // yoko_skull
        // NO-OP
        break;
    }
    change_wallpaper(WALLPAPER_IDS[resource_index], whiteSwapColor, blackSwapColor);
}

/**
 * This function will be called at regular intervals as specified with the
 * tick_units field.
 * ----
 * Update displayed time and conditionally change the wallpaper.
 */
void handle_minute_tick(struct tm* time, TimeUnits units_changed) {
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
    int theDay = time->tm_yday;
    if (theDay != lastShownDate) {
        lastShownDate = theDay;
        strftime(dateText, sizeof(dateText), "%a %b %e", time);
        text_layer_set_text(_dateLayer, dateText);
    }

    // set the time based off the user's 12/24h clock style preference
    static char timeText[] = "00:00";
    const char *timeFormat = clock_is_24h_style() ? "%R" : "%l:%M";
    strftime(timeText, sizeof(timeText), timeFormat, time);
    text_layer_set_text(_timeLayer, timeText);

    // TODO: Use some kind of xor composition if text layers ever get this support. See GCompOp.
    // http://developer.getpebble.com/sdkref/group___graphics_types.html#ga0d021422bd90a2a49e6c8d848e6d556f
}

/**
 * Called by the system when the app gets killed. This is a
 * good moment to free things the app has allocated earlier.
 */
void deinit(void) {
    tick_timer_service_unsubscribe();
    bitmap_layer_destroy(_wallpaperLayer);
    if(_bmpCreated) {
      gbitmap_destroy(_bmp);
      _bmpCreated = false;
    }
    text_layer_destroy(_dateLayer);
    text_layer_destroy(_timeLayer);
    // fonts_unload_custom_font(no custom fonts)
    window_destroy(_window);
}

/**
 * Sets up one text layer.
 */
void setupTextLayer(TextLayer** layer, int x, int y, int width, int height, 
                    GFont font, GColor colorChoice, GColor bgChoice, GTextAlignment whatAlign) {
    *layer = text_layer_create(GRect(x, y, width, height));
    text_layer_set_text_color(*layer, colorChoice);
    text_layer_set_background_color(*layer, bgChoice);
    text_layer_set_text_alignment(*layer, whatAlign);
    text_layer_set_font(*layer, font);
    layer_add_child(window_get_root_layer(_window), text_layer_get_layer(*layer));
}

/**
 * Called by the system when the app needs to initialize. This
 * is a good moment to allocate things the app needs to use.
 */
void init(void) {
    _window = window_create();
    window_stack_push(_window, true /* Animated */);
    window_set_background_color(_window, GColorBlack);

    #ifdef PBL_SDK_2
    window_set_fullscreen(_window, true);
    #endif

    // create the bitmap layer at the back
    _wallpaperLayer = bitmap_layer_create(GRect(0,0, SCREEN_WIDTH, WALLPAPER_HEIGHT));
    layer_add_child(window_get_root_layer(_window), bitmap_layer_get_layer(_wallpaperLayer));

    // Create date layer
    setupTextLayer(&_dateLayer, 0, WALLPAPER_HEIGHT, SCREEN_WIDTH, CLOCK_HEIGHT, 
        fonts_get_system_font(FONT_KEY_GOTHIC_18), GColorWhite, GColorBlack, GTextAlignmentLeft);

    // Create time layer
    setupTextLayer(&_timeLayer, 0, WALLPAPER_HEIGHT, SCREEN_WIDTH, CLOCK_HEIGHT, 
        fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GColorWhite, GColorClear, GTextAlignmentRight);

    // Subscribe to time event
    tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

/**
 * main
 */
int main(void) {
   init();
   app_event_loop();
   deinit();
}
