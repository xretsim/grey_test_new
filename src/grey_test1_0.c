#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x55, 0xED, 0xC2, 0x2E, 0x11, 0x4A, 0x4C, 0xD3, 0xB4, 0xDF, 0x53, 0x84, 0xC5, 0xA8, 0x81, 0x50 }
PBL_APP_INFO(MY_UUID,
             "Grey Test", "Glenn Loos-Austin",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window _window;

AppContextRef _storedctx;
AppTimerHandle _timer_handle;

int _base_interval = 15;
int _currentShown = 0;
bool _isPlaying = false;

BmpContainer _greyBmp[3];


void remove_container(BmpContainer *bmp_container) {
    layer_remove_from_parent(&bmp_container->layer.layer);            //remove it from layer so it can be safely deinited
    bmp_deinit_container(bmp_container);                              //deinit the old image.
}

void set_container_image(BmpContainer *bmp_container, const int resource_id, GPoint origin, Layer *targetLayer) {
    
    remove_container(bmp_container);
    
    bmp_init_container(resource_id, bmp_container);                   //init the container with the new image
    
    GRect frame = layer_get_frame(&bmp_container->layer.layer);       //posiiton the new image with the supplied coordinates.
    frame.origin.x = origin.x;
    frame.origin.y = origin.y;
    layer_set_frame(&bmp_container->layer.layer, frame);
    
    layer_add_child(targetLayer, &bmp_container->layer.layer);        //add the new image to the target layer.
}


void up_click_handler(ClickRecognizerRef recognizer, Window *window) {
    //_base_interval -= 5;
    if(_base_interval<1) _base_interval = 1;
}
void down_click_handler(ClickRecognizerRef recognizer, Window *window) {
    //_base_interval += 1;
}
void select_click_handler(ClickRecognizerRef recognizer, Window *window) {
    _isPlaying = !_isPlaying;
    if(_isPlaying) {
        layer_set_hidden(&_greyBmp[0].layer.layer, true);
        _timer_handle = app_timer_send_event(_storedctx, _base_interval /* milliseconds */, 0);
    } else
    {
        layer_set_hidden(&_greyBmp[_currentShown].layer.layer, true);
        layer_set_hidden(&_greyBmp[0].layer.layer, false);
    }
}

void main_config_provider(ClickConfig **config, Window *window) {
    config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_click_handler;
    config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_click_handler;
    config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_click_handler;
    
    (void)window;
}


void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t ignored) {
    layer_set_hidden(&_greyBmp[_currentShown].layer.layer, true);
    _currentShown++;
    if(_currentShown==3) _currentShown=1;
    layer_set_hidden(&_greyBmp[_currentShown].layer.layer, false);

     if(_isPlaying) {
         _timer_handle = app_timer_send_event(ctx, _base_interval /* milliseconds */, 0);
     }

}

void handle_deinit(AppContextRef ctx) {
    (void)ctx;
    bmp_deinit_container(&_greyBmp[0]);
    bmp_deinit_container(&_greyBmp[1]);
    bmp_deinit_container(&_greyBmp[2]);
}

void handle_init(AppContextRef ctx) {
    (void)ctx;
    _storedctx = ctx;

    window_init(&_window, "Window Name");
    window_stack_push(&_window, true /* Animated */);
    window_set_fullscreen(&_window, true);

    resource_init_current_app(&APP_RESOURCES);
    
     window_set_click_config_provider(&_window, (ClickConfigProvider) main_config_provider);
    
    set_container_image(&_greyBmp[1], RESOURCE_ID_GIRLB_1, GPoint(0,0), &_window.layer);
    layer_set_hidden(&_greyBmp[1].layer.layer, true);

    set_container_image(&_greyBmp[2], RESOURCE_ID_GIRLB_2, GPoint(0,0), &_window.layer);
    layer_set_hidden(&_greyBmp[2].layer.layer, true);

    set_container_image(&_greyBmp[0], RESOURCE_ID_GIRLB_BASE, GPoint(0,0), &_window.layer);
    
    //_timer_handle = app_timer_send_event(ctx, _base_interval /* milliseconds */, 0);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
    .timer_handler = &handle_timer
  };
  app_event_loop(params, &handlers);
}
