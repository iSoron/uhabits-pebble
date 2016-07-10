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
#include "action_menu_layer.h"
#include "style.h"

const char *ACTIONS[] = {
        "   Check",
        "   View stats"
};

// callback: MenuLayerSelectCallback
static void on_click(struct MenuLayer *menu_layer,
                     MenuIndex *cell_index,
                     void *callback_context)
{
    struct ActionMenuLayer *action_menu = callback_context;
    if(!action_menu->callbacks.on_select) return;
    action_menu->callbacks.on_select(action_menu->callback_context);
}

// MenuLayerGetNumberOfRowsInSectionsCallback
static uint16_t on_get_num_rows(MenuLayer *menu_layer,
                                uint16_t section_index,
                                void *context)
{
    return 2;
}

// MenuLayerDrawRowCallback
static void on_draw_row(GContext *ctx,
                        const Layer *cell_layer,
                        MenuIndex *cell_index,
                        void *extra)
{
    int n = cell_index->row;
    menu_cell_basic_draw(ctx, cell_layer, ACTIONS[n], NULL, NULL);
}

// MenuLayerGetCellHeightCallback
static int16_t on_get_cell_height(struct MenuLayer *menu_layer,
                                  MenuIndex *cell_index,
                                  void *context)
{
    return CELL_HEIGHT;
}

static void set_menu_layer_callbacks(struct ActionMenuLayer *action_menu,
                                     struct MenuLayer *menu)
{
    MenuLayerCallbacks callbacks = {
            .get_num_rows = on_get_num_rows,
            .draw_row = on_draw_row,
            .get_cell_height = on_get_cell_height,
            .select_click = on_click,
    };

    menu_layer_set_callbacks(menu, action_menu, callbacks);
    menu_layer_set_highlight_colors(menu, HIGHLIGHT_BACKGROUND_COLOR,
            HIGHLIGHT_FOREGROUND_COLOR);
    menu_layer_set_normal_colors(menu, NORMAL_BACKGROUND_COLOR,
            NORMAL_FOREGROUND_COLOR);
}

void ACTION_MENU_LAYER_add_to_layer(struct ActionMenuLayer *action_menu,
                                    struct Layer *root_layer)
{
    struct Layer *raw_layer = menu_layer_get_layer(action_menu->menu_layer);
    layer_add_child(root_layer, raw_layer);
}

void ACTION_MENU_LAYER_attach_to_window(struct ActionMenuLayer *action_menu,
                                        struct Window *window)
{
    struct MenuLayer *menu = action_menu->menu_layer;
    menu_layer_set_click_config_onto_window(menu, window);
};

struct ActionMenuLayer* ACTION_MENU_LAYER_create(struct GRect bounds)
{
    struct ActionMenuLayer *action_menu = 0;
    action_menu = (struct ActionMenuLayer*) malloc(sizeof(struct ActionMenuLayer));
    if(!action_menu) return NULL;

    struct MenuLayer *menu = menu_layer_create(bounds);
    if(!menu) return NULL;

    set_menu_layer_callbacks(action_menu, menu);
    action_menu->menu_layer = menu;

    return action_menu;
}

void ACTION_MENU_LAYER_destroy(struct ActionMenuLayer *action_menu)
{
    if(!action_menu) return;
    if(action_menu->menu_layer) menu_layer_destroy(action_menu->menu_layer);
    free(action_menu);
}