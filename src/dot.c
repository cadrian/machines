/*
  This file is part of Machines.

  Machines is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, version 3 of the License.

  Machines is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with YacJP.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "machines_dot.h"

typedef struct machines_dot_impl {
     machines_dot_t fn;
     char *name;

     cad_memory_t memory;

     char *buffer;
     int buffer_count; //< `buffer_count == strlen(buffer)`
     int buffer_capacity;

     int transition_count;
} machines_dot_impl_t;

typedef struct machines_dot_observer {
     machines_dot_impl_t *machine;
     char *label;
     int set;
} machines_dot_observer_t;

static  __PRINTF__ void extend(machines_dot_impl_t *machine, char *format, ...) {
     va_list arg;
     int n, c = machine->buffer_capacity;
     char *new_buffer;

     va_start(arg, format);
     n = vsnprintf("", 0, format, arg);
     va_end(arg);

     if (c - machine->buffer_count < n) {
          do {
               c *= 2;
          } while (c - machine->buffer_count < n);
          new_buffer = (char*)machine->memory.malloc(c);
          memcpy(new_buffer, machine->buffer, machine->buffer_count);
          machine->memory.free(machine->buffer);
          machine->buffer = new_buffer;
          machine->buffer_capacity = c;
     }

     va_start(arg, format);
     vsnprintf(machine->buffer + machine->buffer_count, machine->buffer_capacity - machine->buffer_count, format, arg);
     machine->buffer_count += n;
     va_end(arg);

}

static void dot_observer_(machines_state_t *state, machines_dot_observer_t *payload) {
     if (!payload->set) {
          extend(payload->machine, "\t%s[label=\"%s\"];\n", payload->label);
          payload->set = 1;
     }
}

static void dot_nop_(machines_state_t *state, machines_dot_observer_t *payload) {
     // NOP
}

static machines_state_observer_t dot_entry(machines_dot_impl_t *this, char *label) {
     machines_dot_observer_t *payload = (machines_dot_observer_t*)this->memory.malloc(sizeof(machines_dot_observer_t));
     payload->machine = this;
     payload->label = label;
     payload->set = 0;
     return machines_observer((machines_state_observer_fn)dot_observer_, payload);
}

static machines_state_observer_t dot_exit(machines_dot_impl_t *this, char *label) {
     return machines_observer((machines_state_observer_fn)dot_nop_, NULL);
}

typedef struct machines_dot_transition {
     machines_dot_impl_t *machine;
     char *label;
     machines_state_transition_t transition;
} machines_dot_transition_t;

static int dot_transition_(machines_state_t *from_state, machines_state_t *to_state, machines_dot_transition_t *payload) {
     int result = payload->transition.fn(from_state, to_state, payload->transition.payload);
     if (result) {
          payload->machine->transition_count++;
          extend(payload->machine, "\t%s [label=\"(%d)\"]\n", payload->label, payload->machine->transition_count);
     }
     return result;
}

machines_state_transition_t dot_transition(machines_dot_impl_t *this, char *label, machines_state_transition_t transition) {
     machines_dot_transition_t *payload = (machines_dot_transition_t*)this->memory.malloc(sizeof(machines_dot_transition_t));
     payload->machine = this;
     payload->label = label;
     payload->transition = transition;
     return machines_transition((machines_state_transition_fn)dot_transition_, payload);
}

int dot_generate(machines_dot_impl_t *this, char *buffer, int buffer_size) {
     return snprintf(buffer, buffer_size, "digraph %s {\n%*s}\n", this->name, this->buffer_count, this->buffer);
}

static void dot_free(machines_dot_impl_t *this) {
     // TODO
}

static machines_dot_t fn = {
     (machines_dot_free_fn      ) dot_free,
     (machines_dot_on_entry_fn  ) dot_entry,
     (machines_dot_on_exit_fn   ) dot_exit,
     (machines_dot_transition_fn) dot_transition,
     (machines_dot_generate_fn  ) dot_generate,
};

#define INITIAL_BUFFER_SIZE 4096

__PUBLIC__ machines_dot_t *machines_new_dot(char *name, cad_memory_t memory) {
     machines_dot_impl_t *result = (machines_dot_impl_t*)memory.malloc(sizeof(machines_dot_impl_t));
     char *buffer = (char*)memory.malloc(INITIAL_BUFFER_SIZE);
     result->fn = fn;
     result->memory = memory;
     result->name = name;
     result->buffer = buffer;
     result->buffer_count = 0;
     result->buffer_capacity = INITIAL_BUFFER_SIZE;
     result->transition_count = 0;
     return &(result->fn);
}
