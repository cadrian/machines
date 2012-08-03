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

static void print_entry_(machines_state_t *state) {
     char *state_name = state ? (char*)state->payload(state) : "<nil>";
     printf("enter %s\n", state_name);
}
static machines_state_observer_t print_entry() {
     machines_state_observer_t result = { (machines_state_observer_fn)print_entry_, NULL };
     return result;
}

static void print_exit_(machines_state_t *state) {
     char *state_name = state ? (char*)state->payload(state) : "<nil>";
     printf("exit %s\n", state_name);
}
static machines_state_observer_t print_exit() {
     machines_state_observer_t result = { (machines_state_observer_fn)print_exit_, NULL };
     return result;
}

static int always_true_(machines_state_t *from_state, machines_state_t *to_state, char *transition) {
     char *from_name = from_state ? (char*)from_state->payload(from_state) : "<nil>";
     char *to_name = to_state ? (char*)to_state->payload(to_state) : "<nil>";
     printf("[%s] from %s to %s\n", transition, from_name, to_name);
     return 1;
}
static machines_state_transition_t always_true(char *tag) {
     machines_state_transition_t result = { (machines_state_transition_fn)always_true_, tag };
     return result;
}

typedef struct {
     char *tag;
     int *count;
} counter_t;

static int countdown_(machines_state_t *from_state, machines_state_t *to_state, counter_t *counter) {
     int result = *(counter->count);
     char *from_name = from_state ? (char*)from_state->payload(from_state) : "<nil>";
     char *to_name = to_state ? (char*)to_state->payload(to_state) : "<nil>";
     printf("[%s] from %s to %s (%d)\n", counter->tag, from_name, to_name, result);
     if (result) {
          *(counter->count) = result - 1;
     }
     return result;
}
static machines_state_transition_t countdown(char *tag, int *count) {
     counter_t *counter = (counter_t*)malloc(sizeof(counter_t));
     machines_state_transition_t result = { (machines_state_transition_fn)countdown_, counter };
     counter->tag = tag;
     counter->count = count;
     return result;
}

typedef struct {
     char *tag;
     int *value;
} condition_t;

static int iff_(machines_state_t *from_state, machines_state_t *to_state, condition_t *condition) {
     int result = *(condition->value);
     char *from_name = from_state ? (char*)from_state->payload(from_state) : "<nil>";
     char *to_name = to_state ? (char*)to_state->payload(to_state) : "<nil>";
     printf("[%s] from %s to %s (%d)\n", condition->tag, from_name, to_name, result);
     return result;
}
static machines_state_transition_t iff(char *tag, int *value) {
     condition_t *cond = (condition_t*)malloc(sizeof(condition_t));
     machines_state_transition_t result = { (machines_state_transition_fn)iff_, cond };
     cond->tag = tag;
     cond->value = value;
     return result;
}
