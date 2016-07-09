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
#include "network.h"
#include "util.h"
#include "windows/list_window.h"

#define MAX_HABIT_COUNT 50
static int HABIT_COUNT;
static int HABIT_CURRENT;

// LIST WINDOW
//------------------------------------------------------------------------------

static int request_next_habit()
{
    int rval = 0;

    if(HABIT_CURRENT >= HABIT_COUNT)
        goto CLEANUP;

    DictionaryIterator *dict;
    rval = app_message_outbox_begin(&dict);
    abort_if(rval, "app_message_outbox_begin failed");

    rval = dict_write_cstring(dict, 0, "FETCH");
    abort_if(rval, "dict_write_cstring failed");

    rval = dict_write_int32(dict, 1, HABIT_CURRENT);
    abort_if(rval, "dict_write_cstring failed");

    rval = app_message_outbox_send();
    abort_if(rval, "app_message_outbox_send failed");

    HABIT_CURRENT++;

CLEANUP:
    return rval;
}

static int process_count(DictionaryIterator *iter)
{
    int rval = 0;

    Tuple *tuple = dict_find(iter, 1);
    abort_if(!tuple, "tuple is null");

    int count = tuple->value->int32;

    rval = LIST_WINDOW_allocate(count);
    abort_if(rval, "LIST_WINDOW_allocate failed");

    if(count > MAX_HABIT_COUNT) count = MAX_HABIT_COUNT;
    HABIT_COUNT = count;
    HABIT_CURRENT = 0;

    rval = request_next_habit();
    abort_if(rval, "request_next_habit failed");

    CLEANUP:
    return rval;
}

static int process_habit(DictionaryIterator *iter)
{
    int rval = 0;

    // id
    Tuple *tuple = dict_find(iter, 1);
    abort_if(!tuple, "tuple is null");
    int id = tuple->value->int32;

    // name
    tuple = dict_find(iter, 2);
    abort_if(!tuple, "tuple is null");
    char *name = tuple->value->cstring;

    // checkmark value
    tuple = dict_find(iter, 3);
    abort_if(!tuple, "tuple is null");
    int checkmark = tuple->value->int32;

    rval = LIST_WINDOW_add_habit(id, name, checkmark);
    abort_if(rval, "LIST_WINDOW_add_habit failed");

    rval = request_next_habit();
    abort_if(rval, "request_next_habit failed");

CLEANUP:
    return rval;
}

int NETWORK_request_list()
{
    int rval = 0;

    DictionaryIterator *dict;
    rval = app_message_outbox_begin(&dict);
    abort_if(rval, "app_message_outbox_begin failed");

    rval = dict_write_cstring(dict, 0, "COUNT");
    abort_if(rval, "dict_write_cstring failed");

    rval = app_message_outbox_send();
    abort_if(rval, "app_message_outbox_send failed");

CLEANUP:
    return rval;
}

int NETWORK_request_toggle(int id)
{
    int rval = 0;

    DictionaryIterator *dict;
    rval = app_message_outbox_begin(&dict);
    abort_if(rval, "app_message_outbox_begin failed");

    rval = dict_write_cstring(dict, 0, "TOGGLE");
    abort_if(rval, "dict_write_cstring failed");

    rval = dict_write_int32(dict, 1, id);
    abort_if(rval, "dict_write_cstring failed");

    rval = app_message_outbox_send();
    abort_if(rval, "app_message_outbox_send failed");

    CLEANUP:
    return rval;
}

//------------------------------------------------------------------------------

static void NETWORK_on_received(DictionaryIterator *iter, void *context)
{
    int rval = 0;

    Tuple *tuple = dict_find(iter, 0);
    abort_if(!tuple, "tuple is null");

    char *action = tuple->value->cstring;

    if(strcmp(action, "COUNT") == 0)
    {
        rval = process_count(iter);
        abort_if(rval, "process_count failed");
    }
    else if(strcmp(action, "HABIT") == 0)
    {
        rval = process_habit(iter);
        abort_if(rval, "process_habit failed");
    }
    else if(strcmp(action, "OK") == 0)
    {
        rval = NETWORK_request_list();
        abort_if(rval, "NETWORK_request_list failed");
    }

CLEANUP:
    return;
}

int NETWORK_register()
{
    app_message_register_inbox_received(NETWORK_on_received);
    app_message_open(1024, 1024);
    return 0;
}