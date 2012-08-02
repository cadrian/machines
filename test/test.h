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
  along with Machines.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>

#include "machines.h"

static void assert_(int test, const char *message, int line, const char *file) {
     int *a = 0;
     static int counter = 0;
     counter++;
     if (!test) {
          fprintf(stderr, "Assertion %d (%s:%d) failed: %s\n", counter, file, line, message);
          fflush(stderr);
          *a = counter;
     }
}

#define assert(t) assert_((t), #t, __LINE__, __FILE__)

static void print_entry_(machine_state_t *state) {
     char *state_name = state ? (char*)state->payload(state) : "<nil>";
     printf("enter %s\n", state_name);
}
static machine_state_on_entry_fn print_entry = (machine_state_on_entry_fn)print_entry_;

static void print_exit_(machine_state_t *state) {
     char *state_name = state ? (char*)state->payload(state) : "<nil>";
     printf("exit %s\n", state_name);
}
static machine_state_on_exit_fn print_exit = (machine_state_on_exit_fn)print_exit_;

static bool_t always_true_(machine_state_t *from_state, machine_state_t *to_state, char *transition) {
     char *from_name = from_state ? (char*)from_state->payload(from_state) : "<nil>";
     char *to_name = to_state ? (char*)to_state->payload(to_state) : "<nil>";
     printf("[%s] from %s to %s\n", transition, from_name, to_name);
     return true;
}
static machine_state_transition_fn always_true = (machine_state_transition_fn)always_true_;

typedef struct {
     char *name;
     int count;
} counter_t;

static bool_t countdown_(machine_state_t *from_state, machine_state_t *to_state, counter_t *counter) {
     bool_t result = false;
     char *from_name = from_state ? (char*)from_state->payload(from_state) : "<nil>";
     char *to_name = to_state ? (char*)to_state->payload(to_state) : "<nil>";
     printf("[%s] from %s to %s (%d)\n", counter->name, from_name, to_name, counter->count);
     if (counter->count) {
          counter->count--;
          result = true;
     }
     return result;
}
static machine_state_transition_fn countdown = (machine_state_transition_fn)countdown_;

static counter_t* counter(char *name, int initial) {
     counter_t *result = (counter_t*)malloc(sizeof(counter_t));
     result->name = name;
     result->count = initial;
     return result;
}

typedef struct {
     char *name;
     bool_t value;
} condition_t;

static bool_t iff_(machine_state_t *from_state, machine_state_t *to_state, condition_t *condition) {
     return condition->value;
}
static machine_state_transition_fn iff = (machine_state_transition_fn)iff_;

static condition_t *condition(char *name) {
     condition_t *result = (condition_t*)malloc(sizeof(condition_t));
     result->name = name;
     result->value = false;
     return result;
}
