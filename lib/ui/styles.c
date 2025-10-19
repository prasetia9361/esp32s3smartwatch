#include "styles.h"
#include "images.h"
#include "fonts.h"

void apply_style_panel_transparan(lv_obj_t *obj) {
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_icon(lv_obj_t *obj) {
    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffaf5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_fontcore(lv_obj_t *obj) {
    lv_obj_set_style_text_color(obj, lv_color_hex(0xfff1f2ed), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &ui_font_saira_36, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_submenu(lv_obj_t *obj) {
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff070707), LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_buton_ssubmenu(lv_obj_t *obj) {
    lv_obj_set_style_radius(obj, 145, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff414446), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 155, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_clengkuengstyle(lv_obj_t *obj) {
    lv_obj_set_style_radius(obj, 45, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(obj, true, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_blend_mode(obj, LV_BLEND_MODE_ADDITIVE, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_styleinputtext(lv_obj_t *obj) {
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
};
void apply_style_keyboardstyle(lv_obj_t *obj) {
    lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
};

