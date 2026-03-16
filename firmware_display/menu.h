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