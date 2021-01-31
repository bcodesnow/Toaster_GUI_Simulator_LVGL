#include "gui.h"
//#include "../encoder/encoder.h"
//#include "sd_card.h"
#include "x_platform.h"
#include "lv_drivers/indev/mousewheel.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void group_focus_cb(lv_group_t * group);
static void t_buttons_ev_cb(lv_obj_t * obj, lv_event_t event);
static void tabview_event_handler(lv_obj_t *tabview, lv_event_t ev);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *   t_tabs;

static lv_obj_t *   main_tab;
static lv_obj_t *   settings_tab;
static lv_obj_t *   rating_tab;

static uint8_t buttons_enabled = 1;
static lv_group_t * t_main_tab_g;
static lv_group_t*  t_settings_tab_g;
static lv_group_t*  t_rating_tab_g;

static lv_indev_drv_t enc_drv;
static lv_indev_t * enc_indev;

static lv_theme_t* theme_green;
static lv_theme_t* theme_red;

// main view objects
static lv_obj_t * t_btn_toast;
static lv_obj_t * t_label_toast;
static lv_obj_t * label_temp;
static lv_obj_t * label_temp1;
static lv_obj_t * label_temp2;
static lv_obj_t * label_temp3;
static lv_obj_t * label_smoke;
static lv_obj_t * label_scale;
static lv_obj_t * label_freq;
static lv_obj_t * label_dist_analog;
static lv_obj_t * label_aq;
static lv_obj_t * label_humi;
static lv_obj_t * spinbox_timer;
static lv_obj_t* cont_color;

// settings view objects
static lv_obj_t * t_btn_heater;
static lv_obj_t * t_btn_download_mode;
static lv_obj_t * t_btn_live_data_mode;
static lv_obj_t * t_btn_fan;
static lv_obj_t * t_btn_led;
static lv_obj_t * t_btn_lift;
static lv_obj_t * t_label_led;
static lv_obj_t * t_label_lift;
static lv_obj_t * t_label_fan;
static lv_obj_t * t_label_heater;
static lv_obj_t * t_label_download_mode;
static lv_obj_t * t_label_live_data_mode;
static lv_obj_t * t_label_wifi_info;
static lv_obj_t* label_debug_terminal;
static lv_obj_t* label_temp_cntr_settings;
static lv_obj_t* roller_temp_source_heater;
static lv_obj_t* roller_temp_cntrl_mode_heater;
static lv_obj_t* roller_temp_heater;



// rating view objects
static lv_obj_t* btn_save;
static lv_obj_t* label_save;
static lv_obj_t* label_nfo;
static lv_obj_t* roller_rating;
static lv_obj_t* roller_5secs;



///* Let lvgl know about the state of the input device */
//bool lvgl_encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
//{
//    (void) indev_drv;      /*Unused*/
//    if (drv_enc.encoder_button_pressed)
//    {
//        data->state = LV_INDEV_STATE_PR;
//        drv_enc.encoder_button_pressed = 0;
//    }
//    else
//        data->state = LV_INDEV_STATE_REL;

//    data->enc_diff = get_encoder_steps(&drv_enc);

//    return false;       /*No more data to read so return false*/
//}


static void t_timer_ev_cb(lv_obj_t * obj, lv_event_t event)
{
    static int t;
    //printf("GUI -> t_timer_ev_cb - Editing / Navigation state of the main tab group: %d \n " , lv_group_get_editing(t_main_tab_g) );
    //printf("GUI -> t_timer_ev_cb, event: %d \n", event);
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        //printf("Time Indicated Changed: %d \n",lv_spinbox_get_value(obj) );
        t = lv_spinbox_get_value(obj);
        //        global.time_indicated = t;
    }
}
static void t_buttons_ev_cb(lv_obj_t * obj, lv_event_t event)
{
    printf("Some Clicks!!");
    static int state = 0;

    //printf("GUI -> t_buttons_ev_cb %d \n", event);
    if(event == LV_EVENT_CLICKED)
    {
        if (state == 0)
            state = 1;
        else
            state = 0;

        // Clicked
        //printf("GUI -> t_buttons_ev_cb: Clicked\n");
        // Start - Stop Button
        if ((intptr_t)obj == (intptr_t)t_btn_toast)
        {

            if ( state )
            {
                lv_btn_set_state(t_btn_toast, LV_BTN_STATE_TGL_REL);
                lv_label_set_text(t_label_toast, LV_SYMBOL_STOP" Stop");
                set_gui_theme(T_GUI_RED);
                gui_set_btns_enabled( 0 );
                gui_update_heater_status(1);

            }
            else
            {
                lv_btn_set_state(t_btn_toast, LV_BTN_STATE_TGL_PR);
                lv_label_set_text(t_label_toast, T_SYM_TOAST" Start");
                set_gui_theme(T_GUI_GREEN);
                gui_set_btns_enabled( 1 );
                gui_update_heater_status(0);

            }
        }
    }

}
//static void t_buttons_ev_cb_original(lv_obj_t * obj, lv_event_t event)
//{
//    //printf("GUI -> t_buttons_ev_cb %d \n", event);
//    if(event == LV_EVENT_CLICKED)
//    {
//        // Clicked
//        //printf("GUI -> t_buttons_ev_cb: Clicked\n");
//        // Start - Stop Button
//        if ((intptr_t)obj == (intptr_t)t_btn_toast)
//        {

//            if ( global.main_state == T_IDLE )
//            {
//                lv_btn_set_state(t_btn_toast, LV_BTN_STATE_TGL_REL);
//                lv_label_set_text(t_label_toast, LV_SYMBOL_STOP" Stop");
//                daq.start_requested = 1;
//                set_gui_theme(T_GUI_RED);
//            }
//            else if ( global.main_state == T_RECORDING )
//            {
//                lv_btn_set_state(t_btn_toast, LV_BTN_STATE_TGL_PR);
//                lv_label_set_text(t_label_toast, T_SYM_TOAST" Start");
//                daq.stop_requested = 1;
//                //set_gui_theme(T_GUI_GREEN);
//            }
//        }
//        //
//        else if ((intptr_t)obj == (intptr_t)btn_save)
//        {
//            gui_change_tab(0);
//            global.settings_enable_requested = 1; // can be united with start stop rerf req?!
//            global.start_stop_button_refresh_requested = 1; //
//            char buf[32];
//            lv_roller_get_selected_str( roller_rating, &buf[0], 32 );
//            buf[1] = '\0';
//            global.class_data.rating = atoi(&buf[0]);
//            //printf("Rating: %d\n", global.class_data.rating);
//            lv_roller_get_selected_str( roller_5secs, &buf[0], 32 );
//            global.class_data.secs = atoi(&buf[0]);
//            //printf("Secs: %d\n", global.class_data.secs);
//            sd_card_add_job( SD_PROCESS_NFO_LOG, 0, NULL );

