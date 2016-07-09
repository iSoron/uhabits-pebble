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

static struct Layer* create_layer(struct BorderLayer *border,
                                  struct GRect frame)
{
    size_t data_region_size = sizeof(struct BorderLayer *);
    struct Layer *layer = layer_create_with_data(frame, data_region_size);
    layer_set_update_proc(layer, on_update);

    struct BorderLayer **data_region = layer_get_data(layer);
    *data_region = border;

    return layer;
}

void BORDER_LAYER_init(struct BorderLayer *border, struct GRect frame)
{
    border->raw_layer = create_layer(border, frame);
    border->color = GColorWhite;
    border->width = 5;
}

void BORDER_LAYER_destroy(struct BorderLayer *border)
{
    layer_destroy(border->raw_layer);
}