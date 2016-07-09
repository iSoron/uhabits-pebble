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
#include "../../network.h"
#include "../../util.h"
#include "list_window.h"
#include "../action/action_window.h"
#include "style.h"

static Window *WINDOW = 0;
static MenuLayer *MENU_LAYER = 0;

static uint16_t HABIT_COUNT = 0;
static char **HABIT_NAMES = 0;
static int *HABIT_IDS = 0;
static int *HABIT_CHECKMARKS = 0;
static size_t MAX_NAME_LENGTH = 20;

static GBitmap *TICK_WHITE;
static GBitmap *TICK_BLACK;
static GBitmap *CROSS_BLACK;
static GBitmap *CROSS_WHITE;

int LIST_WINDOW_allocate(int count);

int LIST_WINDOW_add_habit(int new_id, char *new_name, int new_checkmark);

void free_habits()
{
    HABIT_COUNT = 0;
    for (int i = 0; i < HABIT_COUNT; i++) free(HABIT_NAMES[i]);
    free(HABIT_NAMES);
    free(HABIT_IDS);
}

// MENU LAYER CALLBACKS
//------------------------------------------------------------------------------

static uint16_t get_num_rows(MenuLayer *menu_layer,
                             uint16_t section_index,
                             void *context)
{
    return HABIT_COUNT;
}

GBitmap *getTickBitmap(int highlighted)
{
    return highlighted ? TICK_WHITE : TICK_BLACK;
}

GBitmap *getCrossBitmap(int highlighted)
{
    return highlighted ? CROSS_WHITE : CROSS_BLACK;
}

GBitmap *getIcon(int is_checked, int is_highlighted)
{
    if(is_checked == 0) return getCrossBitmap(is_highlighted);
    else return getTickBitmap(is_highlighted);
}

static void draw_row(GContext *ctx,
                     const Layer *cell_layer,
                     MenuIndex *cell_index,
                     void *context)
{
    int n = cell_index->row;
    char *name = HABIT_NAMES[n];
    int is_checked = HABIT_CHECKMARKS[n];
    int is_highlighted = menu_cell_layer_is_highlighted(cell_layer);

    GBitmap *icon = getIcon(is_checked, is_highlighted);
    menu_cell_basic_draw(ctx, cell_layer, name, NULL, NULL);
}

static int16_t get_cell_height(struct MenuLayer *menu_layer,
                               MenuIndex *cell_index,
                               void *context)
{
    return CELL_HEIGHT;
}

static void select_click(MenuLayer *menu_layer, MenuIndex *index, void *context)
{
    int n = index->row;
    int id = HABIT_IDS[n];
//    NETWORK_request_toggle(id);

    ACTION_WINDOW_push();
}

static void request_list(void *context)
{
    LIST_WINDOW_allocate(5);
    LIST_WINDOW_add_habit(0, "Wake up early", 0);
    LIST_WINDOW_add_habit(1, "Meditate", 1);
    LIST_WINDOW_add_habit(2, "Exercise", 1);
    LIST_WINDOW_add_habit(3, "Go to school", 0);

    NETWORK_request_list();
}

// WINDOWS HANDLERS
//------------------------------------------------------------------------------

static void on_load(Window *window)
{
    Layer *root_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(root_layer);

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
    app_timer_register(500, request_list, 0);
}

static void on_unload(Window *window)
{
    menu_layer_destroy(MENU_LAYER);
}

//--------------------------------------------------------------------------------

int LIST_WINDOW_allocate(int count)
{
    int rval = 0;

    if(HABIT_COUNT > 0) free_habits();

    HABIT_NAMES = (char**) malloc(count * sizeof(char*));
    abort_if(!HABIT_NAMES, "could not allocate HABIT_NAMES");

    for(int i = 0; i < count; i++)
    {
        HABIT_NAMES[i] = (char*) malloc(MAX_NAME_LENGTH * sizeof(char));
        abort_iff(!HABIT_NAMES[i], "could not allocate HABIT_NAMES[%d]", i);
    }

    HABIT_CHECKMARKS = (int *) malloc(count * sizeof(int));
    abort_if(!HABIT_CHECKMARKS, "could not allocate HABIT_CHECKMARKS");

    HABIT_IDS = (int *) malloc(count * sizeof(int));
    abort_if(!HABIT_IDS, "could not allocate HABIT_IDS");

CLEANUP:
    return rval;
}

int LIST_WINDOW_add_habit(int new_id, char *new_name, int new_checkmark)
{
    char *name = HABIT_NAMES[HABIT_COUNT];
    strncpy(name, new_name, MAX_NAME_LENGTH);
    menu_layer_reload_data(MENU_LAYER);

    HABIT_IDS[HABIT_COUNT] = new_id;
    HABIT_CHECKMARKS[HABIT_COUNT] = new_checkmark;
    HABIT_COUNT++;

    return 0;
}

void create_bitmaps()
{
    CROSS_BLACK = gbitmap_create_with_resource(RESOURCE_ID_CROSS_BLACK);
    CROSS_WHITE = gbitmap_create_with_resource(RESOURCE_ID_CROSS_WHITE);
    TICK_WHITE = gbitmap_create_with_resource(RESOURCE_ID_TICK_WHITE);
    TICK_BLACK = gbitmap_create_with_resource(RESOURCE_ID_TICK_BLACK);
}

void free_bitmaps()
{
    gbitmap_destroy(CROSS_BLACK);
    gbitmap_destroy(CROSS_WHITE);
    gbitmap_destroy(TICK_WHITE);
    gbitmap_destroy(TICK_BLACK);
}

void LIST_WINDOW_push()
{
    NETWORK_register();

    create_bitmaps();

    WindowHandlers handlers = {
        .load = on_load,
        .unload = on_unload
    };

    WINDOW = window_create();
    window_set_window_handlers(WINDOW, handlers);
    window_stack_push(WINDOW, true);
}

void LIST_WINDOW_destroy()
{
    free_bitmaps();
    free_habits();
    window_destroy(WINDOW);
}