//            if ( (global.class_data.rating != PERFECT) && ( global.class_data.rating != BAD_DATA ) )
//            {
//                if ( global.class_data.rating == TOO_LATE )
//                {
//                    //printf("decrement \n");
//                    global.time_indicated -= global.class_data.secs;
//                }
//                else if ( global.class_data.rating == TOO_EARLY)
//                {
//                    //printf("increment \n");
//                    global.time_indicated += global.class_data.secs;
//                }
//                global.time_refresh_requested = 1; // this changes the indicated value
//            }
//        }
//        //
//        else
//        {
//            if ( buttons_enabled )
//            {
//                lv_btn_state_t state = LV_BTN_STATE_INA;

//                // LED Button
//                if ((intptr_t)obj == (intptr_t)t_btn_led)
//                {
//                    state = LV_BTN_STATE_TGL_PR;
//                    if ( drv_leds.led_cs_requested_state < LED_CS_STATE_MAX)
//                        drv_leds.led_cs_requested_state++;
//                    else
//                        drv_leds.led_cs_requested_state = 0;

//                    switch (drv_leds.led_cs_requested_state)
//                    {
//                    case LED_CS_STATE_OFF:
//                        lv_label_set_text(t_label_led, T_SYM_BULB_OFF" Off");
//                        state = LV_BTN_STATE_TGL_REL;
//                        break;
//                    case LED_CS_STATE_FULL_ON:
//                        lv_label_set_text(t_label_led, T_SYM_BULB_ON" On");
//                        break;
//                    case LED_CS_STATE_STROBO:
//                        lv_label_set_text(t_label_led, T_SYM_BULB_ON" Strobo");
//                        break;
//                    }
//                }
//                else if ((intptr_t)obj == (intptr_t)t_btn_fan)
//                {
//                    // FAN
//                    state = LV_BTN_STATE_TGL_PR;
//                    if ( drv_fan.curr_speed< FAN_SPEED_MAX)
//                        drv_fan.curr_speed++;
//                    else
//                        drv_fan.curr_speed = 0;
//                    fan_set_speed(drv_fan.curr_speed, FAN_MODE_SET_FADE);
//                    switch (drv_fan.curr_speed)
//                    {
//                    case FAN_SPEED_SILENT:
//                        lv_label_set_text(t_label_fan, T_SYM_FAN" Silent");
//                        break;
//                    case FAN_SPEED_FULL_ON:
//                        lv_label_set_text(t_label_fan, T_SYM_FAN" On");
//                        break;
//                    case FAN_SPEED_OFF:
//                        lv_label_set_text(t_label_fan, T_SYM_FAN" Off");
//                        state = LV_BTN_STATE_REL;
//                        break;
//                    case FAN_SPEED_DAQ:
//                        lv_label_set_text(t_label_fan, T_SYM_FAN" DAQ");
//                        break;
//                    }
//                }
//                // Heat
//                else if ((intptr_t)obj == (intptr_t)t_btn_heater)
//                {
//                    state = LV_BTN_STATE_TGL_PR;
//                    if ( drv_heater.requested_state == 1)
//                    {
//                        drv_heater.requested_state = 0;
//                        lv_label_set_text( t_label_heater, T_SYM_HEAT" On");
//                    }
//                    else
//                    {
//                        drv_heater.requested_state = 1;
//                        state = LV_BTN_STATE_REL;
//                        lv_label_set_text( t_label_heater, T_SYM_HEAT" Off");
//                    }

//                    printf("Heat Button Clicked!\n");
//                }
//                // DL MODE
//                else if ((intptr_t)obj == (intptr_t)t_btn_download_mode)
//                {
//                    state = LV_BTN_STATE_TGL_PR;
//                    if ( global.download_mode_active == 1)
//                    {
//                        printf("Stopping Download Mode! \n");
//                        global.download_mode_active = 0;
//                        lv_label_set_text(t_label_download_mode, LV_SYMBOL_DOWNLOAD" Mode Off");
//                    }
//                    else
//                    {
//                        global.download_mode_active = 1;
//                        printf("Activating Download Mode! \n");
//                        state = LV_BTN_STATE_REL;
//                        lv_label_set_text(t_label_download_mode, LV_SYMBOL_DOWNLOAD" Mode On");
//                    }

//                    printf("Download Button Clicked!\n");
//                }
//                // Live Data Mode
//                else if ((intptr_t)obj == (intptr_t)t_btn_live_data_mode)
//                {
//                    state = LV_BTN_STATE_TGL_PR;
//                    if ( global.udp_live_data_active == 1)
//                    {
//                        printf("Stopping Live Mode! \n");
//                        global.udp_live_data_active = 0;
//                        lv_label_set_text(t_label_live_data_mode, LV_SYMBOL_WIFI" Live Off");
//                    }
//                    else
//                    {
//                        if ( global.wifi_connected && global.udp_ready )
//                        {
//                            global.udp_live_data_active = 1;
//                            printf("Activating Live Mode! \n");
//                            state = LV_BTN_STATE_REL;
//                            lv_label_set_text(t_label_live_data_mode, LV_SYMBOL_WIFI" Live On");
//                        }
//                    }

