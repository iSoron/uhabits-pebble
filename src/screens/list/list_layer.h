/*
 * Copyright (C) 2016 Álinson Santos Xavier <isoron@gmail.com>
 *
 * This file is part of Loop Habit Tracker.
 *
 * Loop Habit Tracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Loop Habit Tracker is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <pebble.h>

struct ListLayerCallbacks
{
    void *on_select_context;

    int (*on_select)(int habit_id, void *callback_context);
};

struct ListLayer
{
    uint16_t habit_count;
    char **habit_names;
    int *habit_ids;
    int *habit_checkmarks;

    struct MenuLayer *menu_layer;
    struct ActionWindow *action_window;

    struct ListLayerCallbacks callbacks;
};

int LIST_LAYER_add_habit(struct ListLayer *layer,
                         int new_id,
                         char *new_name,
                         int new_checkmark);

int LIST_LAYER_allocate(struct ListLayer *layer, int count);

void LIST_LAYER_attach_to_window(struct ListLayer *layer, struct Window *window);

void LIST_LAYER_add_to_layer(struct ListLayer *layer, struct Layer *root_layer);

struct ListLayer* LIST_LAYER_create(struct GRect bounds);

void LIST_LAYER_destroy(struct ListLayer *layer);

int LIST_LAYER_clear(struct ListLayer *layer);