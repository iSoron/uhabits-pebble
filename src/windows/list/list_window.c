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

#include <pebble.h>
#include "list_window.h"
#include "../action/action_window.h"
#include "list_layer.h"

static void on_select(void *callback_context)
{
    struct ListWindow *window = callback_context;
    LIST_WINDOW_show_action_window(window);
}

// callback: WindowHandler
static void on_load(Window *raw_window)
{
    Layer *root_layer = window_get_root_layer(raw_window);
    GRect bounds = layer_get_bounds(root_layer);
    struct ListWindow *window = window_get_user_data(raw_window);

    window->list_layer = LIST_LAYER_create(bounds);
    LIST_LAYER_attach_to_window(window->list_layer, raw_window);
    LIST_LAYER_add_to_layer(window->list_layer, root_layer);

    window->list_layer->callback_context = window;
    window->list_layer->callbacks = (struct ListLayerCallbacks) {
            .on_select = on_select
    };
}

// callback: WindowHandler
static void on_unload(Window *raw_window)
{
    struct ListWindow *list_window = window_get_user_data(raw_window);
    LIST_LAYER_destroy(list_window->list_layer);
    ACTION_WINDOW_destroy(list_window->action_window);
}

void LIST_WINDOW_show_action_window(struct ListWindow *window)
{
    ACTION_WINDOW_destroy(window->action_window);

    window->action_window = ACTION_WINDOW_create();
    if(!window->action_window) return;

    window_stack_push(window->action_window->raw_window, true);
}

struct ListWindow *LIST_WINDOW_create()
{
    WindowHandlers handlers = {
        .load = on_load,
        .unload = on_unload
    };

    struct ListWindow *list_window = 0;
    list_window = (struct ListWindow*) malloc(sizeof(struct ListWindow));
    if(!list_window) return NULL;

    Window *raw_window = window_create();
    window_set_user_data(raw_window, list_window);
    window_set_window_handlers(raw_window, handlers);
    list_window->raw_window = raw_window;

    return list_window;
}

void LIST_WINDOW_destroy(struct ListWindow *list_window)
{
    if(!list_window) return;
    window_destroy(list_window->raw_window);
    free(list_window);
}