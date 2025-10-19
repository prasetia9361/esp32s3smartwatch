#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void apply_style_panel_transparan(lv_obj_t *obj);
extern void apply_style_icon(lv_obj_t *obj);
extern void apply_style_fontcore(lv_obj_t *obj);
extern void apply_style_submenu(lv_obj_t *obj);
extern void apply_style_buton_ssubmenu(lv_obj_t *obj);
extern void apply_style_clengkuengstyle(lv_obj_t *obj);
extern void apply_style_styleinputtext(lv_obj_t *obj);
extern void apply_style_keyboardstyle(lv_obj_t *obj);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/