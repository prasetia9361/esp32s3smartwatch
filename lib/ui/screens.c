#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

objects_t objects;
lv_obj_t *tick_value_change_obj;

static void event_handler_cb_main_obj0(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_LONG_PRESSED) {
        flowPropagateValue(flowState, 2, 0);
    }
}

static void event_handler_cb_main_obj6(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 13, 0);
    }
}

static void event_handler_cb_menu_1_menu_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_LONG_PRESSED_REPEAT) {
        flowPropagateValue(flowState, 1, 0);
    }
}

static void event_handler_cb_menu_1_obj7(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_PRESS_LOST) {
        flowPropagateValue(flowState, 0, 0);
    }
}

static void event_handler_cb_menu_1_button_setting_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 2, 0);
    }
}

static void event_handler_cb_menu_1_button_healty_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 4, 0);
    }
}

static void event_handler_cb_menu_1_button_audio_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 6, 0);
    }
}

static void event_handler_cb_menu_1_button_alarm_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 8, 0);
    }
}

static void event_handler_cb_setting_1_obj12(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 0, 0);
    }
}

static void event_handler_cb_setting_1_button_tampilan_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 4, 0);
    }
}

static void event_handler_cb_setting_1_obj15(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_PRESSED) {
        flowPropagateValue(flowState, 8, 0);
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            assignBooleanProperty(flowState, 8, 3, value, "Failed to assign Checked state");
        }
    }
}

static void event_handler_cb_setting_1_obj17(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_PRESSED) {
        flowPropagateValue(flowState, 12, 0);
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            bool value = lv_obj_has_state(ta, LV_STATE_CHECKED);
            assignBooleanProperty(flowState, 12, 3, value, "Failed to assign Checked state");
        }
    }
}

static void event_handler_cb_setting_1_button_date_time(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 15, 0);
    }
}

static void event_handler_cb_sub_mneu_dsiplay_1_obj20(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 0, 0);
    }
}

static void event_handler_cb_sub_mneu_dsiplay_1_button_background_1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 4, 0);
    }
}

static void event_handler_cb_sub_mneu_dsiplay_1_obj24(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_READY) {
        flowPropagateValue(flowState, 8, 0);
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_dropdown_get_selected(ta);
            if (tick_value_change_obj != ta) {
                assignIntegerProperty(flowState, 8, 4, value, "Failed to assign Selected in Dropdown widget");
            }
        }
    }
}

static void event_handler_cb_healty_1_obj27(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 0, 0);
    }
}

static void event_handler_cb_audioo_1_obj29(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 0, 0);
    }
}

static void event_handler_cb_alarm_1_obj31(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 0, 0);
    }
}

static void event_handler_cb_date_time_obj33(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_PRESSED) {
        flowPropagateValue(flowState, 0, 0);
    }
}

static void event_handler_cb_date_time_inputday(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_CLICKED) {
        flowPropagateValue(flowState, 2, 0);
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            const char *value = lv_textarea_get_text(ta);
            if (tick_value_change_obj != ta) {
                assignStringProperty(flowState, 2, 3, value, "Failed to assign Text in Textarea widget");
            }
        }
    }
}

static void event_handler_cb_date_time_inputyear(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_CLICKED) {
        flowPropagateValue(flowState, 5, 0);
    }
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            const char *value = lv_textarea_get_text(ta);
            if (tick_value_change_obj != ta) {
                assignStringProperty(flowState, 5, 3, value, "Failed to assign Text in Textarea widget");
            }
        }
    }
}

static void event_handler_cb_date_time_monthroller(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_roller_get_selected(ta);
            if (tick_value_change_obj != ta) {
                assignIntegerProperty(flowState, 7, 3, value, "Failed to assign Selected in Roller widget");
            }
        }
    }
}

static void event_handler_cb_date_time_hourroller(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_roller_get_selected(ta);
            if (tick_value_change_obj != ta) {
                assignIntegerProperty(flowState, 10, 3, value, "Failed to assign Selected in Roller widget");
            }
        }
    }
}

static void event_handler_cb_date_time_minuteroller(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_roller_get_selected(ta);
            if (tick_value_change_obj != ta) {
                assignIntegerProperty(flowState, 11, 3, value, "Failed to assign Selected in Roller widget");
            }
        }
    }
}

