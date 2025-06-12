// Copyright 2025 Vinicius Fortuna
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <string>
#include <vector>
#include <lvgl.h>

/**
 * @brief Recursively assigns an LVGL object and all its children to a specified group.
 *
 * This function first adds the given object to the group. Then, it iterates
 * through all children of the object and calls itself recursively for each child,
 * effectively adding the entire widget tree rooted at 'obj' to the 'group'.
 *
 * @param obj The LVGL object (widget) to start the recursive assignment from.
 *            If NULL, the function does nothing.
 * @param group The LVGL group to which the object and its children will be assigned.
 *              If NULL, the function does nothing.
 */
void recursive_set_group(lv_obj_t* obj, lv_group_t* group) {
    if (obj == NULL || group == NULL) {
        // Safety check: do nothing if either object or group is null.
        return;
    }

    // Assign the current object to the group.
    if (lv_obj_get_group(obj) == lv_group_get_default()) {
      ESP_LOGD("lvgl", "obj %p group: %p -> %p", obj, lv_obj_get_group(obj), group);
        // Only add the object to the group if it is not already part of it.
        // This prevents adding the same object multiple times.
      lv_group_add_obj(group, obj);
    }

    // Recursively assign children to the group.
    uint32_t child_count = lv_obj_get_child_cnt(obj);
    for (uint32_t i = 0; i < child_count; ++i) {
        lv_obj_t* child = lv_obj_get_child(obj, i);
        // The recursive call's initial check handles if child is NULL.
        recursive_set_group(child, group);
    }
}

/**
 * @brief Finds an input device (lv_indev_t) that uses a specific driver.
 *
 * This function iterates through all registered LVGL input devices using
 * lv_indev_get_next(). For each input device, it compares its driver
 * with the provided target_driver.
 *
 * @param target_driver A pointer to the lv_indev_drv_t driver to search for.
 * @return A pointer to the matching lv_indev_t if found, otherwise NULL.
 */
lv_indev_t* find_indev_by_driver(lv_indev_drv_t* target_driver) {
    if (target_driver == NULL) {
        // If the target driver is NULL, we can't find a match.
        return NULL;
    }
    for (lv_indev_t* current_indev = lv_indev_get_next(NULL);
         current_indev != NULL;
         current_indev = lv_indev_get_next(current_indev)) {
        // Check if the current input device's driver matches the target driver
        if (current_indev->driver == target_driver) {
            return current_indev; // Found a match
        }
    }
    // No matching input device was found
    return NULL;
}

/**
 * @brief Retrieves the i-th LVGL object from a group using _lv_ll_get_next.
 *
 * This function accesses the internal linked list of objects within the
 * specified LVGL group and returns the object at the given index by
 * iterating through the list.
 *
 * @param group A pointer to the lv_group_t from which to get the object.
 * @param index The 0-based index of the object to retrieve.
 * @return A pointer to the lv_obj_t at the specified index if found and
 *         the index is valid, otherwise NULL.
 */
lv_obj_t* get_group_item_by_index(lv_group_t* group, int index) {
    if (group == NULL) {
        // Safety check: group is null
        return NULL;
    }
    if (index < 0) {
        // Safety check: index is out of bounds
        return NULL;
    }

    // Access the internal linked list of objects
    lv_ll_t* obj_ll = &group->obj_ll;

    // Get the head of the linked list
    void* current_obj = _lv_ll_get_head(obj_ll);

    // Iterate to the i-th item
    for (int i = 0; i < index; ++i) {
        if (current_obj == NULL) {
            // Should not happen if index is within bounds, but as a safeguard
            return NULL;
        }
        current_obj = _lv_ll_get_next(obj_ll, current_obj);
    }

    return *((lv_obj_t**)current_obj);
}

/**
 * @brief Retrieves the index of the focused LVGL object within a group.
 *
 * This function iterates through the objects in the specified LVGL group
 * and returns the 0-based index of the object that currently has focus.
 *
 * @param group A pointer to the lv_group_t to search within.
 * @return The 0-based index of the focused object if found.
 *         Returns -1 if the group is NULL, no object is focused,
 *         or the focused object is not found within the group's list.
 */
int get_focused_item_index_in_group(lv_group_t* group) {
    if (group == NULL) {
        return 0; // Safety check: group is null
    }
    lv_obj_t* focused_obj = lv_group_get_focused(group);
    if (focused_obj == NULL) {
        return 0; // No object is currently focused in this group
    }

    lv_ll_t* obj_ll = &group->obj_ll;
    void* obj_p;
    int index = 0;
    _LV_LL_READ(obj_ll, obj_p) {
        if (*(lv_obj_t**)obj_p == focused_obj) {
            return index;
        }
        index++;
    }

    return 0; // Focused object not found in the group's list (should not happen in normal operation)
}