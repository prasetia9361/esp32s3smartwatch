#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *menu;
    lv_obj_t *setting;
    lv_obj_t *audioo;
    lv_obj_t *date_time;
    lv_obj_t *hasilhidrasi;
    lv_obj_t *healty;
    lv_obj_t *calculator_healty;
    lv_obj_t *alarm;
    lv_obj_t *setting_alarm;
    lv_obj_t *data_cairan_masuk;
    lv_obj_t *set_sta;
    lv_obj_t *set_ap;
    lv_obj_t *button_alarm_1;
    lv_obj_t *button_audio_1;
    lv_obj_t *button_date_time;
    lv_obj_t *button_date_time_1;
    lv_obj_t *button_healty_1;
    lv_obj_t *button_setting_1;
    lv_obj_t *buttonsavetime;
    lv_obj_t *hourroller;
    lv_obj_t *humidity_panel;
    lv_obj_t *input_total_boddy_water;
    lv_obj_t *inputama_1;
    lv_obj_t *inputbb_1;
    lv_obj_t *inputday;
    lv_obj_t *inputusia_1;
    lv_obj_t *inputyear;
    lv_obj_t *jumlah_cairan;
    lv_obj_t *keyboard_bb_1;
    lv_obj_t *keyboard_day;
    lv_obj_t *keyboard_nams_1;
    lv_obj_t *keyboard_pass;
    lv_obj_t *keyboard_pass_1;
    lv_obj_t *keyboard_ssid;
    lv_obj_t *keyboard_ssid_1;
    lv_obj_t *keyboard_usia_1;
    lv_obj_t *keyboard_year;
    lv_obj_t *minuteroller;
    lv_obj_t *monthroller;
    lv_obj_t *nama_1;
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
    lv_obj_t *obj37;
    lv_obj_t *obj38;
    lv_obj_t *obj39;
    lv_obj_t *obj4;
    lv_obj_t *obj40;
    lv_obj_t *obj41;
    lv_obj_t *obj42;
    lv_obj_t *obj43;
    lv_obj_t *obj44;
    lv_obj_t *obj45;
    lv_obj_t *obj46;
    lv_obj_t *obj47;
    lv_obj_t *obj48;
    lv_obj_t *obj49;
    lv_obj_t *obj5;
    lv_obj_t *obj50;
    lv_obj_t *obj51;
    lv_obj_t *obj52;
    lv_obj_t *obj53;
    lv_obj_t *obj54;
    lv_obj_t *obj55;
    lv_obj_t *obj56;
    lv_obj_t *obj57;
    lv_obj_t *obj58;
    lv_obj_t *obj59;
    lv_obj_t *obj6;
    lv_obj_t *obj60;
    lv_obj_t *obj61;
    lv_obj_t *obj62;
    lv_obj_t *obj63;
    lv_obj_t *obj64;
    lv_obj_t *obj65;
    lv_obj_t *obj66;
    lv_obj_t *obj67;
    lv_obj_t *obj68;
    lv_obj_t *obj69;
    lv_obj_t *obj7;
    lv_obj_t *obj70;
    lv_obj_t *obj71;
    lv_obj_t *obj72;
    lv_obj_t *obj73;
    lv_obj_t *obj74;
    lv_obj_t *obj75;
    lv_obj_t *obj76;
    lv_obj_t *obj77;
    lv_obj_t *obj78;
    lv_obj_t *obj79;
    lv_obj_t *obj8;
    lv_obj_t *obj80;
    lv_obj_t *obj81;
    lv_obj_t *obj9;
    lv_obj_t *panel1;
    lv_obj_t *panel2;
    lv_obj_t *panel3;
    lv_obj_t *pass;
    lv_obj_t *pass_1;
    lv_obj_t *rolleralarmhours;
    lv_obj_t *rolleralarmminutes;
    lv_obj_t *ssid;
    lv_obj_t *ssid_1;
    lv_obj_t *total_cairan;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_MENU = 2,
    SCREEN_ID_SETTING = 3,
    SCREEN_ID_AUDIOO = 4,
    SCREEN_ID_DATE_TIME = 5,
    SCREEN_ID_HASILHIDRASI = 6,
    SCREEN_ID_HEALTY = 7,
    SCREEN_ID_CALCULATOR_HEALTY = 8,
    SCREEN_ID_ALARM = 9,
    SCREEN_ID_SETTING_ALARM = 10,
    SCREEN_ID_DATA_CAIRAN_MASUK = 11,
    SCREEN_ID_SET_STA = 12,
    SCREEN_ID_SET_AP = 13,
};

void create_screen_main();
void tick_screen_main();

void create_screen_menu();
void tick_screen_menu();

void create_screen_setting();
void tick_screen_setting();

void create_screen_audioo();
void tick_screen_audioo();

void create_screen_date_time();
void tick_screen_date_time();

void create_screen_hasilhidrasi();
void tick_screen_hasilhidrasi();

void create_screen_healty();
void tick_screen_healty();

void create_screen_calculator_healty();
void tick_screen_calculator_healty();

void create_screen_alarm();
void tick_screen_alarm();

void create_screen_setting_alarm();
void tick_screen_setting_alarm();

void create_screen_data_cairan_masuk();
void tick_screen_data_cairan_masuk();

void create_screen_set_sta();
void tick_screen_set_sta();

void create_screen_set_ap();
void tick_screen_set_ap();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/