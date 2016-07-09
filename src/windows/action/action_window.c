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
#include "style.h"
#include "../../layers/border_layer.h"
#include "../../util.h"

static struct Window *WINDOW = 0;
static struct MenuLayer *MENU_LAYER = 0;
static struct BorderLayer *BORDER_LAYER = 0;

char *ACTIONS[] = { "   Check", "   View stats"};

// DRAWING
//------------------------------------------------------------------------------

void draw_left_border(GContext *ctx, const Layer *cell_layer)
{
    GRect bounds = layer_get_bounds(cell_layer);
    GRect rect = GRect(bounds.origin.x, bounds.origin.y, 5, bounds.size.h);
    graphics_context_set_fill_color(ctx, GColorFolly);
    graphics_fill_rect(ctx, rect, 0, GCornerNone);
}

// MENU LAYER CALLBACKS
//------------------------------------------------------------------------------

static uint16_t get_num_rows(MenuLayer *menu_layer,
                             uint16_t section_index,
                             void *context)
{
    return 2;
}

static void draw_row(GContext *ctx,
                     const Layer *cell_layer,
                     MenuIndex *cell_index,
                     void *extra)
{
    int n = cell_index->row;
    menu_cell_basic_draw(ctx, cell_layer, ACTIONS[n], NULL, NULL);
    draw_left_border(ctx, cell_layer);
}

static int16_t get_cell_height(struct MenuLayer *menu_layer,
                               MenuIndex *cell_index,
                               void *context)
{
    return CELL_HEIGHT;
}

static void select_click(MenuLayer *menu_layer, MenuIndex *index, void *context)
{
}

// WINDOWS HANDLERS
//------------------------------------------------------------------------------

void add_menu_layer(Window *window,
                    Layer *root_layer,
                    GRect bounds)
{
    MenuLayerCallbacks callbacks = {
        .get_num_rows = get_num_rows,
        .draw_row = draw_row,
        .get_cell_height = get_cell_height,
        .select_click = select_click,
    };

    MENU_LAYER = menu_layer_create(bounds);
    menu_layer_set_click_config_onto_window(MENU_LAYER, window);
    menu_layer_set_callbacks(MENU_LAYER, NULL, callbacks);
    menu_layer_set_highlight_colors(MENU_LAYER, HIGHLIGHT_BACKGROUND_COLOR,
        HIGHLIGHT_FOREGROUND_COLOR);
    menu_layer_set_normal_colors(MENU_LAYER, NORMAL_BACKGROUND_COLOR,
        NORMAL_FOREGROUND_COLOR);

    layer_add_child(root_layer, menu_layer_get_layer(MENU_LAYER));
}

static int add_border_layer(Window *window, Layer *root_layer, GRect frame)
{
    int rval = 0;

    BORDER_LAYER = (struct BorderLayer*) malloc(sizeof(struct BorderLayer));
    abort_if(!BORDER_LAYER, "could not allocate BORDER_LAYER");

    BORDER_LAYER_init(BORDER_LAYER, frame);
    BORDER_LAYER->color = GColorFolly;

    layer_add_child(root_layer, BORDER_LAYER->raw_layer);

CLEANUP:
    return rval;
}

static void on_load(Window *window)
{
    Layer *root_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(root_layer);
    add_menu_layer(window, root_layer, bounds);
    add_border_layer(window, root_layer, bounds);
}

static void on_unload(Window *window)
{
    menu_layer_destroy(MENU_LAYER);
    free(BORDER_LAYER);
}

//--------------------------------------------------------------------------------

void ACTION_WINDOW_push()
{
    WindowHandlers handlers = {
        .load = on_load,
        .unload = on_unload
    };

    WINDOW = window_create();
    window_set_window_handlers(WINDOW, handlers);
    window_stack_push(WINDOW, true);
}

void ACTION_WINDOW_destroy()
{
    window_destroy(WINDOW);
}