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
#include "screens/list/list_window.h"
#include "network.h"
#include "util.h"

int main(void)
{
    int rval = 0;
    struct ListWindow *window = 0;

    rval = NETWORK_register();
    abort_if(rval, "NETWORK_register failed");

    window = LIST_WINDOW_create();
    abort_if(!window, "LIST_WINDOW_create failed");

    window_stack_push(window->raw_window, true);

    app_event_loop();

CLEANUP:
    LIST_WINDOW_destroy(window);
    NETWORK_cleanup();
    return rval;
}
