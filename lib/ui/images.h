#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_background1;
extern const lv_img_dsc_t img_sepatu_speed;
extern const lv_img_dsc_t img_menu_tombol;
extern const lv_img_dsc_t img_menu_audio;
extern const lv_img_dsc_t img_menu_setting;
extern const lv_img_dsc_t img_menu_healthy;
extern const lv_img_dsc_t img_alarm;
extern const lv_img_dsc_t img_hotspot;
extern const lv_img_dsc_t img_date_time;
extern const lv_img_dsc_t img_display;
extern const lv_img_dsc_t img_kalkulator;
extern const lv_img_dsc_t img_riwayat;
extern const lv_img_dsc_t img_step_counter;
extern const lv_img_dsc_t img_background;
extern const lv_img_dsc_t img_time_sleepp;
extern const lv_img_dsc_t img_lock_theme;
extern const lv_img_dsc_t img_nama;
extern const lv_img_dsc_t img_usia;
extern const lv_img_dsc_t img_berat_badan;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[19];


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/