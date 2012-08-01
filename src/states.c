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

#include "machines_states.h"

typedef struct machine_state_impl machine_state_impl_t;
typedef struct machine_state_observer machine_state_observer_t;
typedef struct machine_state_transition machine_state_transition_t;

struct machine_state_impl {
     machine_state_t fn;
     void *payload;

     machine_state_impl_t *parent ;
     machine_state_impl_t *start  ;
     machine_state_impl_t *current;

     machine_state_observer_t *on_entry;
     machine_state_observer_t *on_exit;

     machine_state_transition_t *transition;
};

typedef struct machine_chain machine_chain_t;
struct machine_chain {
     machine_chain_t *next;
};

typedef void (*machine_observer_run_fn)(machine_state_t *state, void *payload);
struct machine_state_observer {
     machine_chain_t chain;
     machine_observer_run_fn run;
     void *payload;
};

struct machine_state_transition {
     machine_chain_t chain;
     machine_state_transition_fn transition;
     machine_state_impl_t *target;
     void *payload;
};

static void run_observers(machine_state_t *state, machine_state_observer_t *observer) {
     while (observer) {
          observer->run(state, observer->payload);
          observer = (machine_state_observer_t*)observer->chain.next;
     }
}

static machine_state_t *current(machine_state_impl_t *this) {
     return (machine_state_t*)this->current;
}

static void* payload(machine_state_impl_t *this) {
     return this->payload;
}

static machine_chain_t *add_chain(machine_chain_t *chain, machine_chain_t *new) {
     machine_chain_t *result;
     if (chain) {
          result = chain;
          while (chain->next) {
               chain = chain->next;
          }
          chain->next = new;
     }
     else {
          result = new;
     }
     new->next = NULL;
     return result;
}

static machine_state_observer_t *add_observer(machine_state_observer_t *observer, machine_observer_run_fn fn, void *payload) {
     machine_state_observer_t *new = (machine_state_observer_t*)malloc(sizeof(machine_state_observer_t));
     new->run = fn;
     new->payload = payload;

     return (machine_state_observer_t*)add_chain(&(observer->chain), &(new->chain));
}

static void add_entry(machine_state_impl_t *this, machine_state_on_entry_fn fn, void *payload) {
     this->on_entry = add_observer(this->on_entry, (machine_observer_run_fn)fn, payload);
}

static void add_exit(machine_state_impl_t *this, machine_state_on_entry_fn fn, void *payload) {
     this->on_exit = add_observer(this->on_exit, (machine_observer_run_fn)fn, payload);
}

static void add_transition(machine_state_impl_t *this, machine_state_transition_fn fn, machine_state_t *target, void *payload) {
     machine_state_transition_t *new;

     new = (machine_state_transition_t*)malloc(sizeof(machine_state_transition_t));
     new->transition = fn;
     new->target = (machine_state_impl_t*)target;
     new->payload = payload;

     this->transition = (machine_state_transition_t*)add_chain((machine_chain_t*)this->transition, (machine_chain_t*)new);
}

static void trigger(machine_state_impl_t *this) {
     machine_state_transition_t *transition;

     if (this->current) {
          transition = this->current->transition;
          if (!transition) {
               run_observers((machine_state_t*)this->current, this->current->on_exit);
               this->current = NULL;
          }
          else {
               bool_t done = false;
               do {
                    done = transition->transition((machine_state_t*)this->current, (machine_state_t*)transition->target, transition->payload);
                    if (done) {
                         run_observers((machine_state_t*)this->current, this->current->on_exit);
                         this->current = transition->target;
                    }
                    else {
                         transition = (machine_state_transition_t*)transition->chain.next;
                    }
               } while (transition && !done);
          }
     }
     else if (this->start) {
          this->current = this->start;
     }

     if (this->current) {
          run_observers((machine_state_t*)this->current, this->current->on_entry);
          trigger(this->current);
     }
}

static machine_state_t fn = {
     (machine_state_current_fn       )current       ,
     (machine_state_payload_fn       )payload       ,
     (machine_state_add_entry_fn     )add_entry     ,
     (machine_state_add_exit_fn      )add_exit      ,
     (machine_state_add_transition_fn)add_transition,
     (machine_state_trigger_fn       )trigger       ,
};

__PUBLIC__ machine_state_t *machines_new_state(void *payload, machine_state_t *parent, machine_state_t *child_startup) {
     machine_state_impl_t *result = (machine_state_impl_t*)malloc(sizeof(machine_state_impl_t));
     machine_state_impl_t *parent_impl = (machine_state_impl_t*)parent;
     result->fn = fn;
     result->payload = payload;
     result->parent = (machine_state_impl_t*)parent;
     result->start = (machine_state_impl_t*)child_startup;
     result->current = NULL;
     result->on_entry = NULL;
     result->on_exit = NULL;
     result->transition = NULL;

     return &(result->fn);
}
