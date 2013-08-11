#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x64, 0x8C, 0xE8, 0xC6, 0xBF, 0x52, 0x46, 0x5F, 0x95, 0x0E, 0x8F, 0xA2, 0xD8, 0x70, 0x5A, 0xB1 }

PBL_APP_INFO(MY_UUID,
             "Ball", "Black Pyramid Software",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_STANDARD_APP);

#define time_duration 100



// Function Prototypes

void draw_layer(Layer *layer, GContext *gctxt);
void config_provider(ClickConfig **config, Window *winder);
void up_single_click_handler(ClickRecognizerRef recognizer, Window *winder);
void down_single_click_handler(ClickRecognizerRef recognizer, Window *winder);
void shift_single_click_handler(ClickRecognizerRef recognizer, Window *winder);



// Globals

Window window;
AppTimerHandle timerHandle;
int pos_x, pos_y, delta_x, delta_y, old_x, old_y;
bool initialWipeFlag;



// Special Functions

void draw_layer(Layer *layer, GContext *gctxt)
{
    if (initialWipeFlag)
    {
        // Erase screen on first run
        
        graphics_context_set_fill_color(gctxt, GColorBlack);
        GRect rect = GRect(0,0,144,168);
        graphics_fill_rect(gctxt, rect, 0, GCornerNone);
        initialWipeFlag = false;
    }
    
    // Wipe old circle
    
    GPoint point = GPoint(old_x, old_y);
    graphics_context_set_fill_color(gctxt, GColorBlack);
    graphics_fill_circle(gctxt, point, 8);
    
    // Draw new circle
    
    point = GPoint(pos_x, pos_y);
    graphics_context_set_fill_color(gctxt, GColorWhite);
    graphics_fill_circle(gctxt, point, 8);
}



// Handler functions

void handle_init(AppContextRef ctxt)
{
    initialWipeFlag = true;
    
    window_init(&window, "Ball");
    window_set_background_color(&window, GColorBlack);
    window_set_fullscreen(&window, true);
    window_stack_push(&window, false);
    
    Layer *root = window_get_root_layer(&window);
    layer_set_update_proc(root, draw_layer);
    
    layer_mark_dirty(root);
    
    // Set up button monitoring DO AFTER PUSHING WINDOW TO STACK
    
    window_set_click_config_provider(&window, (ClickConfigProvider)config_provider);
    
    srand(time(NULL));
    
    pos_x = 68 + rand() % 8;
    pos_y = 80 + rand() % 8;
    
    delta_x = 8;
    delta_y = 8;
    
    old_x = 0;
    old_y = 0;
    
    // Set up timer
    
    timerHandle = app_timer_send_event(ctxt, 500, 1);
}



void handle_deinit(AppContextRef ctxt)

{
    app_timer_cancel_event(ctxt, timerHandle);
    
}



// Click configuration functions

void config_provider(ClickConfig **config, Window *winder)
{
    config[BUTTON_ID_UP]->click.handler = (ClickHandler)up_single_click_handler;
    config[BUTTON_ID_DOWN]->click.handler = (ClickHandler)down_single_click_handler;
    config[BUTTON_ID_SELECT]->click.handler = (ClickHandler)shift_single_click_handler;
}



void up_single_click_handler(ClickRecognizerRef recognizer, Window *winder)
{
    delta_x = delta_x * -1;
}



void down_single_click_handler(ClickRecognizerRef recognizer, Window *winder)
{
    delta_y = delta_y * -1;
}



void shift_single_click_handler(ClickRecognizerRef recognizer, Window *winder)
{
    old_x = pos_x;
    old_y = pos_y;
    
    pos_x = rand() % 140;
    pos_y = rand() % 160;
    
    // Tell layer to redraw
    
    Layer *root = window_get_root_layer(&window);
    layer_mark_dirty(root);
}



// Event handlers

void handle_timer(AppContextRef ctxt, AppTimerHandle handle, uint32_t cookie)
{
    // vibes_short_pulse();
    
    old_x = pos_x;
    old_y = pos_y;
    
    pos_x = pos_x + delta_x;
    pos_y = pos_y + delta_y;
    
    if (pos_x > 140)
    {
        pos_x = 132;
        delta_x = -8;
        vibes_short_pulse();
    }
    
    if (pos_x < 4)
    {
        pos_x = 12;
        delta_x = 8;
        vibes_short_pulse();
    }
    
    if (pos_y > 162)
    {
        pos_y = 154;
        delta_y = -8;
        vibes_short_pulse();
    }
    
    if (pos_y < 4)
    {
        pos_y = 12;
        delta_y = 8;
        vibes_short_pulse();
    }
    
    // Reset timer
    
    timerHandle = app_timer_send_event(ctxt, time_duration, 1);
    
    // Tell layer to redraw
    
    Layer *root = window_get_root_layer(&window);
    layer_mark_dirty(root);
}


void handle_tick(AppContextRef ctxt, PebbleTickEvent *event)
{
    pos_x = pos_x + delta_x;
    pos_y = pos_y + delta_y;
    
    if (pos_x > 140)
    {
        pos_x = 132;
        delta_x = -8;
    }
    
    if (pos_x < 4)
    {
        pos_x = 12;
        delta_x = 8;
    }
    
    if (pos_y > 162)
    {
        pos_y = 154;
        delta_y = -8;
    }
    
    if (pos_y < 4)
    {
        pos_y = 12;
        delta_y = 8;
    }
    
    Layer *root = window_get_root_layer(&window);
    layer_mark_dirty(root);
}



void pbl_main(void *params)
{
  
    AppContextRef ctxt = (AppContextRef)params;
    
    PebbleAppHandlers handlers =
    {
        .init_handler = &handle_init,
        .deinit_handler = &handle_deinit,
        .timer_handler = &handle_timer
    };
  
    app_event_loop(ctxt, &handlers);
}
