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
#include "action_window.h"
#include "../../layers/border_layer.h"
#include "../../util.h"
#include "action_menu_layer.h"
#include "style.h"
#include "../animation/animation_window.h"
#include "../../network.h"

#ifdef CLION
#include <resource_ids.auto.h>
#endif

// NETWORK CALLBACKS
// -----------------------------------------------------------------------------

static int on_ok(void *callback_context)
{
    struct AnimationWindow *window = ANIMATION_WINDOW_create(
            RESOURCE_ID_CONFIRM_SEQUENCE);

    window_stack_push(window->raw_window, true);
    return 0;
}

// ACTION MENU LAYER CALLBACKS
// -----------------------------------------------------------------------------

static int on_select(void *callback_context)
{
    int rval = 0;

    struct ActionWindow *window = callback_context;

    rval = NETWORK_request_toggle(window->habit_id);
    abort_if(rval, "NETWORK_request_toggle failed");

    struct NetworkCallbacks *net_callbacks = NETWORK_get_callbacks();
    abort_if(!net_callbacks, "NETWORK_get_callbacks failed");

    net_callbacks->on_ok = on_ok;

CLEANUP:
    return rval;
}

// -----------------------------------------------------------------------------

static int add_menu_layer(struct ActionWindow *window,
                          struct Layer *root_layer,
                          struct GRect bounds)
{
    int rval = 0;

    struct ActionMenuLayer *menu_layer = 0;
    menu_layer = ACTION_MENU_LAYER_create(bounds);
    abort_if(!menu_layer, "ACTION_MENU_LAYER_create failed");

    window->menu_layer = menu_layer;
    ACTION_MENU_LAYER_attach_to_window(menu_layer, window->raw_window);
    ACTION_MENU_LAYER_add_to_layer(menu_layer, root_layer);

    menu_layer->callback_context = window;
    menu_layer->callbacks = (struct ActionMenuLayerCallbacks) {
            .on_select = on_select
    };

CLEANUP:
    return rval;
}

static int add_border_layer(struct ActionWindow *window,
                            struct Layer *root_layer,
                            const struct GRect frame)
{
    int rval = 0;

    struct BorderLayer *border_layer = 0;

    border_layer = BORDER_LAYER_create(frame);
    abort_if(!border_layer, "BORDER_LAYER_create failed");

    border_layer->color = BORDER_COLOR;
    border_layer->width = BORDER_WIDTH;

    window->border_layer = border_layer;
    BORDER_LAYER_add_to_layer(border_layer, root_layer);

CLEANUP:
    return rval;
}

// callback: WindowHandler
static void on_load(Window *raw_window)
{
    struct ActionWindow *window = window_get_user_data(raw_window);
    Layer *root_layer = window_get_root_layer(raw_window);
    GRect bounds = layer_get_bounds(root_layer);
    add_menu_layer(window, root_layer, bounds);
    add_border_layer(window, root_layer, bounds);
}

// callback: WindowHandler
static void on_unload(Window *raw_window)
{
    struct ActionWindow *window = window_get_user_data(raw_window);
    ACTION_MENU_LAYER_destroy(window->menu_layer);
    BORDER_LAYER_destroy(window->border_layer);
    ANIMATION_WINDOW_destroy(window->animation_window);
}

static void set_raw_window_handlers(Window *raw_window)
{
    WindowHandlers handlers = {
            .load = on_load,
            .unload = on_unload
    };

    window_set_window_handlers(raw_window, handlers);
}

struct ActionWindow *ACTION_WINDOW_create(int habit_id)
{
    struct ActionWindow *window = 0;

    window = (struct ActionWindow*) malloc(sizeof(struct ActionWindow));
    if(!window) return NULL;

    window->habit_id = habit_id;

    Window *raw_window = window_create();
    if(!raw_window) return NULL;

    window->raw_window = raw_window;
    window_set_user_data(raw_window, window);
    set_raw_window_handlers(raw_window);

    window->animation_window = 0;

    return window;
}

void ACTION_WINDOW_destroy(struct ActionWindow *window)
{
    if(!window) return;
    window_destroy(window->raw_window);
    free(window);
}