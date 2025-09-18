#ifndef __LV_PORT_INDEV_H__
#define __LV_PORT_INDEV_H__

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif
void lv_port_indev_init();
void lv_port_focus_init(lv_obj_t *screen);
#ifdef __cplusplus
}
#endif

#endif