//                    printf("Live Data Button Clicked!\n");
//                }
//                //Lift
//                else if ((intptr_t)obj == (intptr_t)t_btn_lift)
//                {
//                    state = LV_BTN_STATE_TGL_PR;
//#if SERVO_DIM_WITH_US == 0
//                    if ( drv_servo.curr_pos == SERVO_UP_DEG )
//                    {
//                        state = LV_BTN_STATE_REL;
//                        servo_set_pos(SERVO_DOWN_DEG);
//                        lv_label_set_text(t_label_lift, T_SYM_FORKLIFT" Lift Up");
//                    }
//                    else if ( drv_servo.curr_pos == SERVO_DOWN_DEG )
//                    {
//                        servo_set_pos(SERVO_UP_DEG);
//                        lv_label_set_text(t_label_lift, T_SYM_FORKLIFT" Lift Down");
//                    }
//#else
//                    if ( drv_servo.curr_pw == SERVO_UP_PW )
//                    {
//                        state = LV_BTN_STATE_REL;
//                        servo_set_pos(SERVO_DOWN_DEG);
//                        lv_label_set_text(t_label_lift, T_SYM_FORKLIFT" Lift Up");
//                    }
//                    else if ( drv_servo.curr_pw == SERVO_DOWN_PW )
//                    {
//                        servo_set_pos(SERVO_UP_DEG);
//                        lv_label_set_text(t_label_lift, T_SYM_FORKLIFT" Lift Down");
//                    }
//                    else
//                    {
//                        printf("!!!!!!!!!!!!!!!!!!!!    CURR PW: %d \n", drv_servo.curr_pw);
//                    }
//#endif
//                }
//                //
//                if ( ( state == LV_BTN_STATE_TGL_PR ) || ( state == LV_BTN_STATE_TGL_PR ) )
//                    lv_btn_set_state(obj, state);
//            }
//        }
//    }
//    //    else if(event == LV_EVENT_VALUE_CHANGED)
//    //    {
//    //        uint8_t btn_state = lv_btn_get_state(obj);
//    //        printf("Toggled\n");
//    //    }
//}

#define TAB_LEFT_PADDING 10
#define COLOR_BAR_WIDTH 16
#define COLOR_BAR_ANIM_TIME 250
#define COLOR_BAR_ANIM_ENABLED LV_ANIM_ON
#define COLOR_BAR_SUBSTR_MINIMA 0
static colored_bar_t tcs_bars[BAR_MAX];

void gui_update_rgbc_indic( )
{
    lv_bar_set_value( tcs_bars[BAR_R].bar, 128, COLOR_BAR_ANIM_ENABLED);
    lv_bar_set_value( tcs_bars[BAR_G].bar, 196, COLOR_BAR_ANIM_ENABLED);
    lv_bar_set_value( tcs_bars[BAR_B].bar, 88, COLOR_BAR_ANIM_ENABLED);
    lv_bar_set_value( tcs_bars[BAR_C].bar, 255, COLOR_BAR_ANIM_ENABLED);

#if ( COLOR_BAR_SUBSTR_MINIMA == 1 )
    uint8_t substr = 0;
    if ( ( cols->r <= cols->g ) && (cols->r <= cols->b) )
        substr = cols->r;
    else if ( ( cols->g <= cols->r ) && (cols->g <= cols->b) )
        substr = cols->g;
    else
        substr = cols->b;
    lv_color_t col = LV_COLOR_MAKE( ( (cols->r) - substr ), ( (cols->g) - substr ), ( (cols->b) - substr ) );
#else
    lv_color_t col = LV_COLOR_MAKE( 128, 196, 88 );
#endif
    tcs_bars[BAR_COL].style.body.main_color = col;
    tcs_bars[BAR_COL].style.body.grad_color = col;
    tcs_bars[BAR_COL].style.body.shadow.color = col;
    lv_bar_set_style(tcs_bars[BAR_COL].bar, LV_BAR_STYLE_INDIC, &tcs_bars[BAR_COL].style);
}


void create_colored_bar(lv_obj_t* parent, colored_bar_t* bars, int id, lv_color_t col, uint16_t height)
{
    bars[id].bar = lv_bar_create(parent, NULL);
    lv_obj_set_size(tcs_bars[id].bar, COLOR_BAR_WIDTH, (lv_coord_t)height);
    lv_bar_set_anim_time(tcs_bars[id].bar, COLOR_BAR_ANIM_TIME);
    lv_bar_set_range( bars[id].bar, 0, 0xFF);
    lv_bar_set_value(tcs_bars[id].bar, 255, COLOR_BAR_ANIM_ENABLED);

    tcs_bars[id].style.body.main_color = col;
    tcs_bars[id].style.body.grad_color = col;
    tcs_bars[id].style.body.shadow.color = col;
    lv_bar_set_style(tcs_bars[id].bar, LV_BAR_STYLE_INDIC, &tcs_bars[id].style);
}


void create_rgbc_indicator(lv_obj_t* parent, lv_group_t* group, uint16_t height, lv_obj_t* align_under, lv_coord_t y_offset)
{
    cont_color = lv_cont_create(parent, NULL);
    lv_obj_align(cont_color, align_under, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset );

    lv_obj_set_auto_realign(cont_color, true);                    /*Auto realign when the size changes*/
    lv_cont_set_fit(cont_color, LV_FIT_TIGHT);
    lv_cont_set_layout(cont_color, LV_LAYOUT_ROW_M);

    for (int i=0; i<BAR_MAX; i++)
        lv_style_copy(&tcs_bars[i].style, &lv_style_pretty);

    create_colored_bar(cont_color, &tcs_bars[0], BAR_R, LV_COLOR_RED, height);
    create_colored_bar(cont_color, &tcs_bars[0], BAR_G, LV_COLOR_GREEN, height);
    create_colored_bar(cont_color, &tcs_bars[0], BAR_B, LV_COLOR_BLUE, height);
    create_colored_bar(cont_color, &tcs_bars[0], BAR_C, LV_COLOR_WHITE, height);
    create_colored_bar(cont_color, &tcs_bars[0], BAR_COL, LV_COLOR_WHITE, height);


    lv_obj_t* label_color = lv_label_create(cont_color,NULL);
    lv_label_set_text(label_color, "Color");
    lv_group_add_obj(group, cont_color);
    lv_obj_set_protect(cont_color, LV_PROTECT_CLICK_FOCUS);
}

