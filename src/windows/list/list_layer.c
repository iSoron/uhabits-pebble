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

#include "list_layer.h"
#include "style.h"
#include "../../util.h"

#define MAX_NAME_LENGTH 20

static int free_habits(struct ListLayer *layer)
{
    for(int i = 0; i < layer->habit_count; i++) free(layer->habit_names[i]);
    free(layer->habit_names);
    free(layer->habit_checkmarks);
    free(layer->habit_ids);
    layer->habit_count = 0;

    return 0;
}

int LIST_LAYER_allocate(struct ListLayer *layer, int count)
{
    int rval = 0;

    free_habits(layer);

    layer->habit_names = (char **) malloc(count * sizeof(char *));
    abort_if(!layer->habit_names, "could not allocate habit_names");

    for(int i = 0; i < count; i++)
    {
        layer->habit_names[i] = (char *) malloc(MAX_NAME_LENGTH * sizeof(char));
        abort_if(!layer->habit_names[i], "could not allocate habit_names");
    }

    layer->habit_checkmarks = (int *) malloc(count * sizeof(int));
    abort_if(!layer->habit_checkmarks, "could not allocate habit_checkmarks");

    layer->habit_ids = (int *) malloc(count * sizeof(int));
    abort_if(!layer->habit_ids, "could not allocate habit_ids");

CLEANUP:
    return rval;
}

int LIST_LAYER_add_habit(struct ListLayer *layer,
                         int new_id,
                         char *new_name,
                         int new_checkmark)
{
    uint16_t count = layer->habit_count;

    char *name = layer->habit_names[count];
    strncpy(name, new_name, MAX_NAME_LENGTH);
    menu_layer_reload_data(layer->menu_layer);

    layer->habit_ids[count] = new_id;
    layer->habit_checkmarks[count] = new_checkmark;
    layer->habit_count++;

    return 0;
}

static uint16_t get_num_rows(MenuLayer *menu_layer,
                             uint16_t section_index,
                             void *callback_context)
{
    struct ListLayer *list_layer = callback_context;
    return list_layer->habit_count;
}

static void on_draw_row(GContext *ctx,
                        const Layer *cell_layer,
                        MenuIndex *cell_index,
                        void *callback_context)
{
    int n = cell_index->row;
    struct ListLayer *list_layer = callback_context;
    char *name = list_layer->habit_names[n];

    menu_cell_basic_draw(ctx, cell_layer, name, NULL, NULL);
}

static int16_t get_cell_height(struct MenuLayer *menu_layer,
                               MenuIndex *cell_index,
                               void *callback_context)
{
    return CELL_HEIGHT;
}

static void select_click(struct MenuLayer *menu_layer,
                         struct MenuIndex *index,
                         void *callback_context)
{
    struct ListLayer *list_layer = callback_context;
    if(!list_layer->callbacks.on_select) return;

    list_layer->callbacks.on_select(list_layer->callback_context);
}

static void request_list(void *context)
{
    struct ListLayer *layer = context;

    LIST_LAYER_allocate(layer, 4);
    LIST_LAYER_add_habit(layer, 0, "Wake up early", 0);
    LIST_LAYER_add_habit(layer, 1, "Meditate", 1);
    LIST_LAYER_add_habit(layer, 2, "Exercise", 1);
    LIST_LAYER_add_habit(layer, 3, "Go to school", 0);
}

static void set_menu_layer_callbacks(struct ListLayer *list_layer,
                                     MenuLayer *menu_layer)
{
    MenuLayerCallbacks callbacks = {
            .get_num_rows = get_num_rows,
            .draw_row = on_draw_row,
            .get_cell_height = get_cell_height,
            .select_click = select_click,
    };

    menu_layer_set_callbacks(menu_layer, list_layer, callbacks);
}

static void set_menu_layer_styles(MenuLayer *menu_layer)
{
    menu_layer_set_highlight_colors(menu_layer, HIGHLIGHT_BACKGROUND_COLOR,
            HIGHLIGHT_FOREGROUND_COLOR);
    menu_layer_set_normal_colors(menu_layer, NORMAL_BACKGROUND_COLOR,
            NORMAL_FOREGROUND_COLOR);
}

void LIST_LAYER_attach_to_window(struct ListLayer *layer, struct Window *window)
{
    menu_layer_set_click_config_onto_window(layer->menu_layer, window);
    app_timer_register(500, request_list, layer);
}

void LIST_LAYER_add_to_layer(struct ListLayer *layer, struct Layer *root_layer)
{
    struct Layer *raw_layer = menu_layer_get_layer(layer->menu_layer);
    layer_add_child(root_layer, raw_layer);
}

struct ListLayer* LIST_LAYER_create(struct GRect bounds)
{
    struct ListLayer *list_layer = 0;
    list_layer = (struct ListLayer*) malloc(sizeof(struct ListLayer));
    if(!list_layer) return NULL;

    MenuLayer *menu_layer = menu_layer_create(bounds);
    if(!menu_layer) return NULL;
    set_menu_layer_styles(menu_layer);
    set_menu_layer_callbacks(list_layer, menu_layer);
    list_layer->menu_layer = menu_layer;

    list_layer->habit_count = 0;

    return list_layer;
}

void LIST_LAYER_destroy(struct ListLayer *list_layer)
{
    if(!list_layer) return;
    free_habits(list_layer);
    menu_layer_destroy(list_layer->menu_layer);
    free(list_layer);
}