static void event_handler_cb_date_time_secondroller(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *ta = lv_event_get_target(e);
        if (tick_value_change_obj != ta) {
            int32_t value = lv_roller_get_selected(ta);
            if (tick_value_change_obj != ta) {
                assignIntegerProperty(flowState, 13, 3, value, "Failed to assign Selected in Roller widget");
            }
        }
    }
}

static void event_handler_cb_date_time_buttonsavetime(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_CLICKED) {
        flowPropagateValue(flowState, 15, 0);
    }
}

static void event_handler_cb_date_time_obj35(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = e->user_data;
    if (event == LV_EVENT_RELEASED) {
        flowPropagateValue(flowState, 17, 0);
    }
}

void create_screen_main() {
    void *flowState = getFlowState(0, 0);
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 368, 448);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    apply_style_submenu(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_img_create(parent_obj);
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 368, 448);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_arc_img_src(obj, &img_background1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_img_src(obj, &img_background1, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 0, 55);
            lv_obj_set_size(obj, 368, 351);
            lv_obj_add_event_cb(obj, event_handler_cb_main_obj0, LV_EVENT_ALL, flowState);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            apply_style_panel_transparan(obj);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj1 = obj;
                    lv_obj_set_pos(obj, 0, -20);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffabe02f), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &ui_font_saira_120, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj2 = obj;
                    lv_obj_set_pos(obj, -21, 110);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_text_font(obj, &ui_font_saira_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfff1f2ed), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 56, 219);
                    lv_obj_set_size(obj, 224, 84);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_set_style_radius(obj, 45, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 55, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff757e7e), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 55, 30);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Step");
                            lv_obj_set_style_text_color(obj, lv_color_hex(0xffabe02f), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &ui_font_saira_24_medium, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 55, -4);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "24365");
                            apply_style_fontcore(obj);
                        }
                        {
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            lv_obj_set_pos(obj, -5, -6);
                            lv_obj_set_size(obj, 60, 60);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                            lv_obj_set_style_radius(obj, 45, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj3 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 368, 41);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            apply_style_panel_transparan(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj4 = obj;
                    lv_obj_set_pos(obj, 292, -6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffaf5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 247, -6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "100%");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfffaf5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj5 = obj;
                    lv_obj_set_pos(obj, 224, -6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
            }
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj6 = obj;
            lv_obj_set_pos(obj, 124, 398);
            lv_obj_set_size(obj, 100, 41);
            lv_obj_add_event_cb(obj, event_handler_cb_main_obj6, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
    }
}

void tick_screen_main() {
    void *flowState = getFlowState(0, 0);
    {
        const char *new_val = evalTextProperty(flowState, 3, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj1);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj1;
            lv_label_set_text(objects.obj1, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 4, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj2);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj2;
            lv_label_set_text(objects.obj2, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 9, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.obj3, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj3;
            if (new_val) lv_obj_add_flag(objects.obj3, LV_OBJ_FLAG_HIDDEN);
            else lv_obj_clear_flag(objects.obj3, LV_OBJ_FLAG_HIDDEN);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 10, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj4);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj4;
            lv_label_set_text(objects.obj4, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 12, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.obj5, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj5;
            if (new_val) lv_obj_add_flag(objects.obj5, LV_OBJ_FLAG_HIDDEN);
            else lv_obj_clear_flag(objects.obj5, LV_OBJ_FLAG_HIDDEN);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 12, 4, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj5);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj5;
            lv_label_set_text(objects.obj5, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 13, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.obj6, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj6;
            if (new_val) lv_obj_add_flag(objects.obj6, LV_OBJ_FLAG_HIDDEN);
            else lv_obj_clear_flag(objects.obj6, LV_OBJ_FLAG_HIDDEN);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_menu_1() {
    void *flowState = getFlowState(0, 1);
    lv_obj_t *obj = lv_obj_create(0);
    objects.menu_1 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 368, 448);
    lv_obj_add_event_cb(obj, event_handler_cb_menu_1_menu_1, LV_EVENT_ALL, flowState);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    apply_style_submenu(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.obj7 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 368, 448);
            lv_obj_add_event_cb(obj, event_handler_cb_menu_1_obj7, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_arc_img_src(obj, &img_background1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_img_src(obj, &img_background1, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // buttonSetting_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_setting_1 = obj;
            lv_obj_set_pos(obj, 71, 143);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, event_handler_cb_menu_1_button_setting_1, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj8 = obj;
                    lv_obj_set_pos(obj, 18, -2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd6c0c0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // buttonHealty_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_healty_1 = obj;
            lv_obj_set_pos(obj, 197, 143);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, event_handler_cb_menu_1_button_healty_1, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj9 = obj;
                    lv_obj_set_pos(obj, 18, -2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd6c0c0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // buttonAudio_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_audio_1 = obj;
            lv_obj_set_pos(obj, 71, 256);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, event_handler_cb_menu_1_button_audio_1, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj10 = obj;
                    lv_obj_set_pos(obj, 18, -2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd6c0c0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // buttonAlarm_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_alarm_1 = obj;
            lv_obj_set_pos(obj, 197, 256);
            lv_obj_set_size(obj, 100, 50);
            lv_obj_add_event_cb(obj, event_handler_cb_menu_1_button_alarm_1, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj11 = obj;
                    lv_obj_set_pos(obj, 18, -2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffd6c0c0), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
}

void tick_screen_menu_1() {
    void *flowState = getFlowState(0, 1);
    {
        const char *new_val = evalTextProperty(flowState, 3, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj8);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj8;
            lv_label_set_text(objects.obj8, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 5, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj9);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj9;
            lv_label_set_text(objects.obj9, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 7, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj10);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj10;
            lv_label_set_text(objects.obj10, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 9, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj11);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj11;
            lv_label_set_text(objects.obj11, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_setting_1() {
    void *flowState = getFlowState(0, 2);
    lv_obj_t *obj = lv_obj_create(0);
    objects.setting_1 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 368, 448);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    apply_style_submenu(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj12 = obj;
            lv_obj_set_pos(obj, 22, 33);
            lv_obj_set_size(obj, 37, 31);
            lv_obj_add_event_cb(obj, event_handler_cb_setting_1_obj12, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj13 = obj;
                    lv_obj_set_pos(obj, -2, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 81, 36);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Setting");
            apply_style_fontcore(obj);
        }
        {
            // buttonTampilan_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_tampilan_1 = obj;
            lv_obj_set_pos(obj, 23, 297);
            lv_obj_set_size(obj, 322, 50);
            lv_obj_add_event_cb(obj, event_handler_cb_setting_1_button_tampilan_1, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            apply_style_buton_ssubmenu(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 44, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Display");
                    apply_style_fontcore(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_saira_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj14 = obj;
                    lv_obj_set_pos(obj, -1, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 23, 122);
            lv_obj_set_size(obj, 322, 50);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_radius(obj, 45, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff414446), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 155, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.obj15 = obj;
                    lv_obj_set_pos(obj, 240, -5);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_add_event_cb(obj, event_handler_cb_setting_1_obj15, LV_EVENT_ALL, flowState);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj16 = obj;
                    lv_obj_set_pos(obj, -1, -2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 44, -5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Hotspot");
                    apply_style_fontcore(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_saira_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 23, 181);
            lv_obj_set_size(obj, 322, 50);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_radius(obj, 45, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff414446), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 155, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    objects.obj17 = obj;
                    lv_obj_set_pos(obj, 240, -5);
                    lv_obj_set_size(obj, 50, 25);
                    lv_obj_add_event_cb(obj, event_handler_cb_setting_1_obj17, LV_EVENT_ALL, flowState);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj18 = obj;
                    lv_obj_set_pos(obj, -1, -2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 44, -5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "WiFi");
                    apply_style_fontcore(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_saira_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // buttonDateTime
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_date_time = obj;
            lv_obj_set_pos(obj, 23, 238);
            lv_obj_set_size(obj, 322, 50);
            lv_obj_add_event_cb(obj, event_handler_cb_setting_1_button_date_time, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            apply_style_buton_ssubmenu(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 44, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Date & Time");
                    apply_style_fontcore(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_saira_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj19 = obj;
                    lv_obj_set_pos(obj, -1, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
            }
        }
    }
}

void tick_screen_setting_1() {
    void *flowState = getFlowState(0, 2);
    {
        const char *new_val = evalTextProperty(flowState, 2, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj13);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj13;
            lv_label_set_text(objects.obj13, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 6, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj14);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj14;
            lv_label_set_text(objects.obj14, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 8, 3, "Failed to evaluate Checked state");
        bool cur_val = lv_obj_has_state(objects.obj15, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj15;
            if (new_val) lv_obj_add_state(objects.obj15, LV_STATE_CHECKED);
            else lv_obj_clear_state(objects.obj15, LV_STATE_CHECKED);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 9, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj16);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj16;
            lv_label_set_text(objects.obj16, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 12, 3, "Failed to evaluate Checked state");
        bool cur_val = lv_obj_has_state(objects.obj17, LV_STATE_CHECKED);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj17;
            if (new_val) lv_obj_add_state(objects.obj17, LV_STATE_CHECKED);
            else lv_obj_clear_state(objects.obj17, LV_STATE_CHECKED);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 13, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj18);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj18;
            lv_label_set_text(objects.obj18, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 17, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj19);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj19;
            lv_label_set_text(objects.obj19, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_sub_mneu_dsiplay_1() {
    void *flowState = getFlowState(0, 3);
    lv_obj_t *obj = lv_obj_create(0);
    objects.sub_mneu_dsiplay_1 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 368, 448);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    apply_style_submenu(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj20 = obj;
            lv_obj_set_pos(obj, 22, 33);
            lv_obj_set_size(obj, 37, 31);
            lv_obj_add_event_cb(obj, event_handler_cb_sub_mneu_dsiplay_1_obj20, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj21 = obj;
                    lv_obj_set_pos(obj, -2, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 81, 36);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Display");
            apply_style_fontcore(obj);
        }
        {
            // buttonBackground_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_background_1 = obj;
            lv_obj_set_pos(obj, 21, 129);
            lv_obj_set_size(obj, 323, 50);
            lv_obj_add_event_cb(obj, event_handler_cb_sub_mneu_dsiplay_1_button_background_1, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            apply_style_buton_ssubmenu(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj22 = obj;
                    lv_obj_set_pos(obj, 282, 7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj23 = obj;
                    lv_obj_set_pos(obj, -1, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 23, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Background");
                    apply_style_fontcore(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_saira_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    objects.obj24 = obj;
                    lv_obj_set_pos(obj, 155, -5);
                    lv_obj_set_size(obj, 150, LV_SIZE_CONTENT);
                    lv_dropdown_set_options(obj, "none\nbackgorund1\nbackground2\nbackgrounf3");
                    lv_obj_add_event_cb(obj, event_handler_cb_sub_mneu_dsiplay_1_obj24, LV_EVENT_ALL, flowState);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfff1f2ed), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // buttonSleep_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_sleep_1 = obj;
            lv_obj_set_pos(obj, 22, 188);
            lv_obj_set_size(obj, 322, 50);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            apply_style_buton_ssubmenu(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 24, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Time Sleep");
                    apply_style_fontcore(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_saira_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfff1f2ed), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    lv_obj_set_pos(obj, 239, 3);
                    lv_obj_set_size(obj, 50, 25);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj25 = obj;
                    lv_obj_set_pos(obj, -1, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
            }
        }
        {
            // buttonLock_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_lock_1 = obj;
            lv_obj_set_pos(obj, 22, 247);
            lv_obj_set_size(obj, 322, 50);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            apply_style_buton_ssubmenu(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 24, 3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Lock Theme");
                    apply_style_fontcore(obj);
                    lv_obj_set_style_text_font(obj, &ui_font_saira_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xfff1f2ed), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_switch_create(parent_obj);
                    lv_obj_set_pos(obj, 239, 3);
                    lv_obj_set_size(obj, 50, 25);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj26 = obj;
                    lv_obj_set_pos(obj, -1, 6);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
            }
        }
    }
}

void tick_screen_sub_mneu_dsiplay_1() {
    void *flowState = getFlowState(0, 3);
    {
        const char *new_val = evalTextProperty(flowState, 2, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj21);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj21;
            lv_label_set_text(objects.obj21, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 5, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.obj22, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj22;
            if (new_val) lv_obj_add_flag(objects.obj22, LV_OBJ_FLAG_HIDDEN);
            else lv_obj_clear_flag(objects.obj22, LV_OBJ_FLAG_HIDDEN);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 5, 4, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj22);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj22;
            lv_label_set_text(objects.obj22, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 6, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj23);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj23;
            lv_label_set_text(objects.obj23, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 8, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.obj24, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj24;
            if (new_val) lv_obj_add_flag(objects.obj24, LV_OBJ_FLAG_HIDDEN);
            else lv_obj_clear_flag(objects.obj24, LV_OBJ_FLAG_HIDDEN);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 8, 4, "Failed to evaluate Selected in Dropdown widget");
        int32_t cur_val = lv_dropdown_get_selected(objects.obj24);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.obj24;
            lv_dropdown_set_selected(objects.obj24, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 12, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj25);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj25;
            lv_label_set_text(objects.obj25, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 16, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj26);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj26;
            lv_label_set_text(objects.obj26, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_healty_1() {
    void *flowState = getFlowState(0, 4);
    lv_obj_t *obj = lv_obj_create(0);
    objects.healty_1 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 368, 448);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    apply_style_submenu(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj27 = obj;
            lv_obj_set_pos(obj, 22, 33);
            lv_obj_set_size(obj, 37, 31);
            lv_obj_add_event_cb(obj, event_handler_cb_healty_1_obj27, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj28 = obj;
                    lv_obj_set_pos(obj, -2, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 81, 36);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Healty");
            apply_style_fontcore(obj);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, -1, 78);
            lv_obj_set_size(obj, 368, 370);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLL_MOMENTUM);
            lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_ON);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            apply_style_panel_transparan(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                    lv_obj_set_pos(obj, 41, 20);
                    lv_obj_set_size(obj, 287, LV_SIZE_CONTENT);
                    lv_dropdown_set_options(obj, "Laki - laki\nPerempuan");
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 41, 108);
                    lv_obj_set_size(obj, 287, 41);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            lv_obj_set_pos(obj, -7, -2);
                            lv_obj_set_size(obj, 265, 10);
                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 41, 199);
                    lv_obj_set_size(obj, 287, 41);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            lv_obj_set_pos(obj, -7, -2);
                            lv_obj_set_size(obj, 265, 10);
                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    lv_obj_set_pos(obj, 41, 283);
                    lv_obj_set_size(obj, 287, 41);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            lv_obj_set_pos(obj, -7, -2);
                            lv_obj_set_size(obj, 265, 10);
                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    lv_obj_set_pos(obj, 116, 386);
                    lv_obj_set_size(obj, 100, 50);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
                }
            }
        }
    }
}

void tick_screen_healty_1() {
    void *flowState = getFlowState(0, 4);
    {
        const char *new_val = evalTextProperty(flowState, 2, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj28);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj28;
            lv_label_set_text(objects.obj28, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_audioo_1() {
    void *flowState = getFlowState(0, 5);
    lv_obj_t *obj = lv_obj_create(0);
    objects.audioo_1 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 368, 448);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    apply_style_submenu(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj29 = obj;
            lv_obj_set_pos(obj, 22, 33);
            lv_obj_set_size(obj, 37, 31);
            lv_obj_add_event_cb(obj, event_handler_cb_audioo_1_obj29, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj30 = obj;
                    lv_obj_set_pos(obj, -2, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 81, 36);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Audio");
            apply_style_fontcore(obj);
        }
    }
}

void tick_screen_audioo_1() {
    void *flowState = getFlowState(0, 5);
    {
        const char *new_val = evalTextProperty(flowState, 2, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj30);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj30;
            lv_label_set_text(objects.obj30, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_alarm_1() {
    void *flowState = getFlowState(0, 6);
    lv_obj_t *obj = lv_obj_create(0);
    objects.alarm_1 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 368, 448);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    apply_style_submenu(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj31 = obj;
            lv_obj_set_pos(obj, 22, 33);
            lv_obj_set_size(obj, 37, 31);
            lv_obj_add_event_cb(obj, event_handler_cb_alarm_1_obj31, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj32 = obj;
                    lv_obj_set_pos(obj, -2, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 81, 36);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Alarm");
            apply_style_fontcore(obj);
        }
    }
}

void tick_screen_alarm_1() {
    void *flowState = getFlowState(0, 6);
    {
        const char *new_val = evalTextProperty(flowState, 2, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj32);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj32;
            lv_label_set_text(objects.obj32, new_val);
            tick_value_change_obj = NULL;
        }
    }
}

void create_screen_date_time() {
    void *flowState = getFlowState(0, 7);
    lv_obj_t *obj = lv_obj_create(0);
    objects.date_time = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 368, 448);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    apply_style_submenu(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj33 = obj;
            lv_obj_set_pos(obj, 0, 88);
            lv_obj_set_size(obj, 368, 360);
            lv_obj_add_event_cb(obj, event_handler_cb_date_time_obj33, LV_EVENT_ALL, flowState);
            apply_style_panel_transparan(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // inputday
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.inputday = obj;
                    lv_obj_set_pos(obj, 124, 162);
                    lv_obj_set_size(obj, 173, 51);
                    lv_textarea_set_max_length(obj, 2);
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_add_event_cb(obj, event_handler_cb_date_time_inputday, LV_EVENT_ALL, flowState);
                    apply_style_styleinputtext(obj);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 7, 179);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Day");
                    apply_style_icon(obj);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 7, 98);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Month");
                    apply_style_icon(obj);
                }
                {
                    // inputyear
                    lv_obj_t *obj = lv_textarea_create(parent_obj);
                    objects.inputyear = obj;
                    lv_obj_set_pos(obj, 124, -2);
                    lv_obj_set_size(obj, 173, 51);
                    lv_textarea_set_max_length(obj, 128);
                    lv_textarea_set_one_line(obj, true);
                    lv_textarea_set_password_mode(obj, false);
                    lv_obj_add_event_cb(obj, event_handler_cb_date_time_inputyear, LV_EVENT_ALL, flowState);
                    apply_style_styleinputtext(obj);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 7, 19);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Year");
                    apply_style_icon(obj);
                }
                {
                    // monthroller
                    lv_obj_t *obj = lv_roller_create(parent_obj);
                    objects.monthroller = obj;
                    lv_obj_set_pos(obj, 124, 72);
                    lv_obj_set_size(obj, 173, 70);
                    lv_roller_set_options(obj, "Januari\nFebruari\nMaret\nApril\nMei\nJuni\nJuli\nAgustus\nSeptember\nOktober\nNovember\nDesember", LV_ROLLER_MODE_INFINITE);
                    lv_obj_add_event_cb(obj, event_handler_cb_date_time_monthroller, LV_EVENT_ALL, flowState);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 7, 248);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Hour");
                    apply_style_icon(obj);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 7, 342);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Minute");
                    apply_style_icon(obj);
                }
                {
                    // hourroller
                    lv_obj_t *obj = lv_roller_create(parent_obj);
                    objects.hourroller = obj;
                    lv_obj_set_pos(obj, 124, 231);
                    lv_obj_set_size(obj, 173, 70);
                    lv_roller_set_options(obj, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23", LV_ROLLER_MODE_INFINITE);
                    lv_obj_add_event_cb(obj, event_handler_cb_date_time_hourroller, LV_EVENT_ALL, flowState);
                }
                {
                    // minuteroller
                    lv_obj_t *obj = lv_roller_create(parent_obj);
                    objects.minuteroller = obj;
                    lv_obj_set_pos(obj, 124, 323);
                    lv_obj_set_size(obj, 173, 70);
                    lv_roller_set_options(obj, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59", LV_ROLLER_MODE_INFINITE);
                    lv_obj_add_event_cb(obj, event_handler_cb_date_time_minuteroller, LV_EVENT_ALL, flowState);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 7, 432);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Second");
                    apply_style_icon(obj);
                }
                {
                    // secondroller
                    lv_obj_t *obj = lv_roller_create(parent_obj);
                    objects.secondroller = obj;
                    lv_obj_set_pos(obj, 124, 413);
                    lv_obj_set_size(obj, 173, 70);
                    lv_roller_set_options(obj, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59", LV_ROLLER_MODE_INFINITE);
                    lv_obj_add_event_cb(obj, event_handler_cb_date_time_secondroller, LV_EVENT_ALL, flowState);
                }
            }
        }
        {
            // keyboardDay
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.keyboard_day = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 368, 200);
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_NUMBER);
        }
        {
            // buttonsavetime
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.buttonsavetime = obj;
            lv_obj_set_pos(obj, 284, 10);
            lv_obj_set_size(obj, 84, 73);
            lv_obj_add_event_cb(obj, event_handler_cb_date_time_buttonsavetime, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            apply_style_buton_ssubmenu(obj);
            lv_obj_set_style_radius(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj34 = obj;
                    lv_obj_set_pos(obj, 10, 7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_36, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj35 = obj;
            lv_obj_set_pos(obj, 22, 33);
            lv_obj_set_size(obj, 37, 31);
            lv_obj_add_event_cb(obj, event_handler_cb_date_time_obj35, LV_EVENT_ALL, flowState);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj36 = obj;
                    lv_obj_set_pos(obj, -2, -3);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "");
                    apply_style_icon(obj);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 81, 36);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Date & Time");
            apply_style_fontcore(obj);
        }
        {
            // keyboardYear
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.keyboard_year = obj;
            lv_obj_set_pos(obj, 0, 248);
            lv_obj_set_size(obj, 368, 200);
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_NUMBER);
            apply_style_keyboardstyle(obj);
        }
    }
    lv_keyboard_set_textarea(objects.keyboard_day, objects.inputday);
    lv_keyboard_set_textarea(objects.keyboard_year, objects.inputyear);
}

void tick_screen_date_time() {
    void *flowState = getFlowState(0, 7);
    {
        const char *new_val = evalTextProperty(flowState, 2, 3, "Failed to evaluate Text in Textarea widget");
        const char *cur_val = lv_textarea_get_text(objects.inputday);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.inputday;
            lv_textarea_set_text(objects.inputday, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 5, 3, "Failed to evaluate Text in Textarea widget");
        const char *cur_val = lv_textarea_get_text(objects.inputyear);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.inputyear;
            lv_textarea_set_text(objects.inputyear, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 7, 3, "Failed to evaluate Selected in Roller widget");
        int32_t cur_val = lv_roller_get_selected(objects.monthroller);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.monthroller;
            lv_roller_set_selected(objects.monthroller, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 10, 3, "Failed to evaluate Selected in Roller widget");
        int32_t cur_val = lv_roller_get_selected(objects.hourroller);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.hourroller;
            lv_roller_set_selected(objects.hourroller, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 11, 3, "Failed to evaluate Selected in Roller widget");
        int32_t cur_val = lv_roller_get_selected(objects.minuteroller);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.minuteroller;
            lv_roller_set_selected(objects.minuteroller, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        int32_t new_val = evalIntegerProperty(flowState, 13, 3, "Failed to evaluate Selected in Roller widget");
        int32_t cur_val = lv_roller_get_selected(objects.secondroller);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.secondroller;
            lv_roller_set_selected(objects.secondroller, new_val, LV_ANIM_OFF);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 14, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.keyboard_day, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.keyboard_day;
            if (new_val) lv_obj_add_flag(objects.keyboard_day, LV_OBJ_FLAG_HIDDEN);
            else lv_obj_clear_flag(objects.keyboard_day, LV_OBJ_FLAG_HIDDEN);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 16, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj34);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj34;
            lv_label_set_text(objects.obj34, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        const char *new_val = evalTextProperty(flowState, 18, 3, "Failed to evaluate Text in Label widget");
        const char *cur_val = lv_label_get_text(objects.obj36);
        if (strcmp(new_val, cur_val) != 0) {
            tick_value_change_obj = objects.obj36;
            lv_label_set_text(objects.obj36, new_val);
            tick_value_change_obj = NULL;
        }
    }
    {
        bool new_val = evalBooleanProperty(flowState, 20, 3, "Failed to evaluate Hidden flag");
        bool cur_val = lv_obj_has_flag(objects.keyboard_year, LV_OBJ_FLAG_HIDDEN);
        if (new_val != cur_val) {
            tick_value_change_obj = objects.keyboard_year;
            if (new_val) lv_obj_add_flag(objects.keyboard_year, LV_OBJ_FLAG_HIDDEN);
            else lv_obj_clear_flag(objects.keyboard_year, LV_OBJ_FLAG_HIDDEN);
            tick_value_change_obj = NULL;
        }
    }
}


void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
    create_screen_menu_1();
    create_screen_setting_1();
    create_screen_sub_mneu_dsiplay_1();
    create_screen_healty_1();
    create_screen_audioo_1();
    create_screen_alarm_1();
    create_screen_date_time();
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_menu_1,
    tick_screen_setting_1,
    tick_screen_sub_mneu_dsiplay_1,
    tick_screen_healty_1,
    tick_screen_audioo_1,
    tick_screen_alarm_1,
    tick_screen_date_time,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
