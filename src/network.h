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

struct NetworkCallbacks
{
    void *on_count_context;

    void *on_habit_context;

    void *on_ok_context;

    int (*on_count)(int count, void *callback_context);

    int (*on_habit)(int id,
                    char *name,
                    int checkmark,
                    void *callback_context);

    int (*on_ok)(void *callback_context);
};

int NETWORK_request_list();

int NETWORK_request_toggle(int id);

struct NetworkCallbacks* NETWORK_get_callbacks();

int NETWORK_register();

void NETWORK_cleanup();