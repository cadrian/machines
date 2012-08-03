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

typedef struct machines_state_impl machines_state_impl_t;

typedef struct machines_chain machines_chain_t;
struct machines_chain {
     machines_chain_t *next;
};

typedef struct machines_state_chain_observer machines_state_chain_observer_t;
typedef struct machines_state_chain_transition machines_state_chain_transition_t;

struct machines_state_impl {
     machines_state_t fn;
     void *payload;

     machines_state_impl_t *parent ;
     machines_state_impl_t *entry  ;
     machines_state_impl_t *exit   ;
     machines_state_impl_t *current;

     machines_state_chain_observer_t *on_entry;
     machines_state_chain_observer_t *on_exit;

     machines_state_chain_transition_t *transition;
};

struct machines_state_chain_observer {
     machines_chain_t chain;
     machines_state_observer_t agent;
};

struct machines_state_chain_transition {
     machines_chain_t chain;
     machines_state_impl_t *target;
     machines_state_transition_t agent;
};

static void run_observers(machines_state_t *state, machines_state_chain_observer_t *observer) {
     while (observer) {
          observer->agent.fn(state, observer->agent.payload);
          observer = (machines_state_chain_observer_t*)observer->chain.next;
     }
}

static machines_state_t *current(machines_state_impl_t *this) {
     return (machines_state_t*)this->current;
}

static void* payload(machines_state_impl_t *this) {
     return this->payload;
}

static machines_chain_t *add_chain(machines_chain_t *chain, machines_chain_t *new) {
     machines_chain_t *result;
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

static machines_state_chain_observer_t *add_observer(machines_state_chain_observer_t *observer, machines_state_observer_t agent) {
     machines_state_chain_observer_t *new = (machines_state_chain_observer_t*)malloc(sizeof(machines_state_chain_observer_t));
     new->agent = agent;
     return (machines_state_chain_observer_t*)add_chain(&(observer->chain), &(new->chain));
}

static void add_entry(machines_state_impl_t *this, machines_state_observer_t entry) {
     this->on_entry = add_observer(this->on_entry, entry);
}

static void add_exit(machines_state_impl_t *this, machines_state_observer_t exit) {
     this->on_exit = add_observer(this->on_exit, exit);
}

static void add_transition(machines_state_impl_t *this, machines_state_t *target, machines_state_transition_t agent) {
     machines_state_chain_transition_t *new;
     new = (machines_state_chain_transition_t*)malloc(sizeof(machines_state_chain_transition_t));
     new->agent = agent;
     new->target = (machines_state_impl_t*)target;
     this->transition = (machines_state_chain_transition_t*)add_chain((machines_chain_t*)this->transition, (machines_chain_t*)new);
}

static void entry_at(machines_state_impl_t *this, machines_state_impl_t *child) {
     this->entry = child;
}

static void exit_at(machines_state_impl_t *this, machines_state_impl_t *child) {
     this->exit = child;
}

static void trigger(machines_state_impl_t *this) {
     machines_state_chain_transition_t *transition;
     machines_state_impl_t *old = this->current;

     if (old) {
          trigger(old);
          if (old->current == NULL) {
               transition = old->transition;
               if (!transition) {
                    run_observers((machines_state_t*)old, old->on_exit);
                    this->current = NULL;
               }
               else {
                    int done;
                    do {
                         done = transition->agent.fn((machines_state_t*)old, (machines_state_t*)transition->target, transition->agent.payload);
                         if (done) {
                              if (transition->target != old) {
                                   run_observers((machines_state_t*)this->current, this->current->on_exit);
                              }
                              this->current = transition->target;
                         }
                         else {
                              transition = (machines_state_chain_transition_t*)transition->chain.next;
                         }
                    } while (transition && !done);

                    if (!done && old == this->exit) {
                         run_observers((machines_state_t*)old, old->on_exit);
                         this->current = NULL;
                         if (this->parent) {
                              trigger(this->parent);
                         }
                    }
               }
          }
     }
     else {
          this->current = this->entry;
     }

     if (this->current && this->current != old) {
          run_observers((machines_state_t*)this->current, this->current->on_entry);
          trigger(this->current);
     }
}

static machines_state_t fn = {
     (machines_state_current_fn       )current       ,
     (machines_state_payload_fn       )payload       ,
     (machines_state_add_entry_fn     )add_entry     ,
     (machines_state_add_exit_fn      )add_exit      ,
     (machines_state_add_transition_fn)add_transition,
     (machines_state_entry_at_fn      )entry_at      ,
     (machines_state_exit_at_fn       )exit_at       ,
     (machines_state_trigger_fn       )trigger       ,
};

__PUBLIC__ machines_state_t *machines_new_state(void *payload, machines_state_t *parent) {
     machines_state_impl_t *result = (machines_state_impl_t*)malloc(sizeof(machines_state_impl_t));
     machines_state_impl_t *parent_impl = (machines_state_impl_t*)parent;
     result->fn = fn;
     result->payload = payload;
     result->parent = (machines_state_impl_t*)parent;
     result->current = NULL;
     result->on_entry = NULL;
     result->on_exit = NULL;
     result->transition = NULL;

     return &(result->fn);
}

__PUBLIC__ machines_state_observer_t machines_observer(machines_state_observer_fn fn, void *payload) {
     machines_state_observer_t result = { fn, payload };
     return result;
}

__PUBLIC__ machines_state_transition_t machines_transition(machines_state_transition_fn fn, void *payload) {
     machines_state_transition_t result = { fn, payload };
     return result;
}
