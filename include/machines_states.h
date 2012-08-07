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

#ifndef _MACHINES_STATES_H_
#define _MACHINES_STATES_H_

/**
 * @file
 * A header for the Machines states.
 */

#include "machines_shared.h"

typedef struct machines_state machines_state_t;
typedef struct machines_visitor machines_visitor_t;

typedef void (*machines_state_observer_fn)(machines_state_t *state, void *payload);
typedef struct {
     machines_state_observer_fn fn;
     void *payload;
} machines_state_observer_t;

typedef int (*machines_state_transition_fn)(machines_state_t *from_state, machines_state_t *to_state, void *payload);
typedef struct {
     machines_state_transition_fn fn;
     void *payload;
} machines_state_transition_t;

typedef void (*machines_state_free_fn)(machines_state_t *this);
typedef machines_state_t*(*machines_state_current_fn)(machines_state_t *this);
typedef void*(*machines_state_payload_fn)(machines_state_t *this);
typedef void (*machines_state_add_entry_fn)(machines_state_t *this, machines_state_observer_t on_entry);
typedef void (*machines_state_add_exit_fn)(machines_state_t *this, machines_state_observer_t on_exit);
typedef void (*machines_state_add_transition_fn)(machines_state_t *this, machines_state_t *target, machines_state_transition_t transition);
typedef void (*machines_state_entry_at_fn)(machines_state_t *this, machines_state_t *child);
typedef void (*machines_state_exit_at_fn)(machines_state_t *this, machines_state_t *child);
typedef void (*machines_state_trigger_fn)(machines_state_t *this);

struct machines_state {
     machines_state_free_fn           free          ;
     machines_state_current_fn        current       ;
     machines_state_payload_fn        payload       ;
     machines_state_add_entry_fn      add_entry     ;
     machines_state_add_exit_fn       add_exit      ;
     machines_state_add_transition_fn add_transition;
     machines_state_entry_at_fn       entry_at      ;
     machines_state_exit_at_fn        exit_at       ;
     machines_state_trigger_fn        trigger       ;
};

__PUBLIC__ machines_state_t *machines_new_state(void *payload, machines_state_t *parent, cad_memory_t memory);

__PUBLIC__ machines_state_observer_t machines_observer(machines_state_observer_fn fn, void *payload);
__PUBLIC__ machines_state_transition_t machines_transition(machines_state_transition_fn fn, void *payload);

#endif /* _MACHINES_STATES_H_ */
