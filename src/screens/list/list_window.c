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
#include "../../network.h"
#include "../../util.h"

// NETWORK CALLBACKS
// -----------------------------------------------------------------------------
int on_habit(int id, char *name, int checkmark, void *callback_context)
{
    int rval = 0;
    struct ListWindow *window = callback_context;

    rval = LIST_LAYER_add_habit(window->list_layer, id, name, checkmark);
    abort_if(rval, "LIST_LAYER_add_habit failed");

CLEANUP:
    return rval;
}

int on_count(int count, void *callback_context)
{
    int rval = 0;
    struct ListWindow *window = callback_context;

    rval = LIST_LAYER_allocate(window->list_layer, count);
    abort_if(rval, "LIST_LAYER_allocate failed");

CLEANUP:
    return rval;
}

// BUTTON CALLBACKS
// -----------------------------------------------------------------------------
static int on_select(int habit_id, void *callback_context)
{
    int rval = 0;
    struct ListWindow *window = callback_context;

    ACTION_WINDOW_destroy(window->action_window);

    window->action_window = ACTION_WINDOW_create(habit_id);
    abort_if(!window->action_window, "ACTION_WINDOW_create failed");

    window_stack_push(window->action_window->raw_window, true);

CLEANUP:
    return rval;
}

// TIMER CALLBACKS
// -----------------------------------------------------------------------------
static void request_list(void *callback_context)
{
    int rval = 0;

    rval = NETWORK_request_list();
    abort_if(rval, "NETWORK_request_list failed");

CLEANUP:
    if(rval) CRASH();
}

// WINDOW HANDLERS
// -----------------------------------------------------------------------------

static void on_load(Window *raw_window)
{
    Layer *root_layer = window_get_root_layer(raw_window);
    GRect bounds = layer_get_bounds(root_layer);
    struct ListWindow *window = window_get_user_data(raw_window);

    window->list_layer = LIST_LAYER_create(bounds);
    LIST_LAYER_attach_to_window(window->list_layer, raw_window);
    LIST_LAYER_add_to_layer(window->list_layer, root_layer);

    window->list_layer->callbacks = (struct ListLayerCallbacks) {
            .on_select = on_select,
            .on_select_context = window
    };
}

static void on_unload(Window *raw_window)
{
    struct ListWindow *list_window = window_get_user_data(raw_window);
    LIST_LAYER_destroy(list_window->list_layer);
    ACTION_WINDOW_destroy(list_window->action_window);
}

static void on_appear(Window *raw_window)
{
    int rval = 0;
    struct ListWindow *window = window_get_user_data(raw_window);

    LIST_LAYER_clear(window->list_layer);

    struct NetworkCallbacks *callbacks = NETWORK_get_callbacks();
    abort_if(!callbacks, "NETWORK_get_callbacks failed");
    callbacks->on_count = on_count;
    callbacks->on_count_context = window;
    callbacks->on_habit = on_habit;
    callbacks->on_habit_context = window;

    app_timer_register(500, request_list, window);

CLEANUP:
    if(rval) CRASH();
}

static void on_disappear(Window *raw_window)
{
    int rval = 0;

    struct NetworkCallbacks *callbacks = NETWORK_get_callbacks();
    abort_if(!callbacks, "NETWORK_get_callbacks failed");
    callbacks->on_count = 0;
    callbacks->on_habit = 0;

CLEANUP:
    if(rval) CRASH();
}

// -----------------------------------------------------------------------------

struct ListWindow *LIST_WINDOW_create()
{
    WindowHandlers handlers = {
        .load = on_load,
        .unload = on_unload,
        .appear = on_appear,
        .disappear = on_disappear,
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