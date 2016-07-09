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

// STYLE
//--------------------------------------------------------------------------------

#define HIGHLIGHT_BACKGROUND_COLOR GColorFolly
#define HIGHLIGHT_FOREGROUND_COLOR GColorWhite
#define NORMAL_BACKGROUND_COLOR GColorBlack
#define NORMAL_FOREGROUND_COLOR GColorWhite
#define CELL_HEIGHT 36

//--------------------------------------------------------------------------------

void LIST_WINDOW_push();

void LIST_WINDOW_destroy();

int LIST_WINDOW_allocate(int count);

int LIST_WINDOW_add_habit(int new_id, char *new_checkmark, int i);
