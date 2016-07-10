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
#include "animation_window.h"
#include "style.h"
#include "../../util.h"

#define FRAME_DELAY 33

void next_frame(struct AnimationWindow *window);

static void on_tick(void *context)
{
    struct AnimationWindow *window = context;
    window->timer = 0;

    if(window->current_frame + 1 < window->num_frames)
        next_frame(window);
    else
    {
        window_stack_pop(true);
        window_stack_pop(true);
    }
}

static void set_timer(struct AnimationWindow *window)
{
    window->timer = app_timer_register(FRAME_DELAY, on_tick, window);
}

void next_frame(struct AnimationWindow *window)
{
    window->current_frame++;
    layer_mark_dirty(window->animation_layer);
    set_timer(window);
}

static void update_proc(Layer *layer, GContext *ctx)
{
    void **data_region = layer_get_data(layer);
    struct AnimationWindow *window = *data_region;

    GRect bounds = layer_get_bounds(layer);
    GSize seq_bounds = gdraw_command_sequence_get_bounds_size(window->sequence);

    GDrawCommandFrame *frame = gdraw_command_sequence_get_frame_by_index(
            window->sequence, window->current_frame);

    if(!frame) return;

    uint16_t x = (uint16_t) ((bounds.size.w - seq_bounds.w) / 2);
    uint16_t y = (uint16_t) ((bounds.size.h - seq_bounds.h) / 2);
    gdraw_command_frame_draw(ctx, window->sequence, frame, GPoint(x, y));
}

static void on_load(Window *raw_window)
{
    struct AnimationWindow *window = window_get_user_data(raw_window);
    struct Layer *root_layer = window_get_root_layer(raw_window);
    GRect bounds = layer_get_bounds(root_layer);


    int16_t animation_y = (int16_t) (bounds.size.h * 5 / 6);
    GRect animation_bounds = GRect(bounds.origin.x, bounds.origin.y,
            bounds.size.w, animation_y);

    int16_t text_y = (int16_t) (bounds.size.h * 6 / 10);
    GRect text_bounds = GRect((int16_t) (bounds.origin.x + PADDING), text_y,
            (int16_t) (bounds.size.w - 2 * PADDING), bounds.size.h - text_y);
    
    window->animation_layer = layer_create_with_data(animation_bounds,
            sizeof(struct AnimationWindow *));

    void **data_region = layer_get_data(window->animation_layer);
    *data_region = window;

    window->text_layer = text_layer_create(text_bounds);
    text_layer_set_text(window->text_layer, "Checkmark Added");
    text_layer_set_background_color(window->text_layer, BACKGROUND_COLOR);
    text_layer_set_text_color(window->text_layer, GColorBlack);
    text_layer_set_text_alignment(window->text_layer, GTextAlignmentCenter);
    text_layer_set_overflow_mode(window->text_layer, GTextOverflowModeWordWrap);

    GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
    text_layer_set_font(window->text_layer, font);

    layer_set_update_proc(window->animation_layer, update_proc);
    layer_add_child(root_layer, text_layer_get_layer(window->text_layer));
    layer_add_child(root_layer, window->animation_layer);
}

static void on_unload(Window *raw_window)
{
    struct AnimationWindow *window = window_get_user_data(raw_window);
    layer_destroy(window->animation_layer);
    text_layer_destroy(window->text_layer);
    if(window->timer) app_timer_cancel(window->timer);
}

static int set_sequence(struct AnimationWindow *window, uint32_t sequence_id)
{
    int rval = 0;
    window->sequence = gdraw_command_sequence_create_with_resource(sequence_id);
    abort_if(!window->sequence, "gdraw_command_sequence_create failed");

    window->current_frame = 0;
    window->num_frames = gdraw_command_sequence_get_num_frames(
            window->sequence);

CLEANUP:
    return rval;
}

struct AnimationWindow *ANIMATION_WINDOW_create(uint32_t sequence_id)
{
    struct AnimationWindow *window = 0;
    window = malloc(sizeof(struct AnimationWindow));
    if(!window) return NULL;

    set_sequence(window, sequence_id);
    set_timer(window);

    window->animation_layer = 0;
    window->text_layer = 0;

    struct Window *raw_window = window_create();
    if(!raw_window) return NULL;

    window->raw_window = raw_window;

    window_set_user_data(raw_window, window);
    window_set_window_handlers(raw_window, (WindowHandlers) {
            .load = on_load,
            .unload = on_unload,
    });

    window_set_background_color(raw_window, BACKGROUND_COLOR);

    return window;
}

void ANIMATION_WINDOW_destroy(struct AnimationWindow *window)
{
    if(!window) return;
    window_destroy(window->raw_window);
    gdraw_command_sequence_destroy(window->sequence);
    //app_timer_cancel(window->timer);
    free(window);
}