static lv_obj_t* label_sd_card;
static lv_obj_t* led_heater;
void init_main_tab(lv_obj_t* parent, lv_group_t* group)
{
    label_sd_card = lv_label_create(parent, NULL);
    lv_label_set_text(label_sd_card, LV_SYMBOL_SD_CARD "000");
    lv_obj_align(label_sd_card, NULL, LV_ALIGN_IN_TOP_RIGHT, -lv_obj_get_height(label_sd_card) / 8, lv_obj_get_height(label_sd_card) / 8);

    led_heater = lv_led_create(parent, NULL);
    lv_obj_align(led_heater, NULL, LV_ALIGN_IN_TOP_LEFT, lv_obj_get_height(label_sd_card) / 8, lv_obj_get_height(label_sd_card) / 8);
    lv_led_off(led_heater);

    t_btn_toast = lv_btn_create(parent, NULL);
    t_label_toast = lv_label_create(t_btn_toast, NULL);
    lv_label_set_text(t_label_toast, T_SYM_TOAST"Start");
    lv_obj_set_event_cb(t_btn_toast, t_buttons_ev_cb);
    lv_obj_align(t_btn_toast, NULL, LV_ALIGN_IN_TOP_MID, 0, (lv_obj_get_height( t_btn_toast )>>2));
    lv_btn_set_fit2(t_btn_toast, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_group_add_obj(group,t_btn_toast);

    lv_coord_t y_offset = (lv_obj_get_height( t_btn_toast )>>3);

    spinbox_timer = lv_spinbox_create(parent, NULL);
    lv_spinbox_set_step(spinbox_timer, 5);
    lv_spinbox_set_padding_left( spinbox_timer, 1);
    lv_obj_set_event_cb(spinbox_timer, t_timer_ev_cb);
    lv_spinbox_set_range(spinbox_timer, 0, 300);
    lv_spinbox_set_value(spinbox_timer, 150);
    lv_spinbox_set_digit_format(spinbox_timer, 4, 2, (LV_SPINBOX_EN_POS | LV_SPINBOX_MIN_N_SECS_POS) );
    lv_obj_set_width(spinbox_timer, lv_obj_get_width(t_btn_toast));
    lv_obj_align(spinbox_timer, t_btn_toast, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset);
    lv_group_add_obj(group,spinbox_timer);

    label_humi = lv_label_create(parent, NULL);
    lv_obj_align(label_humi, spinbox_timer, LV_ALIGN_OUT_BOTTOM_LEFT, TAB_LEFT_PADDING-(lv_obj_get_x(spinbox_timer)), y_offset);
    lv_label_set_text(label_humi, T_SYM_HUMI"Humidity");
    lv_group_add_obj(group, label_humi);
    lv_obj_set_protect(label_humi, LV_PROTECT_CLICK_FOCUS);

    label_temp = lv_label_create(parent, NULL);
    lv_obj_align(label_temp, label_humi, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset);
    lv_label_set_text(label_temp, T_SYM_THERMO "Temp" T_SYM_CELSIUS);
    lv_group_add_obj(group, label_temp);
    lv_obj_set_protect(label_temp, LV_PROTECT_CLICK_FOCUS);

    label_temp1 = lv_label_create(parent, NULL);
    lv_obj_align(label_temp1, label_temp, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset);
    lv_label_set_text(label_temp1, T_SYM_THERMO "Temp" T_SYM_CELSIUS"\n"); // PT1000A & PT1000B
    lv_group_add_obj(group, label_temp1);
    lv_obj_set_protect(label_temp1, LV_PROTECT_CLICK_FOCUS);

    label_temp2 = lv_label_create(parent, NULL);
    lv_obj_align(label_temp2, label_temp1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset);
    lv_label_set_text(label_temp2, T_SYM_RAILGUN "Temp" T_SYM_CELSIUS"\n"); // MLX
    lv_group_add_obj(group, label_temp2);
    lv_obj_set_protect(label_temp2, LV_PROTECT_CLICK_FOCUS);

    label_temp3 = lv_label_create(parent, NULL);
    lv_obj_align(label_temp3, label_temp2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset);
    lv_label_set_text(label_temp3, T_SYM_CHIP "Temp" T_SYM_CELSIUS"C");
    lv_group_add_obj(group, label_temp3);
    lv_obj_set_protect(label_temp3, LV_PROTECT_CLICK_FOCUS);

    label_dist_analog = lv_label_create(parent, NULL);
    lv_obj_align(label_dist_analog, label_temp3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset);
    lv_label_set_text(label_dist_analog, T_SYM_SINE "Temp" T_SYM_CELSIUS "\n\n\n\n");
    lv_group_add_obj(group, label_dist_analog);
    lv_obj_set_protect(label_dist_analog, LV_PROTECT_CLICK_FOCUS);

    label_smoke = lv_label_create(parent, NULL);
    lv_obj_align(label_smoke, label_dist_analog, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset);
    lv_label_set_text(label_smoke, T_SYM_SENS_SMOKE "Smoke\n");
    lv_group_add_obj(group, label_smoke);
    lv_obj_set_protect(label_smoke, LV_PROTECT_CLICK_FOCUS);

    label_scale = lv_label_create(parent, NULL);
    lv_obj_align(label_scale, label_smoke, LV_ALIGN_OUT_BOTTOM_LEFT, 0,y_offset);
    lv_label_set_text(label_scale, T_SYM_SCALE "Scale");
    lv_group_add_obj(group, label_scale);
    lv_obj_set_protect(label_scale, LV_PROTECT_CLICK_FOCUS);

    label_freq = lv_label_create(parent, NULL);
    lv_obj_align(label_freq, label_scale, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset);
    lv_label_set_text(label_freq, T_SYM_FREQ "Freqs\n\n");
    lv_group_add_obj(group, label_freq);
    lv_obj_set_protect(label_freq, LV_PROTECT_CLICK_FOCUS);

    label_aq = lv_label_create(parent, NULL);
    lv_obj_align(label_aq, label_freq, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset);
    lv_label_set_text(label_aq, T_SYM_LEAF "EVOC\n\n\n");
    lv_group_add_obj(group, label_aq);
    lv_obj_set_protect(label_aq, LV_PROTECT_CLICK_FOCUS);

    create_rgbc_indicator( parent, group, 64, label_aq, y_offset );
}

static void settings_roller_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        //        char buf[32];
        //        lv_roller_get_selected_str( roller_temp_heater, &buf[0], 32 );
        //        buf[4] = '\0';
        //        drv_heater.requested_temperature = atoi(&buf[0]);
        //        vol_control_req_temp_e2 =  drv_heater.requested_temperature*100;

        //        lv_roller_get_selected_str( roller_temp_source_heater, &buf[0], 32 );
        //        if ( buf[0]  == 'P')
        //            drv_heater.temp_source = HEATER_TEMP_SOURCE_PT1000;
        //        else
        //            drv_heater.temp_source = HEATER_TEMP_SOURCE_MLX_OBJ;

        //        lv_roller_get_selected_str( roller_temp_cntrl_mode_heater, &buf[0], 32 );
        //        if ( buf[0] == 'N')
        //            drv_heater.mode = HEATER_MODE_ON_OFF;
        //        else
        //            drv_heater.mode = HEATER_MODE_BANG_BANG;

        //print_heater_stats();
    }
}


