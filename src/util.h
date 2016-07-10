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

#include <pebble.h>

#define abort_if(cond, msg) if(cond) { \
        APP_LOG(APP_LOG_LEVEL_ERROR, msg " (%d)", rval); \
        rval = 1; goto CLEANUP; }

#define abort_iff(cond, msg, ...) if(cond) { \
        APP_LOG(APP_LOG_LEVEL_ERROR, msg " (%d)", __VA_ARGS__, rval); \
        rval = 1; goto CLEANUP; }

#define log_debug(msg, ...) APP_LOG(APP_LOG_LEVEL_DEBUG, msg, __VA_ARGS__)

static void CRASH()
{
    APP_LOG(APP_LOG_LEVEL_ERROR, "%s", "forcing crash");
    int *x = NULL;
    *x = 42;
}