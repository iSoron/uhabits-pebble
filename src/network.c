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

#define MAX_HABIT_COUNT 50

struct NetworkState
{
    int habit_count;

    int habit_current;

    struct NetworkCallbacks *callbacks;
};

static int process_ok(struct NetworkState *state)
{
    int rval = 0;
    abort_if(!state, "state is null");

    struct NetworkCallbacks *callbacks = state->callbacks;
    abort_if(!callbacks, "callbacks is null");

    if(!callbacks->on_ok) goto CLEANUP;
    rval = callbacks->on_ok(callbacks->on_ok_context);
    abort_if(rval, "state->on_ok failed");

CLEANUP:
    return rval;
}

static int request_next_habit(struct NetworkState *state)
{
    int rval = 0;
    abort_if(!state, "state is null");

    if(state->habit_current >= state->habit_count)
        goto CLEANUP;

    DictionaryIterator *dict;
    rval = app_message_outbox_begin(&dict);
    abort_if(rval, "app_message_outbox_begin failed");

    rval = dict_write_cstring(dict, 0, "FETCH");
    abort_if(rval, "dict_write_cstring failed");

    rval = dict_write_int32(dict, 1, state->habit_current);
    abort_if(rval, "dict_write_cstring failed");

    rval = app_message_outbox_send();
    abort_if(rval, "app_message_outbox_send failed");

    state->habit_current++;

CLEANUP:
    return rval;
}

static int process_count(DictionaryIterator *iter, struct NetworkState *state)
{
    int rval = 0;
    abort_if(!state, "state is null");
    struct NetworkCallbacks *callbacks = state->callbacks;

    Tuple *tuple = dict_find(iter, 1);
    abort_if(!tuple, "tuple is null");

    int count = tuple->value->int32;

    if(callbacks->on_count)
    {
        rval = callbacks->on_count(count, callbacks->on_count_context);
        abort_if(rval, "state->on_count failed");
    }

    if(count > MAX_HABIT_COUNT) count = MAX_HABIT_COUNT;
    state->habit_count = count;
    state->habit_current = 0;

    rval = request_next_habit(state);
    abort_if(rval, "request_next_habit failed");

CLEANUP:
    return rval;
}

static int process_habit(DictionaryIterator *iter, struct NetworkState *state)
{
    int rval = 0;
    struct NetworkCallbacks *callbacks = state->callbacks;

    Tuple *tuple = dict_find(iter, 1);
    abort_if(!tuple, "tuple is null");
    int id = tuple->value->int32;

    tuple = dict_find(iter, 2);
    abort_if(!tuple, "tuple is null");
    char *name = tuple->value->cstring;

    tuple = dict_find(iter, 3);
    abort_if(!tuple, "tuple is null");
    int checkmark = tuple->value->int32;

    if(callbacks->on_habit)
    {
        void *ctx = callbacks->on_habit_context;
        rval = callbacks->on_habit(id, name, checkmark, ctx);
        abort_if(rval, "state->on_habit failed");

        rval = request_next_habit(state);
        abort_if(rval, "request_next_habit failed");
    }

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

    log_debug("--> %s", "COUNT");

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

    log_debug("--> %s", "COUNT");

CLEANUP:
    return rval;
}

static void NETWORK_on_received(DictionaryIterator *iter, void *context)
{
    int rval = 0;
    struct NetworkState *state = context;
    abort_if(!state, "state is null");

    Tuple *tuple = dict_find(iter, 0);
    abort_if(!tuple, "tuple is null");

    char *action = tuple->value->cstring;
    log_debug("<-- %s", action);

    if(strcmp(action, "COUNT") == 0)
    {
        rval = process_count(iter, state);
        abort_if(rval, "process_count failed");
    }
    else if(strcmp(action, "HABIT") == 0)
    {
        rval = process_habit(iter, state);
        abort_if(rval, "process_habit failed");
    }
    else if(strcmp(action, "OK") == 0)
    {
        rval = process_ok(state);
        abort_if(rval, "process_ok failed");
    }

CLEANUP:
    return;
}

struct NetworkCallbacks * NETWORK_get_callbacks()
{
    struct NetworkState *state = app_message_get_context();
    return state->callbacks;
}

int NETWORK_register()
{
    int rval = 0;

    struct NetworkState *state = 0;
    state = (struct NetworkState*) malloc(sizeof(struct NetworkState));
    abort_if(!state, "could not allocate state");

    struct NetworkCallbacks *callbacks = 0;
    callbacks = (struct NetworkCallbacks*) malloc(sizeof(struct NetworkCallbacks));
    abort_if(!callbacks, "could not allocate callbacks");

    state->callbacks = callbacks;
    callbacks->on_ok = 0;
    callbacks->on_habit = 0;
    callbacks->on_count = 0;
    callbacks->on_ok_context = 0;
    callbacks->on_habit_context = 0;
    callbacks->on_count_context = 0;

    app_message_register_inbox_received(NETWORK_on_received);
    app_message_open(1024, 1024);
    app_message_set_context(state);

CLEANUP:
    return rval;
}

void NETWORK_cleanup()
{
    struct NetworkState *state = app_message_get_context();
    free(state->callbacks);
    free(state);
    app_message_deregister_callbacks();
}