void init_settings_tab(lv_obj_t* parent, lv_group_t* group)
{
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    t_btn_heater = lv_btn_create(parent, NULL);
    t_label_heater = lv_label_create(t_btn_heater, NULL);
    lv_label_set_text(t_label_heater, T_SYM_HEAT" Heater");
    lv_obj_set_event_cb(t_btn_heater, t_buttons_ev_cb);
    lv_obj_align(t_btn_heater, parent, LV_ALIGN_IN_TOP_MID, 0, (lv_obj_get_height( t_btn_heater )>>2));
    lv_btn_set_fit2(t_btn_heater, LV_FIT_TIGHT, LV_FIT_TIGHT);
    lv_group_add_obj(group, t_btn_heater);

    lv_coord_t height_offset =  lv_obj_get_height( t_btn_heater )>>1;

    t_btn_download_mode = lv_btn_create(parent, NULL);
    t_label_download_mode = lv_label_create(t_btn_download_mode, NULL);
    lv_label_set_text(t_label_download_mode, LV_SYMBOL_DOWNLOAD" DL Mode OFF");
    lv_obj_set_event_cb(t_btn_download_mode, t_buttons_ev_cb);
    lv_obj_align(t_btn_download_mode, t_btn_heater, LV_ALIGN_OUT_BOTTOM_MID, 0,  height_offset);
    lv_btn_set_fit2(t_btn_download_mode, LV_FIT_TIGHT, LV_FIT_TIGHT);
    lv_group_add_obj(group, t_btn_download_mode);

    t_btn_live_data_mode = lv_btn_create(parent, NULL);
    t_label_live_data_mode = lv_label_create(t_btn_live_data_mode, NULL);
    lv_label_set_text(t_label_live_data_mode, LV_SYMBOL_WIFI" Live OFF");
    lv_obj_set_event_cb(t_btn_live_data_mode, t_buttons_ev_cb);
    lv_obj_align(t_btn_live_data_mode, t_btn_download_mode, LV_ALIGN_OUT_BOTTOM_MID, 0,  height_offset);
    lv_btn_set_fit2(t_btn_live_data_mode, LV_FIT_TIGHT, LV_FIT_TIGHT);
    lv_group_add_obj(group, t_btn_live_data_mode);

    t_btn_fan = lv_btn_create(parent, NULL);
    t_label_fan = lv_label_create(t_btn_fan, NULL);
    lv_label_set_text(t_label_fan, T_SYM_FAN" Fan");
    lv_obj_set_event_cb(t_btn_fan, t_buttons_ev_cb);
    lv_obj_align(t_btn_fan, t_btn_live_data_mode, LV_ALIGN_OUT_BOTTOM_MID, 0,  height_offset);
    lv_btn_set_fit2(t_btn_fan, LV_FIT_TIGHT, LV_FIT_TIGHT);
    lv_group_add_obj(group, t_btn_fan);

    t_btn_led = lv_btn_create(parent, NULL);
    t_label_led = lv_label_create(t_btn_led, NULL);
    lv_label_set_text(t_label_led, T_SYM_BULB_ON" Off");
    lv_obj_set_event_cb(t_btn_led, t_buttons_ev_cb);
    lv_obj_align(t_btn_led, t_btn_fan, LV_ALIGN_OUT_BOTTOM_MID, 0, height_offset );
    lv_btn_set_fit2(t_btn_led, LV_FIT_TIGHT, LV_FIT_TIGHT);
    lv_group_add_obj(group, t_btn_led);

    t_btn_lift = lv_btn_create(parent, NULL);
    t_label_lift = lv_label_create(t_btn_lift, NULL);
    lv_label_set_text(t_label_lift, T_SYM_FORKLIFT" Lift Down");
    lv_obj_set_event_cb(t_btn_lift, t_buttons_ev_cb);
    lv_obj_align(t_btn_lift, t_btn_led, LV_ALIGN_OUT_BOTTOM_MID, 0, height_offset );
    lv_btn_set_fit2(t_btn_lift, LV_FIT_TIGHT, LV_FIT_TIGHT);
    lv_group_add_obj(group, t_btn_lift);

    t_label_wifi_info = lv_label_create(parent, NULL);
    lv_obj_align(t_label_wifi_info, t_btn_lift, LV_ALIGN_OUT_BOTTOM_LEFT, 0, height_offset);
    lv_label_set_text(t_label_wifi_info, LV_SYMBOL_WIFI "Connected / - \n 192.168.2.88 \n UDP: Ready / UDP: - \n");
    lv_group_add_obj(group, t_label_wifi_info);
    lv_obj_set_protect(t_label_wifi_info, LV_PROTECT_CLICK_FOCUS);

    label_debug_terminal = lv_label_create(parent, NULL);
    lv_obj_align(label_debug_terminal, t_label_wifi_info, LV_ALIGN_OUT_BOTTOM_LEFT, 0, height_offset);
    lv_label_set_text(label_debug_terminal, "No Messages yet!\n\n\n");
    lv_obj_set_width(label_debug_terminal, vres * 0.9f );
    lv_label_set_long_mode(label_debug_terminal, LV_LABEL_LONG_SROLL_CIRC);     /*Circular scroll*/
    lv_group_add_obj(group, label_debug_terminal);
    lv_obj_set_protect(label_debug_terminal, LV_PROTECT_CLICK_FOCUS);

    label_temp_cntr_settings = lv_label_create(parent, NULL);
    lv_label_set_text(label_temp_cntr_settings, "Temp. Control:");
    lv_obj_align(label_temp_cntr_settings, label_debug_terminal, LV_ALIGN_OUT_BOTTOM_LEFT, 0, height_offset);

    lv_group_add_obj(group, label_temp_cntr_settings);

    roller_temp_heater = lv_roller_create(parent, NULL);
    lv_roller_set_options(roller_temp_heater,
                          "145" T_SYM_CELSIUS "\n"
                                              "150" T_SYM_CELSIUS "\n"
                                                                  "155" T_SYM_CELSIUS "\n"
                                                                                      "160" T_SYM_CELSIUS "\n"
                                                                                                          "165" T_SYM_CELSIUS "\n"
                                                                                                                              "170" T_SYM_CELSIUS "\n"
                                                                                                                                                  "175" T_SYM_CELSIUS "\n"
                                                                                                                                                                      "180" T_SYM_CELSIUS "\n"
                                                                                                                                                                                          "185" T_SYM_CELSIUS "\n"
                                                                                                                                                                                                              "190" T_SYM_CELSIUS "\n"
                                                                                                                                                                                                                                  "195" T_SYM_CELSIUS "\n"
                                                                                                                                                                                                                                                      "200" T_SYM_CELSIUS,
                          LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_visible_row_count(roller_temp_heater, 1);
    lv_roller_set_anim_time(roller_temp_heater, 0);
    lv_coord_t y_offset = (lv_obj_get_height( roller_temp_heater )>>3);

    lv_obj_align(roller_temp_heater, label_temp_cntr_settings, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset);
    lv_group_add_obj(group, roller_temp_heater);
    lv_obj_set_event_cb(roller_temp_heater, settings_roller_event_handler);

#define FIRST_VAL_OF_TOASTING_TEMP_SELECTOR 145
#define GET_INIT_ID_OF_HEAT_TEMP_ROLLER() ( ( DEFAULT_TOASTING_TEMP - FIRST_VAL_OF_TOASTING_TEMP_SELECTOR ) / 5 )
    lv_roller_set_selected(roller_temp_heater, 145, LV_ANIM_OFF);

    //lv_roller_set_selected(roller_temp_heater, (global_nvs_saved.ttemp - FIRST_VAL_OF_TOASTING_TEMP_SELECTOR ) / 5, LV_ANIM_OFF);

    roller_temp_source_heater = lv_roller_create(parent, NULL);
    lv_roller_set_options(roller_temp_source_heater,
                          "MLX\n"
                          "PT1000",
                          LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_visible_row_count(roller_temp_source_heater, 1);
    lv_roller_set_anim_time(roller_temp_source_heater, 0);
    lv_obj_align(roller_temp_source_heater, roller_temp_heater, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset);
    lv_group_add_obj(group, roller_temp_source_heater);

    lv_obj_set_event_cb(roller_temp_source_heater, settings_roller_event_handler);
    //    if ( drv_heater.temp_source == HEATER_TEMP_SOURCE_MLX_OBJ)
    lv_roller_set_selected(roller_temp_source_heater, 0, LV_ANIM_OFF);
    //    else
    //        lv_roller_set_selected(roller_temp_source_heater, 1, LV_ANIM_OFF);

    roller_temp_cntrl_mode_heater = lv_roller_create(parent, NULL);
    lv_roller_set_options(roller_temp_cntrl_mode_heater,
                          "NO CONTROL\n"
                          "BANG-BANG",
                          LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_visible_row_count(roller_temp_cntrl_mode_heater, 1);
    lv_roller_set_anim_time(roller_temp_cntrl_mode_heater, 0);
    lv_obj_align(roller_temp_cntrl_mode_heater, roller_temp_source_heater, LV_ALIGN_OUT_BOTTOM_LEFT, 0, y_offset);
    lv_group_add_obj(group, roller_temp_cntrl_mode_heater);

    lv_obj_set_event_cb(roller_temp_cntrl_mode_heater, settings_roller_event_handler);
    //    if ( drv_heater.mode == HEATER_MODE_ON_OFF)
    lv_roller_set_selected(roller_temp_cntrl_mode_heater, 0, LV_ANIM_OFF);
    //    else
    //        lv_roller_set_selected(roller_temp_cntrl_mode_heater, 1, LV_ANIM_OFF);
}



void init_rating_tab(lv_obj_t* parent, lv_group_t* group)
{
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    roller_rating = lv_roller_create(parent, NULL);
    lv_roller_set_options(roller_rating,
                          "0 - Perfect!\n"
                          "1 - Too Early!\n"
                          "2 - Too Late!\n"
                          "3 - Bad Data",
                          LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_visible_row_count(roller_rating, 1);
    lv_roller_set_anim_time(roller_rating, 0);

    lv_obj_align(roller_rating, parent, LV_ALIGN_IN_TOP_MID, 0, (lv_obj_get_height( roller_rating )>>2));
    lv_group_add_obj(group, roller_rating);

    lv_coord_t y_offset = (lv_obj_get_height( roller_rating )>>3);

    roller_5secs = lv_roller_create(parent, NULL);
    lv_roller_set_options(roller_5secs,
                          "0\n"
                          "5\n"
                          "10\n"
                          "15\n"
                          "20\n"
                          "25\n"
                          "30\n"
                          "35\n"
                          "40",
                          LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_visible_row_count(roller_5secs, 1);
    lv_roller_set_anim_time(roller_5secs, 0);

    lv_obj_align(roller_5secs, roller_rating, LV_ALIGN_OUT_BOTTOM_MID, 0,  y_offset);

    lv_group_add_obj(group, roller_5secs);

    btn_save = lv_btn_create(parent, NULL);
    label_save = lv_label_create(btn_save, NULL);
    lv_label_set_text(label_save, LV_SYMBOL_SAVE" Save");
    lv_obj_set_event_cb(btn_save, t_buttons_ev_cb);
    lv_obj_align(btn_save, roller_5secs, LV_ALIGN_OUT_BOTTOM_MID, 0, y_offset );
    lv_btn_set_fit2(btn_save, LV_FIT_TIGHT, LV_FIT_TIGHT);
    lv_group_add_obj(group, btn_save);

}

static lv_group_t * enc_current_group;
// change the current "click" group behind the encoder
void encoder_input_update(lv_group_t * new_group)
{
    /* Freeze previous group */
    lv_group_focus_freeze(enc_current_group, true);
    /* Unfreeze current group */
    lv_group_focus_freeze(new_group, false);
    /* Set new group */
    lv_indev_set_group(enc_indev, new_group);
    /* Save it */
    enc_current_group = new_group;
}

static void group_focus_cb(lv_group_t * group)
{
    lv_obj_t* obj = NULL;
    if (group == t_main_tab_g)
        obj =  main_tab;
    else if (group == t_settings_tab_g )
        obj = settings_tab;
    else if ( group == t_rating_tab_g )
        obj = rating_tab;
    else
        printf("GROUP FOCUS ERROR");
    if (obj != NULL)
    {
        lv_obj_t * f = lv_group_get_focused(enc_current_group);
        if( ((intptr_t)f != (intptr_t)t_main_tab_g) && ((intptr_t)f != (intptr_t)t_settings_tab_g))
            lv_page_focus(obj, f, LV_ANIM_ON);
    }
}

// handle the group changes
static void tabview_event_handler(lv_obj_t *tabview, lv_event_t ev)
{
    //printf("!!! tabview_event_handler %d \n", lv_tabview_get_tab_act(t_tabs));
    if (LV_EVENT_VALUE_CHANGED == ev)
    {
        int tab_id;
        tab_id = lv_tabview_get_tab_act(t_tabs);
        switch(tab_id)
        {
        case 0:
            /* Put new group into the indev structure */
            encoder_input_update(t_main_tab_g); //!!!
            //            lv_group_add_obj(t_main_tab_g, t_tabs);
            break;
        case 1:
            encoder_input_update(t_settings_tab_g);
            //            lv_group_add_obj(t_settings_tab_g, t_tabs); //!!!
            break;
        case 2:
            encoder_input_update(t_rating_tab_g);
            break;
        default:
            break;
        }
    }
    else
    {
        //printf("Tabview Event Handler - 'other - %d' tabview event type! \n", ev);
    }
}

void toaster_view(void)
{
    //lv_coord_t hres = lv_disp_get_hor_res(NULL);
    //lv_coord_t vres = lv_disp_get_ver_res(NULL);

    // groups
    t_main_tab_g = lv_group_create();
    t_settings_tab_g = lv_group_create();
    t_rating_tab_g = lv_group_create();
    lv_group_set_focus_cb( t_main_tab_g, group_focus_cb);
    lv_group_set_focus_cb( t_settings_tab_g, group_focus_cb);
    lv_group_set_focus_cb( t_rating_tab_g, group_focus_cb);

    // encoder driver
    //    lv_indev_drv_init(&enc_drv);
    //    enc_drv.type = LV_INDEV_TYPE_ENCODER;
    //    enc_drv.read_cb = lvgl_encoder_read;
    //    enc_indev = lv_indev_drv_register(&enc_drv);
    //    lv_indev_set_group(enc_indev, t_main_tab_g);
    lv_indev_drv_init(&enc_drv);
    enc_drv.type = LV_INDEV_TYPE_ENCODER;
    enc_drv.read_cb = mousewheel_read;
    enc_indev = lv_indev_drv_register(&enc_drv);
    lv_indev_set_group(enc_indev, t_main_tab_g);
    enc_current_group = t_main_tab_g;

    // tabs
    t_tabs = lv_tabview_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_event_cb(t_tabs, tabview_event_handler);
    lv_tabview_set_sliding(t_tabs,true); // small bar below the active tab button

    lv_group_add_obj(t_settings_tab_g, t_tabs);
    lv_group_add_obj(t_rating_tab_g, t_tabs); // i think this will be ignored..
    lv_group_add_obj(t_main_tab_g, t_tabs); // i think this will be ignored..

    main_tab = lv_tabview_add_tab(t_tabs, T_SYM_SAMPLE);
    settings_tab = lv_tabview_add_tab(t_tabs, T_SYM_SETTINGS);
    rating_tab = lv_tabview_add_tab(t_tabs, LV_SYMBOL_EYE_OPEN);

    init_main_tab(main_tab, t_main_tab_g);
    init_settings_tab(settings_tab, t_settings_tab_g);
    init_rating_tab(rating_tab, t_rating_tab_g);

}

#define T0 20
#define T1 150
#define V0 1885
#define V1 420

float convert_lmt86lp_raw(int16_t val)
{
    float gain_val = 4.096f;
    float voltage = gain_val / 0x7fff * val * 1000;
    return ( ( voltage - V1) * (T1 - T0)  / (V1 - V0) ) + T1; // double - 4 usec, float 1 usec
    //    return (float) ( ( 10.888 - sqrt ( pow(-10.888, 2.0) + 4.0 * 0.00347 * (1777.3 - voltage) ) ) / 2.0 * (0.0-0.00347) ) + 30.0; //7usec and it does not work

}


void gui_update_sensor_data ()
{
    printf("In here!");
    uint32_t flags = 0x3FF;

#define UPDATE_PT1000 (1u<<0u)
#define UPDATE_SHT    (1u<<1u)
#define UPDATE_MLX    (1u<<2u)
#define UPDATE_NAU    (1u<<3u)
#define UPDATE_SGP    (1u<<4u)
#define UPDATE_FREQ   (1u<<5u)
#define UPDATE_ADS    (1u<<6u)
#define UPDATE_BOARD  (1u<<7u)
#define UPDATE_SMOKE  (1u<<8u)

#define LEFT_MARG "     "

    // this takes between 4 to 10ms (can still be heavily optimized, or splitted up for multiple calls)
    //int64_t pre_time = esp_timer_get_time();

    if ( flags & UPDATE_SHT )
    {

        lv_label_set_text_fmt( label_humi,  T_SYM_HUMI" SHT:%.2f%%", (float)56780/ 1000.0f );
        lv_label_set_text_fmt( label_temp,  T_SYM_THERMO" SHT:%.2f"T_SYM_CELSIUS"C", 42570 / 1000.0f );
    }
    if ( flags & UPDATE_PT1000 )
    {
        lv_label_set_text_fmt( label_temp1,  T_SYM_THERMO" PT1000A:%.1f"T_SYM_CELSIUS"C\n"T_SYM_THERMO" PT1000B:%.1f"T_SYM_CELSIUS"C", 219.5f, 87.4f );
    }
    if ( flags & UPDATE_MLX )
    {
        lv_label_set_text_fmt( label_temp2,  T_SYM_RAILGUN" Obj:%.2f"T_SYM_CELSIUS"C\n"LEFT_MARG"Amb:%.2f"T_SYM_CELSIUS"C", 178.5f, 50.0f );
    }
    if ( flags & UPDATE_NAU )
    {
        lv_label_set_text_fmt( label_scale,  T_SYM_SCALE" %d", 28765 );
    }
    if ( flags & UPDATE_SGP )
    {
        lv_label_set_text_fmt( label_aq,  T_SYM_LEAF" TVOC:%d\n"LEFT_MARG"ECO2:%d\n"LEFT_MARG"ETH:%d\n"LEFT_MARG"H2:%d", 402, 47, 13475, 12347 );
    }
    if ( flags & UPDATE_FREQ )
    {
        lv_label_set_text_fmt( label_freq,  T_SYM_FREQ" F0:%dHz\n"LEFT_MARG" F1:%dHz\n"LEFT_MARG" F2:%dHz", 78 * 100, 223* 100, 184 * 100 );
    }
    if ( flags & UPDATE_BOARD )
        lv_label_set_text_fmt(label_temp3, T_SYM_CHIP" Board:%.2f"T_SYM_CELSIUS"C", 22.7f );

    uint16_t a[4] = {14565,	14512,	14541,	14638};
    if (flags & UPDATE_ADS )
        lv_label_set_text_fmt(label_dist_analog,
                              T_SYM_THERMO"  -  "T_SYM_SINE
                              "\n0:%4.1f"T_SYM_CELSIUS"C-%4.3fV\n"
                                                      "1:%4.1f"T_SYM_CELSIUS"C-%4.3fV\n"
                                                                            "2:%4.1f"T_SYM_CELSIUS"C-%4.3fV\n"
                                                                                                  "3:%4.1f"T_SYM_CELSIUS"C-%4.3fV",
                              (float) convert_lmt86lp_raw(a[0]), (float)(4.096f / 0x7FFf * ((float)a[0])),
                (float) convert_lmt86lp_raw(a[1]),  (float)(4.096f / 0x7FFf *((float) a[1])),
                convert_lmt86lp_raw(a[2]),  4.096f / 0x7FFf * a[2],
                convert_lmt86lp_raw(a[3]),  4.096f / 0x7FFf * a[3]);
    if (flags & UPDATE_SMOKE )
        lv_label_set_text_fmt( label_smoke, T_SYM_SENS_SMOKE"UV: %.1f%%\n"LEFT_MARG"IR: %.1f%%",3.1f, 4.2f);
    //int64_t post_time = esp_timer_get_time();
    //printf("GUI SENSOR DATA UPDATE TAKES: %ld  us\n", (long)(post_time-pre_time) );

}

int32_t get_timer_value()
{
    return lv_spinbox_get_value(spinbox_timer);
}

void set_timer_value( int32_t val )
{
    lv_spinbox_set_value(spinbox_timer, val);
}

void gui_set_btns_enabled(uint8_t enabled)
{
    if (enabled)
    {
        printf("GUI -> Enable Buttons \n");
        buttons_enabled = 1;
        //        lv_btn_set_state(t_btn_fan, (drv_fan.curr_speed == FAN_SPEED_OFF) ? LV_BTN_STATE_TGL_REL : LV_BTN_STATE_TGL_PR);
        //        lv_btn_set_state(t_btn_led, (drv_leds.led_cs_current_state == LED_CS_STATE_OFF) ? LV_BTN_STATE_TGL_REL : LV_BTN_STATE_TGL_PR);
        //        lv_btn_set_state(t_btn_heater, (drv_heater.state == 0) ? LV_BTN_STATE_TGL_REL : LV_BTN_STATE_TGL_PR);
#if SERVO_DIM_WITH_US == 0
        //        lv_btn_set_state(t_btn_lift, (drv_servo.goal_pos == SERVO_UP_DEG) ? LV_BTN_STATE_TGL_REL : LV_BTN_STATE_TGL_PR);
#else
        lv_btn_set_state(t_btn_lift, (drv_servo.goal_pw == SERVO_UP_PW) ? LV_BTN_STATE_TGL_REL : LV_BTN_STATE_TGL_PR);
#endif
        lv_spinbox_set_enabled(spinbox_timer, 1);
    }
    else
    {
        printf("GUI -> Disable Buttons \n");
        buttons_enabled = 0;
        lv_btn_set_state(t_btn_fan, LV_BTN_STATE_INA);
        lv_btn_set_state(t_btn_led, LV_BTN_STATE_INA);
        lv_btn_set_state(t_btn_heater, LV_BTN_STATE_INA);
        lv_btn_set_state(t_btn_lift, LV_BTN_STATE_INA);
        lv_spinbox_set_enabled(spinbox_timer, 0);
    }
}


void gui_set_start_stop_button_state(uint8_t show_start)
{
    if ( show_start )
    {
        lv_btn_set_state(t_btn_toast, LV_BTN_STATE_TGL_REL);
        lv_label_set_text(t_label_toast, T_SYM_TOAST" Start");
        set_gui_theme(T_GUI_GREEN);
    }
    else
    {
        lv_btn_set_state(t_btn_toast, LV_BTN_STATE_TGL_PR);
        lv_label_set_text(t_label_toast, LV_SYMBOL_STOP" Stop");
    }
}

//!!This feature takes about 8kb RAM!
void set_gui_theme(uint8_t color)
{
    if ( color == T_GUI_RED )
    {
        theme_red = lv_theme_alien_init(0, NULL);
        lv_theme_set_current(theme_red);
    }
    else if ( color == T_GUI_GREEN)
    {
        theme_green = lv_theme_alien_init(120, NULL);
        lv_theme_set_current(theme_green);
    }
}

//void operate_debug_terminal()
//{
//    char* t;
//    BaseType_t rec;
//    rec = xQueueReceive( xDebugTerminalQueue, &t, 0 );
//    if (rec == pdTRUE)
//    {
//        lv_label_set_text(label_debug_terminal, t);
//        free(t);
//    }
//}

//#define MAX_DEBUG_STRING_LEN 128
//void debug_terminal_print(const char *fmt, ...)
//{
//    char* t = malloc(MAX_DEBUG_STRING_LEN);
//    va_list args;
//    va_start(args, fmt);
//    int rc = vsnprintf(t, MAX_DEBUG_STRING_LEN, fmt, args);
//    va_end(args);
//    xQueueSend( xDebugTerminalQueue, &t, NULL);
//}

void gui_update_wifi_status()
{
    if (true)
    {
        if ( true )
            lv_label_set_text_fmt(t_label_wifi_info, LV_SYMBOL_WIFI "Connected\n%s \nUDP: Ready \n" IPV4_MULTICAST_ADDR ":%d", "192.168.2.88", 5555);
        //else
        //    lv_label_set_text_fmt(t_label_wifi_info, LV_SYMBOL_WIFI "Connected\n%s \nUDP: - \n \n",  global.toaster_ip);
    }
    else
    {
        //lv_label_set_text_fmt(t_label_wifi_info, LV_SYMBOL_WIFI "Not Connected \n \n \n",  global.toaster_ip);
    }
}

//void gui_update_sd_card_status()
//{
//    if ( global.drv_sd.status & ( SD_STA_AVAIL | SD_STA_INDEX_VALID ))
//        lv_label_set_text_fmt(label_sd_card, LV_SYMBOL_SD_CARD "%03d", global.drv_sd.current_fileindex);
//    else
//        lv_label_set_text_fmt(label_sd_card, LV_SYMBOL_SD_CARD "---", global.drv_sd.current_fileindex);
//}

void gui_update_heater_status(uint8_t val)
{
    if ( val )
        lv_led_on(led_heater);
    else
        lv_led_off(led_heater);
}


void gui_change_tab(uint16_t id)
{
    lv_tabview_set_tab_act(t_tabs, id, true);
    tabview_event_handler( t_tabs ,LV_EVENT_VALUE_CHANGED);
    if ( id == 0)
    {
        lv_group_focus_obj(t_btn_toast);
    }
    else if ( id == 2)
    {
        lv_group_focus_obj(roller_rating);
    }
}
