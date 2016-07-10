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
#pragma once

struct AnimationWindow
{
    uint32_t current_frame;
    uint32_t num_frames;//

    struct Layer *animation_layer;
    struct TextLayer *text_layer;
    struct AppTimer *timer;//
    struct GDrawCommandSequence *sequence;//
    struct Window *raw_window;//
};

struct AnimationWindow *ANIMATION_WINDOW_create(uint32_t sequence_id);

void ANIMATION_WINDOW_destroy(struct AnimationWindow *window);