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

typedef struct machine_state machine_state_t;
typedef struct machine_visitor machine_visitor_t;

typedef void (*machine_state_on_entry_fn)(machine_state_t *state, void *payload);
typedef void (*machine_state_on_exit_fn)(machine_state_t *state, void *payload);
typedef bool_t (*machine_state_transition_fn)(machine_state_t *from_state, machine_state_t *to_state, void *payload);

typedef machine_state_t*(*machine_state_current_fn)(machine_state_t *this);
typedef void*(*machine_state_payload_fn)(machine_state_t *this);
typedef void (*machine_state_add_entry_fn)(machine_state_t *this, machine_state_on_entry_fn fn, void *payload);
typedef void (*machine_state_add_exit_fn)(machine_state_t *this, machine_state_on_exit_fn fn, void *payload);
typedef void (*machine_state_add_transition_fn)(machine_state_t *this, machine_state_t *target, machine_state_transition_fn fn, void *payload);
typedef void (*machine_state_entry_at_fn)(machine_state_t *this, machine_state_t *child);
typedef void (*machine_state_exit_at_fn)(machine_state_t *this, machine_state_t *child);
typedef void (*machine_state_trigger_fn)(machine_state_t *this);

struct machine_state {
     machine_state_current_fn        current       ;
     machine_state_payload_fn        payload       ;
     machine_state_add_entry_fn      add_entry     ;
     machine_state_add_exit_fn       add_exit      ;
     machine_state_add_transition_fn add_transition;
     machine_state_entry_at_fn       entry_at      ;
     machine_state_exit_at_fn        exit_at       ;
     machine_state_trigger_fn        trigger       ;
};

__PUBLIC__ machine_state_t *machines_new_state(void *payload, machine_state_t *parent);

#endif /* _MACHINES_STATES_H_ */
