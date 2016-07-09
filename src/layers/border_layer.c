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

#include "border_layer.h"

struct BorderLayer* get_data(const struct Layer *layer)
{
    struct BorderLayer **data_region = layer_get_data(layer);
    struct BorderLayer *border = *data_region;
    return border;
}

// callback: LayerUpdateProc
static void on_update(struct Layer *layer, GContext *context)
{
    struct BorderLayer *border = get_data(layer);
    struct GRect bounds = layer_get_bounds(layer);
    struct GRect rect = {
        .origin = bounds.origin,
        .size.h = bounds.size.h,
        .size.w = border->width
    };
    graphics_context_set_fill_color(context, border->color);
    graphics_fill_rect(context, rect, 0, GCornerNone);
}

static struct Layer* create_raw_layer(struct BorderLayer *border,
                                      struct GRect frame)
{
    size_t data_region_size = sizeof(struct BorderLayer *);
    struct Layer *layer = layer_create_with_data(frame, data_region_size);
    if(!layer) return NULL;

    struct BorderLayer **data_region = layer_get_data(layer);
    *data_region = border;

    return layer;
}

void BORDER_LAYER_add_to_layer(struct BorderLayer *border,
                               struct Layer *root_layer)
{
    struct Layer *raw_layer = border->raw_layer;
    layer_add_child(root_layer, raw_layer);
}

struct BorderLayer *BORDER_LAYER_create(struct GRect frame)
{
    struct BorderLayer *border_layer = 0;

    border_layer = (struct BorderLayer*) malloc(sizeof(struct BorderLayer));
    if(!border_layer) return NULL;

    struct Layer *raw_layer = create_raw_layer(border_layer, frame);
    if(!raw_layer) return NULL;

    layer_set_update_proc(raw_layer, on_update);

    border_layer->raw_layer = raw_layer;
    border_layer->color = GColorWhite;
    border_layer->width = 5;

    return border_layer;
}

void BORDER_LAYER_destroy(struct BorderLayer *border)
{
    if(!border) return;

    layer_destroy(border->raw_layer);
    free(border);
}