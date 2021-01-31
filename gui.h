#ifndef __esp32_toaster_gui_h
#define __esp32_toaster_gui_h

#include "lvgl/lvgl.h"
//#include "global.h"
//#include "io.h"
//#include "freertos/semphr.h"

// Custom UTF8 Glyphs for the Toaster
#define T_SYM_TOAST         "\xEF\x9F\xAC"
#define T_SYM_THERMO        "\xEF\x8B\x88"
#define T_SYM_HUMI          "\xEF\x9D\x90"
#define T_SYM_HEAT          "\xEF\xA4\x8C"
#define T_SYM_SENS_SMOKE    "\xEF\xA4\xAC"
#define T_SYM_SCALE         "\xEF\x89\x8E"
#define T_SYM_SETTINGS      "\xEF\x8F\xB2"
#define T_SYM_FAN           "\xEF\xA1\xA3"
#define T_SYM_CELSIUS       "\xC2\xBA"
#define T_SYM_PLAY          "\xEF\x85\x84"
#define T_SYM_SAMPLE        "\xEF\x93\xB0"
#define T_SYM_STOP          "\xEF\x8A\x8D"
#define T_SYM_RAILGUN       "\xEF\xA4\xA5"
#define T_SYM_FREQ          "\xEF\xA0\xBE"
#define T_SYM_FORKLIFT      "\xEF\x91\xBA"
#define T_SYM_BULB_ON       "\xEF\x99\xB2"
#define T_SYM_BULB_OFF      "\xEF\x83\xAB"
#define T_SYM_CHIP          "\xEF\x8B\x9B"
#define T_SYM_SMOG          "\xEF\x9D\x9F"
#define T_SYM_LEAF          "\xEF\x81\xAC"
#define T_SYM_WUNDERBAUM    "\xEF\x97\x90"
#define T_SYM_SINE          "\xEF\xA2\x99"

#define T_GUI_RED           1
#define T_GUI_GREEN         2



// Color Sensor Visualization
typedef enum
{
    BAR_R   = 0,
    BAR_G,
    BAR_B,
    BAR_C,
    BAR_COL,
    BAR_MAX
} color_bar_enum_t;

typedef struct
{
    lv_obj_t* bar;
    lv_style_t style;
} colored_bar_t;

//extern xQueueHandle xDebugTerminalQueue;

// Functions
extern void toaster_view(void);

extern void gui_update_rgbc_indic(  );
extern void gui_update_sensor_data();
//extern void gui_update_sd_card_status();
extern void gui_update_wifi_status();
extern void gui_update_heater_status( uint8_t val);


extern void gui_set_start_stop_button_state( uint8_t show_start);

extern void gui_set_btns_enabled( uint8_t enabled );

//extern void set_timer_value( int32_t val);
//extern int32_t get_timer_value();

extern void set_gui_theme( uint8_t color);

//extern void gui_change_tab(uint16_t id);
//extern void operate_debug_terminal();
//extern void debug_terminal_print( const char *fmt, ...);

#endif
