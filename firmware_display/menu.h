// Copyright 2025 Vinicius Fortuna
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <lvgl.h>

/**
 * @brief Finds an input device (lv_indev_t) that uses a specific driver.
 */
lv_indev_t* find_indev_by_driver(lv_indev_drv_t* target_driver) {
    if (target_driver == NULL) return NULL;
    for (lv_indev_t* indev = lv_indev_get_next(NULL); indev != NULL; indev = lv_indev_get_next(indev)) {
        if (indev->driver == target_driver) return indev;
    }
    return NULL;
}

/**
 * Page registry — populated by register_page() calls in page_selector on_boot.
 * Allows show/dismiss scripts to look up page groups by index without
 * hardcoded arrays that must be updated each time a page is added.
 */
namespace {
    lv_obj_t* g_page_objs[16] = {};
    int g_page_count = 0;
}

/**
 * Registers a page and its selector button.
 * - Assigns the next page index to selector_btn via user_data
 * - Adds selector_btn to the selector group
 * - Stores page_obj in the registry for later group lookup
 *
 * Call once per page from page_selector's on_boot, in page order.
 */
inline void register_page(lv_group_t* selector_group, lv_obj_t* page_obj, lv_obj_t* selector_btn) {
    int index = g_page_count;
    g_page_objs[g_page_count++] = page_obj;
    lv_group_add_obj(selector_group, selector_btn);
    lv_obj_set_user_data(selector_btn, (void*)(intptr_t)index);
}

/**
 * Returns the focus group for the page at the given index, or NULL if none.
 * The group is stored in the page object's user_data by each page's on_boot.
 */
inline lv_group_t* get_page_group(int index) {
    if (index < 0 || index >= g_page_count) return NULL;
    return (lv_group_t*) lv_obj_get_user_data(g_page_objs[index]);
}