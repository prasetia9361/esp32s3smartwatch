#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *menu_1;
    lv_obj_t *setting_1;
    lv_obj_t *sub_mneu_dsiplay_1;
    lv_obj_t *healty_1;
    lv_obj_t *audioo_1;
    lv_obj_t *alarm_1;
    lv_obj_t *date_time;
    lv_obj_t *button_alarm_1;
    lv_obj_t *button_audio_1;
    lv_obj_t *button_background_1;
    lv_obj_t *button_date_time;
    lv_obj_t *button_healty_1;
    lv_obj_t *button_lock_1;
    lv_obj_t *button_setting_1;
    lv_obj_t *button_sleep_1;
    lv_obj_t *button_tampilan_1;
    lv_obj_t *buttonsavetime;
    lv_obj_t *hourroller;
    lv_obj_t *inputday;
    lv_obj_t *inputyear;
    lv_obj_t *keyboard_day;
    lv_obj_t *keyboard_year;
    lv_obj_t *minuteroller;
    lv_obj_t *monthroller;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *obj17;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *obj2;
    lv_obj_t *obj20;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *obj23;
    lv_obj_t *obj24;
    lv_obj_t *obj25;
    lv_obj_t *obj26;
    lv_obj_t *obj27;
    lv_obj_t *obj28;
    lv_obj_t *obj29;
    lv_obj_t *obj3;
    lv_obj_t *obj30;
    lv_obj_t *obj31;
    lv_obj_t *obj32;
    lv_obj_t *obj33;
    lv_obj_t *obj34;
    lv_obj_t *obj35;
    lv_obj_t *obj36;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *secondroller;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_MENU_1 = 2,
    SCREEN_ID_SETTING_1 = 3,
    SCREEN_ID_SUB_MNEU_DSIPLAY_1 = 4,
    SCREEN_ID_HEALTY_1 = 5,
    SCREEN_ID_AUDIOO_1 = 6,
    SCREEN_ID_ALARM_1 = 7,
    SCREEN_ID_DATE_TIME = 8,
};

void create_screen_main();
void tick_screen_main();

void create_screen_menu_1();
void tick_screen_menu_1();

void create_screen_setting_1();
void tick_screen_setting_1();

void create_screen_sub_mneu_dsiplay_1();
void tick_screen_sub_mneu_dsiplay_1();

void create_screen_healty_1();
void tick_screen_healty_1();

void create_screen_audioo_1();
void tick_screen_audioo_1();

void create_screen_alarm_1();
void tick_screen_alarm_1();

void create_screen_date_time();
void tick_screen_date_time();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/